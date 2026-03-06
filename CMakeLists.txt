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

add_executable(NBodyProblem src/main.cpp
        src/gui/NBodyWidget.cpp
        src/gui/NBodyWidget.h
        src/gui/MainWindow.cpp
        src/gui/MainWindow.h)
target_link_libraries(NBodyProblem
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::OpenGL
        Qt::OpenGLWidgets
        OpenGL::GL
        GLEW::GLEW
        Eigen3::Eigen
        glm::glm
)

