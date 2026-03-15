#include <gtest/gtest.h>
#include <Eigen/Dense>

#include "core/GravityEngine.h"
#include "core/CelestialBody.h"

TEST(GravityEngineTest, CanAddBodiesToEngine) {
    GravityEngine engine;

    EXPECT_EQ(engine.getBodies().size(), 0);

    CelestialBody earth(Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero(), 5.97e24f);
    CelestialBody moon(Eigen::Vector3f(384000000.0f, 0, 0), Eigen::Vector3f::Zero(), 7.34e22f);

    engine.addBody(earth);
    engine.addBody(moon);

    ASSERT_EQ(engine.getBodies().size(), 2);

    EXPECT_FLOAT_EQ(engine.getBodies()[0].mass, 5.97e24f);
    EXPECT_FLOAT_EQ(engine.getBodies()[1].mass, 7.34e22f);
}

TEST(GravityEngineTest, SingleBodyRemainsStatic) {
    GravityEngine engine;

    Eigen::Vector3f initialPos(100.0f, -50.0f, 25.0f);
    CelestialBody lonelyPlanet(initialPos, Eigen::Vector3f::Zero(), 1000.0f);

    engine.addBody(lonelyPlanet);

    // Run the simulation for 1 second
    engine.update(1.0f);

    const auto& bodies = engine.getBodies();

    EXPECT_EQ(bodies[0].position, initialPos);

    EXPECT_EQ(bodies[0].velocity, Eigen::Vector3f::Zero());
}

TEST(PhysicsEngineTest, TwoBodiesAttractEachOther) {
    GravityEngine engine;

    CelestialBody bodyA(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f::Zero(), 1.0e12f);

    CelestialBody bodyB(Eigen::Vector3f(100.0f, 0.0f, 0.0f), Eigen::Vector3f::Zero(), 1.0e12f);

    engine.addBody(bodyA);
    engine.addBody(bodyB);

    // Run the simulation for 1 second
    engine.update(1.0f);

    const auto& bodies = engine.getBodies();

    EXPECT_GT(bodies[0].velocity.x(), 0.0f);

    EXPECT_LT(bodies[1].velocity.x(), 0.0f);

    EXPECT_FLOAT_EQ(bodies[0].velocity.y(), 0.0f);
    EXPECT_FLOAT_EQ(bodies[0].velocity.z(), 0.0f);
    EXPECT_FLOAT_EQ(bodies[1].velocity.y(), 0.0f);
    EXPECT_FLOAT_EQ(bodies[1].velocity.z(), 0.0f);
}