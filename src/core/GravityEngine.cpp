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
    initializeForces();
}

void GravityEngine::saveInitialState() {
    initializeForces();
    initialBodies = bodies;
}

void GravityEngine::resetInitialState() {
    bodies = initialBodies;
    for (auto& body : bodies)
        body.clearTrailHistory();
}

void GravityEngine::initializeForces() {
    for (auto& body : bodies)
        body.resetForce();

    if (m_forceAlgorithm == ForceAlgorithm::Naive)
        calculateForcesNaive();
    else if (m_forceAlgorithm == ForceAlgorithm::BarnesHut)
        calculateForcesBarnesHut();
}

void GravityEngine::update(double deltaTime) {
    if (bodies.empty()) return;

    for (auto& body : bodies)
        body.updatePosition(deltaTime);

    for (auto& body : bodies)
        body.resetForce();

    if (m_forceAlgorithm == ForceAlgorithm::Naive)
        calculateForcesNaive();
    else if (m_forceAlgorithm == ForceAlgorithm::BarnesHut)
        calculateForcesBarnesHut();

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
// Helper: compute the octant index for a body relative to a node's centre.
// ---------------------------------------------------------------------------
static int getOctant(const OctreeNode* node, const CelestialBody* body) {
    int oct = 0;
    if (body->position.x() >= node->center.x()) oct |= 1;
    if (body->position.y() >= node->center.y()) oct |= 2;
    if (body->position.z() >= node->center.z()) oct |= 4;
    return oct;
}

// ---------------------------------------------------------------------------
// Helper: create child node for given octant if it doesn't exist yet.
// ---------------------------------------------------------------------------
static void createChild(OctreeNode* node, int octant) {
    if (node->children[octant]) return;
    float newSize = node->size / 2.0f;
    float offset  = newSize / 2.0f;
    Eigen::Vector3f nc = node->center;
    nc.x() += (octant & 1) ? offset : -offset;
    nc.y() += (octant & 2) ? offset : -offset;
    nc.z() += (octant & 4) ? offset : -offset;
    node->children[octant] = std::make_unique<OctreeNode>(nc, newSize);
}

// ---------------------------------------------------------------------------
// Octree insertion
//
// Design: every node tracks the aggregate (totalMass, centerOfMass) of ALL
// bodies in its subtree.  When a leaf is split we must push the existing body
// DIRECTLY into the appropriate child, not back into the same node, otherwise
// the node looks like an empty leaf again and the existing body never descends.
// ---------------------------------------------------------------------------
void GravityEngine::insertToNode(OctreeNode* node, CelestialBody* newBody, int depth) {
    // ── depth cap: accumulate into this node without descending further ──
    if (depth > 64) {
        float newTM = node->totalMass + newBody->mass;
        if (newTM > 0.0f)
            node->centerOfMass = (node->centerOfMass * node->totalMass +
                                  newBody->position  * newBody->mass) / newTM;
        node->totalMass = newTM;
        return;
    }

    // ── empty leaf: just store the body here ──
    if (node->isLeaf() && node->body == nullptr) {
        node->body        = newBody;
        node->totalMass   = newBody->mass;
        node->centerOfMass = newBody->position;
        return;
    }

    if (node->body != nullptr) {
        CelestialBody* existing = node->body;
        node->body = nullptr;
        int existingOctant = getOctant(node, existing);
        createChild(node, existingOctant);
        insertToNode(node->children[existingOctant].get(), existing, depth + 1);
    }

    float newTM = node->totalMass + newBody->mass;
    node->centerOfMass = (node->centerOfMass * node->totalMass +
                          newBody->position   * newBody->mass) / newTM;
    node->totalMass = newTM;

    int octant = getOctant(node, newBody);
    createChild(node, octant);
    insertToNode(node->children[octant].get(), newBody, depth + 1);
}

// ---------------------------------------------------------------------------
// Barnes-Hut force evaluation
// ---------------------------------------------------------------------------
Eigen::Vector3f GravityEngine::calculateBarnesHutForce(OctreeNode* node, CelestialBody* target,
                                                        float theta, double G, double softeningSq) {
    if (!node) return Eigen::Vector3f::Zero();

    // Leaf containing only the target: skip (self-interaction).
    if (node->isLeaf() && node->body == target)
        return Eigen::Vector3f::Zero();

    Eigen::Vector3f r_vec = node->centerOfMass - target->position;
    double dist_sq = r_vec.squaredNorm();
    double dist_softened = std::sqrt(dist_sq + softeningSq);

    // MAC: treat node as a point mass if it's a leaf or far enough away.
    if (node->isLeaf() || (node->size / dist_softened < theta)) {
        double r_sq = dist_sq + softeningSq;
        double r    = std::sqrt(r_sq);
        double forceMagnitude = (G * target->mass * node->totalMass) / (r_sq * r);
        return static_cast<float>(forceMagnitude) * r_vec;
    }

    Eigen::Vector3f totalForce = Eigen::Vector3f::Zero();
    for (int i = 0; i < 8; ++i) {
        if (node->children[i])
            totalForce += calculateBarnesHutForce(node->children[i].get(), target,
                                                  theta, G, softeningSq);
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
    if (maxSize == 0.0f) maxSize = 1.0f;  // all bodies at same point

    auto root = std::make_unique<OctreeNode>(center, maxSize);
    for (auto& body : bodies)
        insertToNode(root.get(), &body, 0);

    double softeningSq = softening * softening;
    int n = static_cast<int>(bodies.size());

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; ++i) {
        Eigen::Vector3f f = calculateBarnesHutForce(root.get(), &bodies[i],
                                                     m_theta, G, softeningSq);
        bodies[i].addForce(f);
    }
}
