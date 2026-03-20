//
// Created by milosz on 3/6/26.
//
#pragma once
#include <Eigen/Dense>

/**
 * Represents a celestial body such as a planet, star, moon, or asteroid within a celestial system.
 * Encapsulates properties and behaviors common to entities existing in space.
 */
class CelestialBody {
public:
    /**
     * Represents the position of a celestial body in a 3D space.
     *
     * This variable stores the Cartesian coordinates (x, y, z) of the celestial body
     * relative to a defined origin. The position is typically measured in units
     * such as meters or kilometers, depending on the application's unit system.
     */
    Eigen::Vector3f position;
    /**
     * Represents the velocity of an object, typically measured as a vector quantity
     * combining both speed and direction.*/
    Eigen::Vector3f velocity;
    /**
     * Represents the force value, typically measured in newtons (N), used in calculations involving physical dynamics.
     */
    Eigen::Vector3f force;
    /**
     * Represents the mass of an object.*/
    float mass;
    /**
     * Represents the radius of a celestial body.
     *
     * This variable defines the radius of the celestial body, which can be used
     * for calculations such as collision detection, rendering, or volume estimation.
     *
     * The radius is typically derived from the mass of the celestial body
     * using a predefined relationship (e.g., assuming constant density).
     *
     * Units of measurement for the radius should be consistent with the
     * system of units used in the application (e.g., meters or kilometers).
     */
    float radius;

    CelestialBody();

    /**
     * Represents a celestial body such as a planet, star, or moon.
     */
    CelestialBody(const Eigen::Vector3f& pos, const Eigen::Vector3f& vel, float m);

    /**
     * Applies an external force to the celestial body.
     *
     * This method adds the provided force vector to the current force acting
     * on the celestial body. It modifies the `force` member variable by
     * accumulating the input force.
     *
     * @param force The force vector to be applied to the celestial body.
     *              This vector typically represents a physical force, measured
     *              in newtons (N), and is added to the object's existing force.
     */
    void addForce(Eigen::Vector3f& force);

    /**
     * Resets the force to its default state*/
    void resetForce() { force.setZero(); }

    /**
     * Updates the position of the celestial body based on its current velocity,
     * acceleration, and the elapsed time.
     *
     * This method calculates the acceleration using the formula: acceleration = force / mass.
     * The position is updated using the formula:
     * position += velocity * deltaTime + 0.5 * acceleration * deltaTime^2.
     * The velocity is also updated in this method using the formula:
     * velocity += 0.5 * acceleration * deltaTime.
     *
     * @param deltaTime The time elapsed over which the position and velocity update
     *                  are calculated. Must be a positive value for accurate results.
     */
    void updatePosition(float deltaTime);

    /**
     * Updates the velocity of the celestial body based on the forces applied to it
     * and the elapsed time.
     *
     * This method calculates the new acceleration of the celestial body using the
     * formula: acceleration = force / mass. The velocity is then updated using
     * the formula: velocity += 0.5 * acceleration * deltaTime.
     *
     * @param deltaTime The time elapsed over which the velocity update is calculated.
     *                  Must be a positive value for accurate results.
     */
    void updateVelocity(float deltaTime);

private:
    /**
     * Calculates the radius of a celestial body based on its mass.
     *
     * The radius is determined using the assumption that the body has a constant density,
     * resulting in the radius being proportional to the cube root of its mass.
     *
     * @param mass The mass of the celestial body. Must be a non-negative value.
     * @return The calculated radius of the celestial body. Returns 0.0f if the mass is non-positive.
     */
    static float calculateRadius(float mass);
};