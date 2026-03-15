//
// Created by milosz on 3/12/26.
//

#include "GravityEngine.h"
#include <cmath>
#include <omp.h>

void GravityEngine::addBody(const CelestialBody& body) {
    bodies.push_back(body);
}

void GravityEngine::update(float deltaTime) {
    for (auto& body : bodies) {
        body.resetForce();
    }

    // Softening Parameter (to prevent singularities)
    // Epsilon^2=0.01 [m]
    const float softeningSquared = 0.01f; // In the future it would depend on size of simulation

    // Force calculations
    size_t n = bodies.size();

    // OMP parallel - divide tasks for every CPU thread
    #pragma omp parallel for
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i==j) continue;

            // Displacement vector
            Eigen::Vector3f r_vec = bodies[j].position - bodies[i].position;

            // Square of the distance with softening
            float r_sq = r_vec.squaredNorm() + softeningSquared;

            float r = std::sqrt(r_sq);

            float forceMagnitude = (G * bodies[i].mass * bodies[j].mass) / (r_sq*r);

            Eigen::Vector3f forceVec = forceMagnitude * r_vec;

            bodies[i].addForce(forceVec);
        }
    }

    for (auto& body : bodies) {
        body.update(deltaTime);
    }
}

const std::vector<CelestialBody>& GravityEngine::getBodies() const { return bodies; }