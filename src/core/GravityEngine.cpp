//
// Created by milosz on 3/12/26.
//

#include "GravityEngine.h"
#include <cmath>
#include <omp.h>

void GravityEngine::addBody(const CelestialBody& body) {
    bodies.push_back(body);
}

void GravityEngine::setBodies(const std::vector<CelestialBody>& newBodies) {
    bodies = newBodies;
    // Prime F(t=0) immediately so the integrator is ready.
    initializeForces();
}

void GravityEngine::saveInitialState() {
    // Compute correct forces before snapshotting, so that resetInitialState()
    // restores a state where the Verlet integrator can start cleanly.
    initializeForces();
    initialBodies = bodies;
}

void GravityEngine::resetInitialState() {
    bodies = initialBodies;  // restores positions, velocities AND forces from snapshot
    for (auto& body : bodies)
        body.clearTrailHistory();
    // F(t=0) is already embedded in initialBodies — no need to recompute.
}

// ---------------------------------------------------------------------------
// initializeForces: compute F(t=0) without advancing time.
// Must be called once before the first update().
// ---------------------------------------------------------------------------
void GravityEngine::initializeForces() {
    for (auto& body : bodies)
        body.resetForce();

    if (m_forceAlgorithm == ForceAlgorithm::Naive)
        calculateForcesNaive();
    else if (m_forceAlgorithm == ForceAlgorithm::BarnesHut)
        calculateForcesBarnesHut();
}

// ---------------------------------------------------------------------------
// update: one complete Velocity Verlet step.
//
//   Requires F(t) to be valid in body.force before this call.
//   After this call, body.force holds F(t+dt).
// ---------------------------------------------------------------------------
void GravityEngine::update(double deltaTime) {
    if (bodies.empty()) return;

    // Step 1 + half-kick:  x(t+dt)  and  v(t + dt/2)
    for (auto& body : bodies)
        body.updatePosition(deltaTime);

    // Reset and recompute F(t+dt) at new positions.
    for (auto& body : bodies)
        body.resetForce();

    if (m_forceAlgorithm == ForceAlgorithm::Naive)
        calculateForcesNaive();
    else if (m_forceAlgorithm == ForceAlgorithm::BarnesHut)
        calculateForcesBarnesHut();

    // Step 3 — second half-kick:  v(t+dt)
    for (auto& body : bodies) {
        body.updateVelocity(deltaTime);
        body.updateTrail(m_maxTrailLength, m_minTrailDistanceSq);
    }
}

const std::vector<CelestialBody>& GravityEngine::getBodies() const { return bodies; }

double GravityEngine::calculateTotalEnergy() const {
    double totalKinetic = 0.0;
    double totalPotential = 0.0;
    int n = static_cast<int>(bodies.size());

    #pragma omp parallel for reduction(+:totalKinetic)
    for (int i = 0; i < n; ++i) {
        double speedSquared = bodies[i].velocity.squaredNorm();
        totalKinetic += 0.5 * bodies[i].mass * speedSquared;
    }

    double softeningSquared = softening * softening;
    #pragma omp parallel for schedule(dynamic) reduction(+:totalPotential)
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            double r_sq = (bodies[i].position - bodies[j].position).squaredNorm() + softeningSquared;
            totalPotential += -(G * bodies[i].mass * bodies[j].mass) / std::sqrt(r_sq);
        }
    }

    return totalKinetic + totalPotential;
}

// ---------------------------------------------------------------------------
// Octree insertion
// ---------------------------------------------------------------------------
void GravityEngine::insertToNode(OctreeNode* node, CelestialBody* newBody, int depth) {
    if (depth > 64) {
        float newTotalMass = node->totalMass + newBody->mass;
        if (newTotalMass > 0.0f) {
            node->centerOfMass = (node->centerOfMass * node->totalMass +
                                  newBody->position * newBody->mass) / newTotalMass;
        }
        node->totalMass = newTotalMass;
        return;
    }

    if (node->isLeaf() && node->body == nullptr) {
        node->body = newBody;
        node->totalMass = newBody->mass;
        node->centerOfMass = newBody->position;
        return;
    }

    if (node->body) {
        CelestialBody* existingBody = node->body;
        node->body = nullptr;
        node->totalMass = 0;
        node->centerOfMass = Eigen::Vector3f::Zero();
        insertToNode(node, existingBody, depth + 1);
    }

    float newTotalMass = node->totalMass + newBody->mass;
    node->centerOfMass = (node->centerOfMass * node->totalMass +
                          newBody->position * newBody->mass) / newTotalMass;
    node->totalMass = newTotalMass;

    int octant = 0;
    if (newBody->position.x() >= node->center.x()) octant |= 1;
    if (newBody->position.y() >= node->center.y()) octant |= 2;
    if (newBody->position.z() >= node->center.z()) octant |= 4;

    if (!node->children[octant]) {
        float newSize  = node->size / 2.0f;
        float offset   = newSize / 2.0f;          // = node->size / 4
        Eigen::Vector3f newCenter = node->center;
        newCenter.x() += (octant & 1) ? offset : -offset;
        newCenter.y() += (octant & 2) ? offset : -offset;
        newCenter.z() += (octant & 4) ? offset : -offset;
        node->children[octant] = std::make_unique<OctreeNode>(newCenter, newSize);
    }

    insertToNode(node->children[octant].get(), newBody, depth + 1);
}

// ---------------------------------------------------------------------------
// Barnes-Hut force evaluation
// ---------------------------------------------------------------------------
Eigen::Vector3f GravityEngine::calculateBarnesHutForce(OctreeNode* node, CelestialBody* target,
                                                        float theta, double G, double softeningSq) {
    if (!node || node->body == target) return Eigen::Vector3f::Zero();

    Eigen::Vector3f r_vec = node->centerOfMass - target->position;
    float dist_sq = r_vec.squaredNorm();

    // Use softened distance in the BH opening-angle criterion to avoid div-by-zero.
    float dist_softened = std::sqrt(dist_sq + static_cast<float>(softeningSq));

    if (node->isLeaf() || (node->size / dist_softened < theta)) {
        double r_sq = dist_sq + softeningSq;
        double r    = std::sqrt(r_sq);
        double forceMagnitude = (G * target->mass * node->totalMass) / (r_sq * r);
        return static_cast<float>(forceMagnitude) * r_vec;
    }

    Eigen::Vector3f totalForce = Eigen::Vector3f::Zero();
    for (int i = 0; i < 8; ++i) {
        if (node->children[i])
            totalForce += calculateBarnesHutForce(node->children[i].get(), target, theta, G, softeningSq);
    }
    return totalForce;
}

// ---------------------------------------------------------------------------
// Naive O(N^2) force calculation
// ---------------------------------------------------------------------------
void GravityEngine::calculateForcesNaive() {
    int n = static_cast<int>(bodies.size());
    double softeningSquared = softening * softening;
    int numThreads = omp_get_max_threads();
    m_maxThreads = numThreads;

    if (m_localForces.size() != static_cast<size_t>(numThreads * n))
        m_localForces.resize(numThreads * n);

    std::fill(m_localForces.begin(), m_localForces.end(), Eigen::Vector3f::Zero());

    #pragma omp parallel
    {
        int threadId = omp_get_thread_num();
        int offset   = threadId * n;

        #pragma omp for schedule(dynamic)
        for (int i = 0; i < n; ++i) {
            for (int j = i+1; j < n; ++j) {
                Eigen::Vector3f r_vec = bodies[j].position - bodies[i].position;
                double r_sq = r_vec.squaredNorm() + softeningSquared;
                double r    = std::sqrt(r_sq);
                double forceMagnitude = (G * bodies[i].mass * bodies[j].mass) / (r_sq * r);
                Eigen::Vector3f forceVec = static_cast<float>(forceMagnitude) * r_vec;
                m_localForces[offset + i] += forceVec;
                m_localForces[offset + j] -= forceVec;
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        Eigen::Vector3f totalForce = Eigen::Vector3f::Zero();
        for (int t = 0; t < numThreads; ++t)
            totalForce += m_localForces[t * n + i];
        bodies[i].addForce(totalForce);
    }
}

// ---------------------------------------------------------------------------
// Barnes-Hut O(N log N) force calculation
// ---------------------------------------------------------------------------
void GravityEngine::calculateForcesBarnesHut() {
    if (bodies.empty()) return;

    float minX = bodies[0].position.x(), maxX = minX;
    float minY = bodies[0].position.y(), maxY = minY;
    float minZ = bodies[0].position.z(), maxZ = minZ;

    for (size_t i = 1; i < bodies.size(); ++i) {
        const auto& pos = bodies[i].position;
        minX = std::min(minX, pos.x());  maxX = std::max(maxX, pos.x());
        minY = std::min(minY, pos.y());  maxY = std::max(maxY, pos.y());
        minZ = std::min(minZ, pos.z());  maxZ = std::max(maxZ, pos.z());
    }

    Eigen::Vector3f center((minX + maxX) / 2.0f,
                           (minY + maxY) / 2.0f,
                           (minZ + maxZ) / 2.0f);
    float maxSize = std::max({maxX - minX, maxY - minY, maxZ - minZ}) * 1.01f;

    auto root = std::make_unique<OctreeNode>(center, maxSize);
    for (auto& body : bodies)
        insertToNode(root.get(), &body, 0);

    double softeningSq = softening * softening;
    int n = static_cast<int>(bodies.size());

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; ++i) {
        Eigen::Vector3f f = calculateBarnesHutForce(root.get(), &bodies[i], m_theta, G, softeningSq);
        bodies[i].addForce(f);
    }
}
