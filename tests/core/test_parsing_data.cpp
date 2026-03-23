// #include <gtest/gtest.h>
// #include <sstream>
//
// #include "core/GravityEngine.h"
//
//
// extern bool loadUniverseFromFile(const std::istream& stream, GravityEngine& engine);
//
// TEST(FileParserTest, HandlesCorruptedDataGracefully) {
//     GravityEngine engine;
//
//     std::stringstream corruptedCSV(
//         "Masa,PosX,PosY,PosZ,VelX,VelY,VelZ\n"
//         "1.98e30, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0\n"
//         "CORRUPTED, 150.0, 0.0, 0.0, 0.0, 30.0, 0.0\n"
//         "6.39e23, 227.0, 0.0, 0.0, 0.0, 24.0, 0.0\n"
//     );
//
//     bool result = loadUniverseFromFile(corruptedCSV, engine);
//
//     EXPECT_TRUE(result);
//
//     ASSERT_EQ(engine.getBodies().size(), 2);
//
//     EXPECT_FLOAT_EQ(engine.getBodies()[1].mass, 6.39e23f);
// }