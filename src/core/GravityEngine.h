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

enum class ForceAlgorithm {
    Naive,
    BarnesHut
};

struct OctreeNode {
    Eigen::Vector3f center;
    float size;

    float totalMass = 0.0f;
    Eigen::Vector3f centerOfMass = Eigen::Vector3f::Zero();

    CelestialBody* body = nullptr;

    std::unique_ptr<OctreeNode> children[8];

    OctreeNode(Eigen::Vector3f c, float s) : center(c), size(s) {}

    bool isLeaf() const {
        for (const auto& child : children) {
            if (child) return false;
        }
        return true;
    }
};

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
    double softening = 0.1;

    float m_minTrailDistanceSq = 0.001f;

    ForceAlgorithm m_forceAlgorithm = ForceAlgorithm::Naive;
    float m_theta = 0.5f;

    void calculateForcesNaive();
    void calculateForcesBarnesHut();

    /**
     * @brief Inserts a body into the octree node recursively.
     * @param node   Target octree node.
     * @param body   Body to insert.
     * @param depth  Current recursion depth (used to cap recursion for co-located bodies).
     */
    void insertToNode(OctreeNode* node, CelestialBody* body, int depth = 0);

    /**
     * @brief Recursively computes the Barnes-Hut force on a target body from a node.
     * @param node        Current octree node.
     * @param target      The body receiving the force.
     * @param theta       Barnes-Hut opening angle threshold.
     * @param G           Gravitational constant.
     * @param softeningSq Squared softening length.
     * @return Force vector acting on target.
     */
    Eigen::Vector3f calculateBarnesHutForce(OctreeNode* node, CelestialBody* target,
                                             float theta, double G, double softeningSq);

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
     * Computes gravitational forces between all pairs of bodies (or via Barnes-Hut tree),
     * then updates their positions and velocities using Velocity Verlet integration.
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

    void setForceAlgorithm(ForceAlgorithm algo) { m_forceAlgorithm = algo; }
    ForceAlgorithm getForceAlgorithm() const { return m_forceAlgorithm; }

    void setTheta(float theta) { m_theta = theta; }
    float getTheta() const { return m_theta; }
};
