#include <gtest/gtest.h>
#include <Eigen/Dense>
#include <cmath>
#include <sstream>

#include "core/GravityEngine.h"
#include "core/CelestialBody.h"

TEST(GravityEngineEdgeCases, noDivisionByZeroOnPerfectOverlap) {
    GravityEngine engine;

    // Making two supermassive black holes
    engine.addBody(CelestialBody(Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero(), 1.0e15f));
    engine.addBody(CelestialBody(Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero(), 1.0e15f));

    // Start simulation (Naive algorithm would divide by zero)
    engine.update(0.016f);

    const auto& bodies = engine.getBodies();

    // If position IS NOT NaN the test should pass
    EXPECT_FALSE(std::isnan(bodies[0].position.x()));
    EXPECT_FALSE(std::isnan(bodies[0].velocity.x()));
}

TEST(GravityEngineEdgeCases, EnergyIsConservedOverTime) {
    GravityEngine engine;

    engine.addBody(CelestialBody(Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero(), 1000.0f));

    engine.addBody(CelestialBody(Eigen::Vector3f(100.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 15.0f, 0.0f), 10.0f));

    // Count the Initial Energy
    double initialEnergy = engine.calculateTotalEnergy();

    for (int i = 0; i < 1000; i++) {
        engine.update(0.00001f);
    }

    double finalEnergy = engine.calculateTotalEnergy();

    double allowedError = std::abs(initialEnergy * 0.01);

    EXPECT_NEAR(finalEnergy, initialEnergy, allowedError);
}