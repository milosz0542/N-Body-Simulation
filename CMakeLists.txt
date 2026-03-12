cmake_minimum_required(VERSION 4.1)
project(NBodyProblem)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        OpenGL
        OpenGLWidgets
        REQUIRED)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

find_package(Eigen3 REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(NBodyCore
        src/core/CelestialBody.cpp
)

target_include_directories(NBodyCore PUBLIC src)
target_link_libraries(NBodyCore PUBLIC Eigen3::Eigen)

add_executable(NBodyProblem src/main.cpp
        src/gui/NBodyWidget.cpp
        src/gui/NBodyWidget.h
        src/gui/MainWindow.cpp
        src/gui/MainWindow.h
        src/core/CelestialBody.h)

target_link_libraries(NBodyProblem
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::OpenGL
        Qt::OpenGLWidgets
        OpenGL::GL
        GLEW::GLEW
        NBodyCore
        glm::glm
)

enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)