//
// Created by milosz on 3/6/26.
//
/**
 * @file CelestialBody.h
 * @brief Definition of the CelestialBody class representing a physical object in space.
 */

#pragma once
#include <Eigen/Dense>
#include <deque>

/**
 * @class CelestialBody
 * @brief Represents a celestial body such as a planet, star, moon, or asteroid within a celestial system.
 *
 * Encapsulates physical properties (position, velocity, mass) and behaviors 
 * common to entities existing in space. It also maintains a history of its 
 * positions to render trails.
 */
class CelestialBody {
public:
    /**
     * @brief Position of the celestial body in 3D space.
     *
     * Cartesian coordinates (x, y, z) relative to a defined origin.
     */
    Eigen::Vector3f position;

    /**
     * @brief Velocity of the celestial body.
     *
     * A vector quantity combining both speed and direction.
     */
    Eigen::Vector3f velocity;

    /**
     * @brief Force acting on the celestial body.
     *
     * Typically measured in newtons (N), used for calculating acceleration.
     */
    Eigen::Vector3f force;

    /**
     * @brief Mass of the celestial body.
     */
    float mass;

    /**
     * @brief Radius of the celestial body.
     *
     * Derived from the mass assuming constant density. Used for rendering 
     * and potentially collision detection.
     */
    float radius;

    /**
     * @brief Default constructor for CelestialBody.
     * 
     * Initializes a body at the origin with zero velocity and mass.
     */
    CelestialBody();

    /**
     * @brief Constructs a CelestialBody with specified initial state.
     * @param pos Initial position vector.
     * @param vel Initial velocity vector.
     * @param m Mass of the body.
     */
    CelestialBody(const Eigen::Vector3f& pos, const Eigen::Vector3f& vel, double m);

    /**
     * @brief Applies an external force to the celestial body.
     *
     * This method adds the provided force vector to the current force acting
     * on the celestial body.
     *
     * @param force The force vector to be applied (e.g., in Newtons).
     */
    void addForce(Eigen::Vector3f& force);

    /**
     * @brief Resets the force to zero.
     * 
     * Should be called at the beginning of each simulation step before recomputing forces.
     */
    void resetForce() { force.setZero(); }

    /**
     * @brief Updates the position and intermediate velocity based on current forces.
     *
     * This method calculates acceleration (a = F / m) and updates position
     * using the current velocity and acceleration over a time step.
     *
     * @param deltaTime The time elapsed for the simulation step.
     */
    void updatePosition(double deltaTime);

    /**
     * @brief Updates the velocity of the celestial body based on the forces.
     *
     * This method completes the velocity update for the current step.
     *
     * @param deltaTime The time elapsed for the simulation step.
     */
    void updateVelocity(double deltaTime);

    /**
     * @brief Records the current position into the trail history.
     */
    void updateTrail(size_t maxLength);

    /**
     * @brief Retrieves the position history of the body.
     * @return A constant reference to the deque of historical positions.
     */
    const std::deque<Eigen::Vector3f>& getTrailHistory() const { return trailHistory; }

    void clearTrailHistory() { trailHistory.clear(); }

private:
    /**
     * @brief Calculates the radius based on mass assuming constant density.
     * @param mass The mass of the body.
     * @return The calculated radius.
     */
    static double calculateRadius(double mass);

    /**
     * @brief Deque storing historical positions for trail rendering.
     */
    std::deque<Eigen::Vector3f> trailHistory;
};