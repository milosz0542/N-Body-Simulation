add_executable(CoreTests
    core/test_celestial_body.cpp
        core/test_gravity_engine.cpp
        render/test_render_utils.cpp
        core/test_edge_cases.cpp
        core/test_parsing_data.cpp
)

target_link_libraries(CoreTests
    PRIVATE
    NBodyCore
    GTest::gtest_main
    OpenMP::OpenMP_CXX
)

add_executable(GuiTests
    gui/test_main_window.cpp
    ../src/gui/MainWindow.cpp
    ../src/gui/NBodyWidget.cpp
    ../src/render/shaders.qrc
)

target_link_libraries(GuiTests
    PRIVATE
    NBodyCore
    Qt6::Test
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    GLEW::GLEW
    OpenMP::OpenMP_CXX
)

include(GoogleTest)
gtest_discover_tests(CoreTests)