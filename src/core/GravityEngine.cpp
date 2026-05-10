//
// Created by milosz on 3/12/26.
//

#include "GravityEngine.h"
#include <cmath>
#include <omp.h>

void GravityEngine::addBody(const CelestialBody& body) {
    bodies.push_back(body);
}

void GravityEngine::setBodies(const std::vector<CelestialBody> &newBodies) {
    bodies = newBodies;
}

void GravityEngine::saveInitialState() {
    initialBodies = bodies;
}

void GravityEngine::resetInitialState() {
    bodies = initialBodies;

    for (auto& body : bodies) {
        body.clearTrailHistory();
    }
}

void GravityEngine::update(double deltaTime) {
    int n = static_cast<int>(bodies.size());
    if (n == 0) return;

    // Position update and force reset
    for (auto& body : bodies) {
        body.updatePosition(deltaTime);
        body.resetForce();
    }

    if (m_forceAlgorithm == ForceAlgorithm::Naive) {
        calculateForcesNaive();
    } else if (m_forceAlgorithm == ForceAlgorithm::BarnesHut) {
        calculateForcesBarnesHut();
    }

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

    // Multithread totalKinetic
    #pragma omp parallel for reduction(+:totalKinetic)
    for (int i = 0; i < n; ++i) {
        double speedSquared = bodies[i].velocity.squaredNorm();
        totalKinetic += 0.5f * bodies[i].mass * speedSquared;
    }

    // Multithread totalPotential
    double softeningSquared = softening * softening;
    #pragma omp parallel for schedule(dynamic) reduction(+:totalPotential)
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            double r_sq = (bodies[i].position - bodies[j].position).squaredNorm() + softeningSquared;
            double distance = std::sqrt(r_sq);
            totalPotential += -(G * bodies[i].mass * bodies[j].mass) / distance;
        }
    }

    return totalKinetic + totalPotential;
}

void GravityEngine::insertToNode(OctreeNode* node, CelestialBody* newBody, int depth) {
    // BUG FIX #3: Limit recursion depth to prevent infinite recursion
    // when two bodies occupy the same (or nearly identical) position.
    if (depth > 64) {
        // Bodies are essentially co-located: just accumulate mass into this node
        // without descending further.
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

        // Reset mass to redistribute it into children
        node->totalMass = 0;
        node->centerOfMass = Eigen::Vector3f::Zero();

        insertToNode(node, existingBody, depth + 1);
    }

    float newTotalMass = node->totalMass + newBody->mass;
    node->centerOfMass = (node->centerOfMass * node->totalMass + newBody->position * newBody->mass) / newTotalMass;
    node->totalMass = newTotalMass;

    // Find sub-octant for newBody
    int octant = 0;
    if (newBody->position.x() >= node->center.x()) octant |= 1;
    if (newBody->position.y() >= node->center.y()) octant |= 2;
    if (newBody->position.z() >= node->center.z()) octant |= 4;

    if (!node->children[octant]) {
        // BUG FIX #1: Child size is node->size/2, so child center must be offset
        // by size/4 (= newSize/2) from the parent center — NOT by newSize/4.
        float newSize = node->size / 2.0f;
        float offset  = newSize / 2.0f;   // = node->size / 4.0f
        Eigen::Vector3f newCenter = node->center;
        newCenter.x() += (octant & 1) ? offset : -offset;
        newCenter.y() += (octant & 2) ? offset : -offset;
        newCenter.z() += (octant & 4) ? offset : -offset;
        node->children[octant] = std::make_unique<OctreeNode>(newCenter, newSize);
    }

    insertToNode(node->children[octant].get(), newBody, depth + 1);
}

Eigen::Vector3f GravityEngine::calculateBarnesHutForce(OctreeNode* node, CelestialBody* target,
                                                        float theta, double G, double softeningSq) {
    if (!node || (node->body == target)) return Eigen::Vector3f::Zero();

    Eigen::Vector3f r_vec = node->centerOfMass - target->position;
    float dist_sq = r_vec.squaredNorm();

    // BUG FIX #2: Use softened distance in the Barnes-Hut criterion (s/d < theta)
    // to avoid division by zero when dist == 0 (bodies at identical positions).
    float dist_softened = std::sqrt(dist_sq + static_cast<float>(softeningSq));

    if (node->isLeaf() || (node->size / dist_softened < theta)) {
        double r_sq  = dist_sq + softeningSq;
        double r     = std::sqrt(r_sq);
        double forceMagnitude = (G * target->mass * node->totalMass) / (r_sq * r);
        return forceMagnitude * r_vec;
    } else {
        Eigen::Vector3f totalForce = Eigen::Vector3f::Zero();
        for (int i = 0; i < 8; ++i) {
            if (node->children[i]) {
                totalForce += calculateBarnesHutForce(node->children[i].get(), target, theta, G, softeningSq);
            }
        }
        return totalForce;
    }
}

void GravityEngine::calculateForcesNaive() {
    int n = static_cast<int>(bodies.size());
    double softeningSquared = softening * softening;
    int numThreads = omp_get_max_threads();
    m_maxThreads = numThreads;

    if (m_localForces.size() != static_cast<size_t>(numThreads * n)) {
        m_localForces.resize(numThreads * n);
    }

    std::fill(m_localForces.begin(), m_localForces.end(), Eigen::Vector3f::Zero());

    #pragma omp parallel
    {
        int threadId = omp_get_thread_num();
        int offset = threadId * n;

        #pragma omp for schedule(dynamic)
        for (int i = 0; i < n; ++i) {
            for (int j = i+1; j < n; ++j) {
                Eigen::Vector3f r_vec = bodies[j].position - bodies[i].position;

                double r_sq = r_vec.squaredNorm() + softeningSquared;
                double r = std::sqrt(r_sq);
                double forceMagnitude = (G * bodies[i].mass * bodies[j].mass) / (r_sq*r);

                Eigen::Vector3f forceVec = forceMagnitude * r_vec;

                m_localForces[offset + i] += forceVec;
                m_localForces[offset + j] -= forceVec;
            }
        }
    } // End of pragma omp parallel

    // Reduction phase
    for (int i = 0; i < n; ++i) {
        Eigen::Vector3f totalForce = Eigen::Vector3f::Zero();
        for (int t = 0; t < numThreads; ++t) {
            totalForce += m_localForces[t * n + i];
        }
        bodies[i].addForce(totalForce);
    }
}

void GravityEngine::calculateForcesBarnesHut() {
    if (bodies.empty()) return;

    float minX = bodies[0].position.x(), maxX = minX;
    float minY = bodies[0].position.y(), maxY = minY;
    float minZ = bodies[0].position.z(), maxZ = minZ;

    for (size_t i = 1; i < bodies.size(); ++i) {
        const auto& pos = bodies[i].position;
        if (pos.x() < minX) minX = pos.x();
        if (pos.x() > maxX) maxX = pos.x();
        if (pos.y() < minY) minY = pos.y();
        if (pos.y() > maxY) maxY = pos.y();
        if (pos.z() < minZ) minZ = pos.z();
        if (pos.z() > maxZ) maxZ = pos.z();
    }

    Eigen::Vector3f center((minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f);
    float maxSize = std::max({maxX - minX, maxY - minY, maxZ - minZ});
    maxSize *= 1.01f; // error margin

    auto root = std::make_unique<OctreeNode>(center, maxSize);

    for (auto& body : bodies) {
        insertToNode(root.get(), &body, 0);
    }

    double softeningSq = softening * softening;
    int n = static_cast<int>(bodies.size());

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; ++i) {
        Eigen::Vector3f f = calculateBarnesHutForce(root.get(), &bodies[i], m_theta, G, softeningSq);
        bodies[i].addForce(f);
    }
}

// void GravityEngine::setSoftening(float softening) {
//     this->softening = softening;
// }
//
// float GravityEngine::getSoftening() const {
//     return softening;
// }
