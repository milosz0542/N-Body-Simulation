//
// Created by milosz on 3/12/26.
//
/**
 * @file GravityEngine.h
 * @brief Definition of the GravityEngine class for n-body simulation.
 */

#pragma once
#include "CelestialBody.h"
#include <memory>
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
 *
 * ### Velocity Verlet integration
 * Each call to update(dt) performs one complete leapfrog/Velocity-Verlet step:
 *   1. x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt^2   [updatePosition]
 *   2. F(t+dt) computed from new positions          [calculateForces*]
 *   3. v(t+dt) = v(t) + 0.5*(a(t)+a(t+dt))*dt      [updateVelocity]
 *
 * IMPORTANT: F(t=0) must be initialised before the first update() call.
 * Call initializeForces() (or saveInitialState()/setBodies()) once after
 * adding all bodies and before starting the simulation loop.
 */
class GravityEngine {
private:
    std::vector<CelestialBody> bodies;
    std::vector<CelestialBody> initialBodies;

    std::vector<Eigen::Vector3f> m_localForces;
    int m_maxThreads = 0;

    const double G = 1;

    /// Softening length epsilon (prevents 1/r singularity).
    double softening = 0.1;

    float m_minTrailDistanceSq = 0.001f;

    ForceAlgorithm m_forceAlgorithm = ForceAlgorithm::Naive;
    float m_theta = 0.5f;

    // ------------------------------------------------------------------ force calculation
    void calculateForcesNaive();
    void calculateForcesBarnesHut();

    /**
     * @brief Insert a body into the octree recursively.
     * @param depth Current recursion depth; capped at 64 to handle co-located bodies.
     */
    void insertToNode(OctreeNode* node, CelestialBody* body, int depth = 0);

    /// Recursively compute Barnes-Hut force on @p target from subtree @p node.
    Eigen::Vector3f calculateBarnesHutForce(OctreeNode* node, CelestialBody* target,
                                             float theta, double G, double softeningSq);

public:
    GravityEngine() = default;

    void addBody(const CelestialBody& body);
    void setBodies(const std::vector<CelestialBody>& newBodies);

    /**
     * @brief Snapshot the current state as the "initial" state AND
     *        compute F(t=0) so the Verlet integrator starts correctly.
     *
     * Must be called once after all bodies are added and before the
     * first update() call.
     */
    void saveInitialState();

    /// Restore bodies to the saved initial state and re-initialise forces.
    void resetInitialState();

    /**
     * @brief Compute forces on all bodies WITHOUT advancing time.
     *
     * Call this once after adding bodies (or changing the force algorithm)
     * to prime F(t=0) for the Velocity Verlet integrator.
     */
    void initializeForces();

    /**
     * @brief Advance the simulation by one time step (Velocity Verlet).
     * @param deltaTime Time step dt.
     */
    void update(double deltaTime);

    const std::vector<CelestialBody>& getBodies() const;

    /**
     * @brief Total mechanical energy: kinetic + potential (with softening).
     */
    double calculateTotalEnergy() const;

    size_t m_maxTrailLength = 100;

    void setTrailResolution(float minDistance) {
        m_minTrailDistanceSq = minDistance * minDistance;
    }

    void setForceAlgorithm(ForceAlgorithm algo) {
        m_forceAlgorithm = algo;
        // Re-prime forces so the next update() uses the correct algorithm from the start.
        if (!bodies.empty()) initializeForces();
    }
    ForceAlgorithm getForceAlgorithm() const { return m_forceAlgorithm; }

    void setTheta(float theta) { m_theta = theta; }
    float getTheta() const { return m_theta; }
};
