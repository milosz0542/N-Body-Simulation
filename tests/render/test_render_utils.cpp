#include <gtest/gtest.h>
#include <Eigen/Dense>
#include "core/CelestialBody.h"
#include "render/RenderUtils.h"

TEST(RenderUtilsTest, PrepareFlatGPUBufferCorrectly) {
    std::vector<CelestialBody> bodies;

    bodies.emplace_back(Eigen::Vector3f(10.0f, 20.0f, 30.0f), Eigen::Vector3f::Zero(), 100.0f);

    bodies.emplace_back(Eigen::Vector3f(-5.0f, 0.0f, 15.5f), Eigen::Vector3f::Zero(), 200.0f);

    std::vector<float> gpuBuffer = RenderUtils::preparePositionBuffer(bodies);

    ASSERT_EQ(gpuBuffer.size(), 6);

    EXPECT_FLOAT_EQ(gpuBuffer[0], 10.0f);
    EXPECT_FLOAT_EQ(gpuBuffer[1], 20.0f);
    EXPECT_FLOAT_EQ(gpuBuffer[2], 30.0f);

    EXPECT_FLOAT_EQ(gpuBuffer[3], -5.0f);
    EXPECT_FLOAT_EQ(gpuBuffer[4], 0.0f);
    EXPECT_FLOAT_EQ(gpuBuffer[5], 15.5f);
}