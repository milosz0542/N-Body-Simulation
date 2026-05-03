//
// Created by milosz on 3/12/26.
//
/**
 * @file GravityEngine.h
 * @brief Definition of the GravityEngine class for n-body simulation.
 */

#pragma once
#include "CelestialBody.h"
#include <vector>

/**
 * @class GravityEngine
 * @brief A simulation engine for modeling gravitational interactions between celestial bodies.
 *
 * GravityEngine is responsible for simulating the effects of gravity on a collection
 * of celestial bodies over time. The engine implements numerical methods to compute
 * the forces acting on the bodies, update their positions and velocities, and advance
 * the simulation in discrete time steps.
 */
class GravityEngine {
private:
    /**
     * @brief A collection of celestial bodies managed by the gravity simulation engine.
     */
    std::vector<CelestialBody> bodies;

    std::vector<CelestialBody> initialBodies;

    std::vector<Eigen::Vector3f> m_localForces;
    int m_maxThreads;

    /**
     * @brief Gravitational constant G.
     * 
     * In this simulation, it's set to 1.0 for simplicity in normalized units.
     */
    const double G = 1;

    /**
     * @brief Softening parameter to prevent singularities during close encounters.
     */
    double softening = 0.1f;

    float m_minTrailDistanceSq = 0.001f;

public:
    GravityEngine() = default;

    /**
     * @brief Adds a celestial body to the gravity simulation.
     * @param body The celestial body to be added.
     */
    void addBody(const CelestialBody& body);

    void setBodies(const std::vector<CelestialBody>& newBodies);

    void saveInitialState();
    void resetInitialState();
    /**
     * @brief Updates the state of the simulation by one time step.
     *
     * Computes gravitational forces between all pairs of bodies, then updates
     * their positions and velocities.
     *
     * @param deltaTime The time step for the simulation update.
     */
    void update(double deltaTime);

    /**
     * @brief Retrieves the collection of celestial bodies.
     * @return A constant reference to the vector of celestial bodies.
     */
    const std::vector<CelestialBody>& getBodies() const;

    /**
     * @brief Calculates the total energy of the system.
     *
     * Sum of kinetic energy (0.5 * m * v^2) and potential energy (-G * m1 * m2 / r).
     *
     * @return The total energy of the system.
     */
    double calculateTotalEnergy() const;

    size_t m_maxTrailLength = 100;

    void setTrailResolution(float minDistance) {
        m_minTrailDistanceSq = minDistance * minDistance;
    }
};
