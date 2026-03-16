//
// Created by milosz on 3/12/26.
//

#include "GravityEngine.h"
#include <cmath>
#include <omp.h>

void GravityEngine::addBody(const CelestialBody& body) {
    bodies.push_back(body);
}

// void GravityEngine::update(float deltaTime) {
//     for (auto& body : bodies) {
//         body.updatePosition(deltaTime);
//     }
//
//     for (auto& body : bodies) {
//         body.resetForce();
//     }
//
//     // Softening Parameter (to prevent singularities)
//     // Epsilon^2=0.01 [m]
//     const float softeningSquared = 0.01f; // In the future it would depend on size of simulation
//
//     // Force calculations
//     size_t n = bodies.size();
//
//     // OMP parallel - divide tasks for every CPU thread
//     #pragma omp parallel for
//     for (size_t i = 0; i < n; i++) {
//         for (size_t j = 0; j < n; j++) {
//             if (i==j) continue;
//
//             // Displacement vector
//             Eigen::Vector3f r_vec = bodies[j].position - bodies[i].position;
//
//             // Square of the distance with softening
//             float r_sq = r_vec.squaredNorm() + softeningSquared;
//
//             float r = std::sqrt(r_sq);
//
//             float forceMagnitude = (G * bodies[i].mass * bodies[j].mass) / (r_sq*r);
//
//             Eigen::Vector3f forceVec = forceMagnitude * r_vec;
//
//             bodies[i].addForce(forceVec);
//         }
//     }
//
//     for (auto& body : bodies) {
//         body.updateVelocity(deltaTime);
//     }
// }

void GravityEngine::update(float deltaTime) {
    int n = static_cast<int>(bodies.size());

    // Position update and force reset
    for (auto& body : bodies) {
        body.updatePosition(deltaTime);
        body.resetForce();
    }

    const float softeningSquared = 0.01f;

    int numThreads = omp_get_max_threads();

    // 2D array [threads num] X [bodies]
    std::vector<std::vector<Eigen::Vector3f>> localForces(
        numThreads, std::vector<Eigen::Vector3f>(n, Eigen::Vector3f::Zero())
        );

    #pragma omp parallel
    {
        // Thread check
        int threadId = omp_get_thread_num();

        #pragma omp for schedule(dynamic)
        for (int i = 0; i < n; ++i) {
            for (int j = i+1; j < n; ++j) {
                Eigen::Vector3f r_vec = bodies[j].position - bodies[i].position;

                float r_sq = r_vec.squaredNorm() + softeningSquared;
                float r = std::sqrt(r_sq);
                float forceMagnitude = (G * bodies[i].mass * bodies[j].mass) / (r_sq*r);

                Eigen::Vector3f forceVec = forceMagnitude * r_vec;

                localForces[threadId][i] += forceVec;
                localForces[threadId][j] -= forceVec;
            }
        }
    } // End of pragma omp parallel

    // Reduction phase
    for (int i = 0; i < n; ++i) {
        Eigen::Vector3f totalForce = Eigen::Vector3f::Zero();
        for (int t = 0; t < numThreads; ++t) {
            totalForce += localForces[t][i];
        }
        bodies[i].addForce(totalForce);
    }

    for (auto& body : bodies) {
        body.updateVelocity(deltaTime);
    }
}

const std::vector<CelestialBody>& GravityEngine::getBodies() const { return bodies; }