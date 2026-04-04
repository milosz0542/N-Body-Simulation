//
// Created by milosz on 3/12/26.
//

#include "GravityEngine.h"
#include <cmath>
#include <omp.h>

void GravityEngine::addBody(const CelestialBody& body) {
    bodies.push_back(body);
}

void GravityEngine::update(double deltaTime) {
    int n = static_cast<int>(bodies.size());

    // Position update and force reset
    for (auto& body : bodies) {
        body.updatePosition(deltaTime);
        body.resetForce();
    }
    
    double softeningSquared = softening * softening;

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

                double r_sq = r_vec.squaredNorm() + softeningSquared;
                double r = std::sqrt(r_sq);
                double forceMagnitude = (G * bodies[i].mass * bodies[j].mass) / (r_sq*r);

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
        body.updateTrail();
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

// void GravityEngine::setSoftening(float softening) {
//     this->softening = softening;
// }
//
// float GravityEngine::getSoftening() const {
//     return softening;
// }