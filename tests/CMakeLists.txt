add_executable(CoreTests
    core/test_celestial_body.cpp
        core/test_gravity_engine.cpp
)

target_link_libraries(CoreTests
    PRIVATE
    NBodyCore
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(CoreTests)