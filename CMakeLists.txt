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

find_package(OpenMP REQUIRED)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating documentation with Doxygen"
    )
endif()

add_library(NBodyCore
        src/core/CelestialBody.cpp
        src/core/GravityEngine.cpp
        src/core/GravityEngine.h
        src/render/RenderUtils.cpp
        src/render/RenderUtils.h
)

target_include_directories(NBodyCore PUBLIC src)
target_link_libraries(NBodyCore PUBLIC Eigen3::Eigen)

add_executable(NBodyProblem src/main.cpp
        src/gui/NBodyWidget.cpp
        src/gui/NBodyWidget.h
        src/gui/MainWindow.cpp
        src/gui/MainWindow.h
        src/core/CelestialBody.h
        src/render/shaders.qrc)

if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo|MinSizeRel")
    if(MSVC)
        target_compile_options(NBodyProblem PRIVATE /O2 /arch:AVX2 /fp:fast)
    else()
        target_compile_options(NBodyProblem PRIVATE -O3 -march=native -ffast-math)
    endif()
endif()

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
        OpenMP::OpenMP_CXX
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