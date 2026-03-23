//
// Created by milosz on 3/12/26.
//
#pragma once
#include "CelestialBody.h"
#include <vector>

/**
 * @brief A simulation engine for modeling gravitational interactions between celestial bodies.
 *
 * GravityEngine is responsible for simulating the effects of gravity on a collection
 * of celestial bodies over time. The engine implements numerical methods to compute
 * the forces acting on the bodies, update their positions and velocities, and advance
 * the simulation in discrete time steps. This class is designed to handle n-body
 * simulations with realistic physics, including support for large-scale astronomical systems.
 */
class GravityEngine {
private:
    /**
     * @brief A collection of celestial bodies managed by the gravity simulation engine.
     *
     * This vector stores instances of CelestialBody that are part of the simulation.
     * Each body represents an individual physical object with properties such as position,
     * velocity, mass, and force. The GravityEngine class uses this container for advancing
     * the simulation, calculating interactions, and updating the states of the celestial bodies.
     */
    std::vector<CelestialBody> bodies;
    // const float G = 6.67430e-11f;
    const float G = 1; // IDGAF. Why G=1 works but G = 6.67430e-11 not?
    float softening = 0.1f;

public:
    /**
    * @brief Adds a celestial body to the gravity simulation.
    * @param body The celestial body to be added, represented as an instance of CelestialBody.
    */
    void addBody(const CelestialBody& body);
    /**
     * @brief Updates the state of the gravity simulation by computing forces, updating positions, and adjusting velocities.
     *
     * This method performs a step in the simulation by calculating gravitational interactions between celestial bodies,
     * applying the forces to update their velocities, and advancing their positions according to the given time step.
     *
     * @param deltaTime The time step for the simulation update, in seconds.
     */

    void update(float deltaTime);

    /**
     * @brief Retrieves the collection of celestial bodies in the gravity simulation.
     * @return A constant reference to a vector containing the celestial bodies managed by the gravity engine.
     */
    const std::vector<CelestialBody>& getBodies() const;

    /**
     * @brief Calculates the total energy of the gravity simulation, including both kinetic
     *        and potential energy of the celestial bodies.
     *
     * This method computes the total kinetic energy using the individual velocities and masses
     * of the celestial bodies. It then calculates the overall potential energy based on the
     * gravitational interactions between pairs of bodies. The computation employs multi-threading
     * to optimize the performance for a large number of bodies.
     *
     * @return The total energy of the system, which is the sum of kinetic and potential energy.
     */
    float calculateTotalEnergy() const;

    // /**
    //  * @brief Sets the softening parameter used in gravitational force and potential energy calculations.
    //  * @param softening The softening length.
    //  */
    // void setSoftening(float softening);
    //
    // /**
    //  * @brief Gets the current softening parameter.
    //  * @return The current softening length.
    //  */
    // float getSoftening() const;
};
