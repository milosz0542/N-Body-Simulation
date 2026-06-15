# N-body simulation
![License](https://img.shields.io/badge/license-MIT-blue)
![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)<br>
This project is a simulation of an N-body problem. <br>
With given parameters (mass, radius, position and velocity) it calculates the position and velocity of the objects. <br>
Simulation is visualized with OpenGL. Inside it is possible to move "camera". (zoom in/out, move in x, y and z axis)

## Documentation
- 📖 Online docs (Doxygen HTML): https://milosz0542.github.io/N-Body-Simulation/
- 📄 PDF Doxygen manual: https://milosz0542.github.io/N-Body-Simulation/documentation.pdf
- 📄 Technical Docs: [technical_documentation.pdf](./docs/technical_documentation.pdf)

## Physics
  This project is based on the following physics:
- Gravitational force
- Newton's law of universal gravitation
- Einstein's General theory of relativity

The most important equation is Newton's law of gravity. <br>
$\overrightarrow{F}_{ij} = \frac{Gm_im_j(\overrightarrow{q}_j - \overrightarrow{q}_i)}{||\overrightarrow{q}_j - \overrightarrow{q}_i ||^3}$

## Libraries
- OpenGL
- Qt
- GLM
- GLEW
- Eigen
- OpenMP
- Google Test (GTest)

## Functionality
- Simulation of N-body problem
- Visualization of the objects
- Camera movement

## Installation/compilation
### Requirements
- C++17 compiler
- CMake 3.18 or higher
- OpenGL 4.6 or higher
- Qt 5.15 or higher
- GLM 0.9.9.8 or higher
- GLEW 2.1.0 or higher
- Eigen 3.3.9 or higher
- OpenMP 4.5 or higher
- Google Test (GTest) 1.11.0 or higher
- Optionally: Doxygen 1.9.2 or higher for documentation generation

### Build
```bash
git clone https://github.com/milosz0542/N-Body-Simulation.git
cd N-Body-Simulation
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Usage

### For GUI mode
- Run the executable without arguments.
- Use the mouse and keyboard to:
    - rotate the camera around the system,
    - zoom in/out,
    - pan in x/y/z,
    - start/pause the simulation.

Parameters such as number of bodies and initial conditions can be adjusted via the .csv file. <br>
Parameters such as simulation speed, visualization adjustments, algorithm (and theta for barnes-hut) can be adjusted via the Qt UI.

### For headless mode:
Use 
```aiignore
./N-body-simulation --headless
```
to run the program in headless mode with default parameters (one planet only). <br>
Use
```aiignore
./N-body-simulation --headless --file <file.txt>
```
to run the program in headless mode with custom parameters.

File format to provide data:
```aiignore
position_x1,position_y1,position_z1,velocity_x1,velocity_y1,velocity_z1,mass1
position_x2,position_y2,position_z2,velocity_x2,velocity_y2,velocity_z2,mass2
...
```
(where 1,2,etc. are indices of the objects)

## Progress
- [x] Sprint 1: Environment and skeleton of the project <br>
Sprint 1 is finished (06.03.2026). <br>
In this sprint, the environment was set up and the skeleton of the project was created. <br>
The project is structured in a way that allows for easy development and maintenance. <br>
- [x] Sprint 2: Physics engine and naive gravity <br>
Sprint 2 is finished (20.03.2026). <br>
In this sprint, the physics engine was implemented and the naive gravity was calculated. <br>
The physics engine is responsible for calculating the forces and updating the positions and velocities of the objects. <br>
The naive gravity is calculated using the Newton's law of gravity. <br>
Headless mode is implemented for verification.
- [x] Sprint 3: Rendering and camera movement in OpenGL <br>
Sprint 3 is finished (04.04.2026). <br>
In this sprint, the rendering and camera movement in OpenGL were implemented. <br>
The rendering is responsible for visualizing the objects in the simulation. <br>
The camera movement allows the user to move the camera in the simulation and zoom in/out. <br>
- [x] Sprint 4: User interface in Qt
Sprint 4 is finished (18.04.2026). <br>
In this sprint, the user interface in Qt was implemented. <br>
The user interface allows the user to interact with the simulation and change the parameters of the objects. <br>
- [x] Sprint 5: Acceleration and optimization
Sprint 5 is finished (11.05.2026). <br>
In this sprint, the acceleration and optimization were implemented. <br>
The acceleration is responsible for improving the performance of the simulation. <br>
The optimization is responsible for improving the performance of the simulation by using better algorithms and data structures. <br>
- [x] Sprint 6: Documentation and finalization <br>
Sprint 6 is finished (07.06.2026). <br>
In this sprint, the documentation and finalization were implemented. <br>
The documentation is responsible for providing information about the project and how to use it. <br>
The finalization is responsible for preparing the project for release and making sure that everything is working correctly. <br>

It is meant to finish this project in one semester. (2025/2026 summer semester)