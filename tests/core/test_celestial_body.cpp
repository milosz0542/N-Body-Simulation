#include <gtest/gtest.h>
#include <Eigen/Dense>

// Linking a header file with physics logic
#include "core/CelestialBody.h" 

// --- TEST 1: Check whether an object is initialized properly ---
TEST(CelestialBodyTest, Initialization) {
    // Preparing test data
    Eigen::Vector3f expectedPosition(10.5f, -5.0f, 0.0f);
    Eigen::Vector3f expectedVelocity(0.1f, 0.0f, -0.5f);
    float expectedMass = 1000.0f;

    CelestialBody body(expectedPosition, expectedVelocity, expectedMass);

    // Verification with GTest macros
    EXPECT_EQ(body.position, expectedPosition);
    EXPECT_EQ(body.velocity, expectedVelocity);

    EXPECT_FLOAT_EQ(body.mass, expectedMass);

    // Starting force should be equal to [0, 0, 0]
    EXPECT_EQ(body.force, Eigen::Vector3f::Zero());
}

// --- TEST 2: Check a mechanism of accumulation and force resetting ---
TEST(CelestialBodyTest, ForceAccumulationAndReset) {
    // Creating a "static" body in the center
    CelestialBody body(Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero(), 50.0f);

    // Simulating some force of gravity
    Eigen::Vector3f gravityPull(15.0f, -2.5f, 9.81f);
    body.addForce(gravityPull);

    EXPECT_NE(body.force, Eigen::Vector3f::Zero());
    EXPECT_EQ(body.force, gravityPull);

    // Running a tested method
    body.resetForce();

    EXPECT_EQ(body.force, Eigen::Vector3f::Zero());
}