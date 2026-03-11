add_executable(CoreTests
    core/test_celestial_body.cpp
)

target_link_libraries(CoreTests
    PRIVATE
    NBodyCore
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(CoreTests)