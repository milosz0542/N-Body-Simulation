#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <string>
#include <Eigen/Dense>

#include "core/GravityEngine.h"
#include "core/CelestialBody.h"

// Mock version of the function from main.cpp to test the parsing logic
bool loadUniverseFromStream(std::istream& file, GravityEngine& engine) {
    std::string line;
    // Skip header if present, but our logic in main.cpp just skips the FIRST line
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string token;
        std::vector<float> data;

        while (std::getline(iss, token, ',')) {
            try {
                data.push_back(std::stof(token));
            } catch (...) {
                return false;
            }
        }

        if (data.size() == 7) {
            Eigen::Vector3f position(data[0], data[1], data[2]);
            Eigen::Vector3f velocity(data[3], data[4], data[5]);
            float mass = data[6];
            CelestialBody body(position, velocity, mass);
            engine.addBody(body);
        }
    }
    return true;
}

TEST(FileParserTest, HandlesFormatCorrectly) {
    GravityEngine engine;
    std::stringstream csvData(
        "x,y,z,vx,vy,vz,m\n"
        "1.0,2.0,3.0,0.1,0.2,0.3,100.0\n"
        "10.0,20.0,30.0,1.1,1.2,1.3,1000.0\n"
    );

    bool result = loadUniverseFromStream(csvData, engine);
    EXPECT_TRUE(result);
    ASSERT_EQ(engine.getBodies().size(), 2);

    EXPECT_FLOAT_EQ(engine.getBodies()[0].position.x(), 1.0f);
    EXPECT_FLOAT_EQ(engine.getBodies()[0].mass, 100.0f);
    EXPECT_FLOAT_EQ(engine.getBodies()[1].position.z(), 30.0f);
    EXPECT_FLOAT_EQ(engine.getBodies()[1].mass, 1000.0f);
}

TEST(FileParserTest, HandlesCorruptedDataGracefully) {
    GravityEngine engine;
    std::stringstream corruptedCSV(
        "x,y,z,vx,vy,vz,m\n"
        "1.0,2.0,3.0,0.1,0.2,0.3,100.0\n"
        "CORRUPTED, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0\n"
    );

    bool result = loadUniverseFromStream(corruptedCSV, engine);
    EXPECT_FALSE(result);
}