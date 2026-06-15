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

TEST(GravityEngineTest, AlgorithmSelection) {
    GravityEngine engine;
    
    // Default should be Naive
    EXPECT_EQ(engine.getForceAlgorithm(), ForceAlgorithm::Naive);
    
    engine.setForceAlgorithm(ForceAlgorithm::BarnesHut);
    EXPECT_EQ(engine.getForceAlgorithm(), ForceAlgorithm::BarnesHut);
    
    engine.setForceAlgorithm(ForceAlgorithm::Naive);
    EXPECT_EQ(engine.getForceAlgorithm(), ForceAlgorithm::Naive);
}

TEST(GravityEngineTest, ThetaAdjustment) {
    GravityEngine engine;
    
    engine.setTheta(0.8f);
    EXPECT_FLOAT_EQ(engine.getTheta(), 0.8f);
    
    engine.setTheta(0.3f);
    EXPECT_FLOAT_EQ(engine.getTheta(), 0.3f);
}

TEST(GravityEngineTest, NaiveVsBarnesHut) {
    GravityEngine engineNaive;
    GravityEngine engineBH;
    
    CelestialBody b1(Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(0, 0, 0), 1e12f);
    CelestialBody b2(Eigen::Vector3f(100, 0, 0), Eigen::Vector3f(0, 0, 0), 1e12f);
    CelestialBody b3(Eigen::Vector3f(0, 100, 0), Eigen::Vector3f(0, 0, 0), 1e12f);
    
    engineNaive.addBody(b1);
    engineNaive.addBody(b2);
    engineNaive.addBody(b3);
    engineNaive.setForceAlgorithm(ForceAlgorithm::Naive);
    
    engineBH.addBody(b1);
    engineBH.addBody(b2);
    engineBH.addBody(b3);
    engineBH.setForceAlgorithm(ForceAlgorithm::BarnesHut);
    engineBH.setTheta(0.0f); // theta=0 should be equivalent to Naive
    
    engineNaive.update(0.1f);
    engineBH.update(0.1f);
    
    auto bodiesNaive = engineNaive.getBodies();
    auto bodiesBH = engineBH.getBodies();
    
    for (size_t i = 0; i < bodiesNaive.size(); ++i) {
        EXPECT_NEAR(bodiesNaive[i].position.x(), bodiesBH[i].position.x(), 1e-3);
        EXPECT_NEAR(bodiesNaive[i].position.y(), bodiesBH[i].position.y(), 1e-3);
        EXPECT_NEAR(bodiesNaive[i].position.z(), bodiesBH[i].position.z(), 1e-3);
    }
}