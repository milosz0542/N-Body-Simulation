/**
 * @file GravityEngine.h
 * @brief Declaration of the gravity simulation engine and Barnes-Hut octree structures.
 *
 * This file defines:
 * - Force computation strategy selection via @ref ForceAlgorithm
 * - Octree node representation for Barnes-Hut approximation via @ref OctreeNode
 * - Main simulation controller class @ref GravityEngine
 *
 * The engine performs N-body simulation with Velocity Verlet integration and supports
 * both direct O(N^2) force computation and Barnes-Hut accelerated approximation.
 */
#pragma once

#include "CelestialBody.h"
#include <memory>
#include <vector>

/**
 * @enum ForceAlgorithm
 * @brief Selects the algorithm used to compute gravitational forces.
 */
enum class ForceAlgorithm {
    /**
     * @brief Direct pairwise force computation.
     *
     * Computes interactions between all body pairs.
     * Complexity: O(N^2).
     */
    Naive,

    /**
     * @brief Barnes-Hut octree approximation.
     *
     * Approximates distant body clusters as a single mass at center of mass.
     * Typical complexity: O(N log N), depending on distribution.
     */
    BarnesHut
};

/**
 * @struct OctreeNode
 * @brief Node used by the Barnes-Hut 3D octree.
 *
 * A node represents a cubic region in space, optionally stores one body
 * (when it is a leaf), and tracks aggregate subtree mass properties.
 */
struct OctreeNode {
    /**
     * @brief Geometric center of this cubic node region.
     */
    Eigen::Vector3f center;

    /**
     * @brief Edge length (or logical node size scale) of this region.
     */
    float size;

    /**
     * @brief Total mass accumulated in this node subtree.
     */
    float totalMass = 0.0f;

    /**
     * @brief Center of mass of all bodies contained in this node subtree.
     */
    Eigen::Vector3f centerOfMass = Eigen::Vector3f::Zero();

    /**
     * @brief Pointer to a body when this node is a leaf holding one body.
     *
     * @note This pointer is non-owning; body storage is managed externally.
     */
    CelestialBody* body = nullptr;

    /**
     * @brief Child octants.
     *
     * Indexing convention is implementation-defined; each non-null child owns
     * one spatial octant of this node.
     */
    std::unique_ptr<OctreeNode> children[8];

    /**
     * @brief Construct a node with given center and size.
     * @param c Node center.
     * @param s Node size.
     */
    OctreeNode(Eigen::Vector3f c, float s) : center(c), size(s) {}

    /**
     * @brief Check whether this node has no children.
     * @return true if all children are null; false otherwise.
     */
    bool isLeaf() const {
        for (const auto& child : children) {
            if (child) return false;
        }
        return true;
    }
};

/**
 * @class GravityEngine
 * @brief Simulation engine for gravitational interactions between celestial bodies.
 *
 * GravityEngine stores a collection of bodies and updates their states over time
 * under Newtonian-like gravity with softening to avoid singularities.
 *
 * Supported force solvers:
 * - @ref ForceAlgorithm::Naive (exact pairwise)
 * - @ref ForceAlgorithm::BarnesHut (approximate octree)
 *
 * Integration method:
 * Velocity Verlet / Leapfrog-style step per @ref update
 *
 * Update sequence:
 * 1. x(t+dt) = x(t) + v(t)dt + 0.5a(t)dt^2
 * 2. Recompute F(t+dt) from updated positions
 * 3. v(t+dt) = v(t) + 0.5(a(t)+a(t+dt))dt
 *
 * @warning Forces must be initialized at t=0 before the first update step.
 * Use @ref initializeForces, or call @ref saveInitialState / @ref setBodies
 * as appropriate after all bodies are configured.
 */
class GravityEngine {
private:
    /**
     * @brief Active simulation bodies.
     */
    std::vector<CelestialBody> bodies;

    /**
     * @brief Stored snapshot of initial body states for reset.
     */
    std::vector<CelestialBody> initialBodies;

    /**
     * @brief Internal temporary per-body force storage.
     */
    std::vector<Eigen::Vector3f> m_localForces;

    /**
     * @brief Maximum thread count for parallel computations (0 = auto/disabled by implementation policy).
     */
    int m_maxThreads = 0;

    /**
     * @brief Gravitational constant used by simulation units.
     */
    const double G = 1;

    /**
     * @brief Softening epsilon used in force law to avoid 1/r singularity.
     *
     * Typically contributes as r^2 + epsilon^2 in denominators.
     */
    double softening = 0.1;

    /**
     * @brief Minimum squared distance between stored trail points.
     */
    float m_minTrailDistanceSq = 0.001f;

    /**
     * @brief Selected force computation algorithm.
     */
    ForceAlgorithm m_forceAlgorithm = ForceAlgorithm::Naive;

    /**
     * @brief Barnes-Hut opening angle criterion (theta).
     *
     * Smaller values increase accuracy and reduce approximation.
     */
    float m_theta = 0.5f;

    /**
     * @brief Compute forces with direct pairwise (naive) method.
     *
     * @details O(N^2), exact for current body set within softening model.
     */
    void calculateForcesNaive();

    /**
     * @brief Compute forces using Barnes-Hut octree approximation.
     */
    void calculateForcesBarnesHut();

    /**
     * @brief Insert a body into an octree node recursively.
     * @param node Target node (must be non-null).
     * @param body Body to insert (non-owning pointer; must remain valid during tree usage).
     * @param depth Current recursion depth.
     *
     * @note Depth is capped (implementation uses default cap logic) to handle
     * co-located or near-identical body positions robustly.
     */
    void insertToNode(OctreeNode* node, CelestialBody* body, int depth = 0);

    /**
     * @brief Compute Barnes-Hut force contribution for one target body from a subtree.
     * @param node Subtree root.
     * @param target Target body for which force is computed.
     * @param theta Opening criterion parameter.
     * @param G Gravitational constant.
     * @param softeningSq Squared softening value.
     * @return Force vector contributed by this subtree.
     */
    Eigen::Vector3f calculateBarnesHutForce(OctreeNode* node, CelestialBody* target,
                                            float theta, double G, double softeningSq);

public:
    /**
     * @brief Default constructor.
     */
    GravityEngine() = default;

    /**
     * @brief Add one body to the simulation state.
     * @param body Body to append.
     *
     * @note After adding all bodies, call @ref initializeForces (or @ref saveInitialState)
     * before first @ref update for correct Verlet startup.
     */
    void addBody(const CelestialBody& body);

    /**
     * @brief Replace the entire body set.
     * @param newBodies New body collection.
     *
     * @note Implementations commonly re-prime integration state after replacement.
     */
    void setBodies(const std::vector<CelestialBody>& newBodies);

    /**
     * @brief Save current bodies as initial state and initialize forces at t=0.
     *
     * Must be called once after setup (when not otherwise initialized) before the first update.
     */
    void saveInitialState();

    /**
     * @brief Restore bodies to saved initial snapshot and reinitialize forces.
     */
    void resetInitialState();

    /**
     * @brief Compute current forces without advancing simulation time.
     *
     * Useful after body setup or after switching force algorithm to ensure
     * the next Verlet update starts from valid F(t).
     */
    void initializeForces();

    /**
     * @brief Advance simulation by one time step using Velocity Verlet integration.
     * @param deltaTime Time step size (dt).
     */
    void update(double deltaTime);

    /**
     * @brief Access current bodies.
     * @return Constant reference to internal body container.
     */
    const std::vector<CelestialBody>& getBodies() const;

    /**
     * @brief Compute total mechanical energy of current system.
     * @return Sum of kinetic and potential energy (using current softening model).
     */
    double calculateTotalEnergy() const;

    /**
     * @brief Maximum number of stored trail points per body.
     */
    size_t m_maxTrailLength = 100;

    /**
     * @brief Set minimum spatial spacing for adding trail points.
     * @param minDistance Minimum linear distance between successive stored trail samples.
     */
    void setTrailResolution(float minDistance) {
        m_minTrailDistanceSq = minDistance * minDistance;
    }

    /**
     * @brief Select force computation algorithm.
     * @param algo Algorithm to use.
     *
     * Re-initializes forces when bodies are present so the next update starts
     * consistently with the newly selected solver.
     */
    void setForceAlgorithm(ForceAlgorithm algo) {
        m_forceAlgorithm = algo;
        if (!bodies.empty()) initializeForces();
    }

    /**
     * @brief Get currently selected force algorithm.
     * @return Active @ref ForceAlgorithm value.
     */
    ForceAlgorithm getForceAlgorithm() const { return m_forceAlgorithm; }

    /**
     * @brief Set Barnes-Hut opening angle parameter.
     * @param theta Opening criterion value.
     */
    void setTheta(float theta) { m_theta = theta; }

    /**
     * @brief Get Barnes-Hut opening angle parameter.
     * @return Current theta value.
     */
    float getTheta() const { return m_theta; }
};