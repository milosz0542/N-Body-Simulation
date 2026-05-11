# N-body simulation
This project is a simulation of an N-body problem. <br>
With given parameters (mass, radius, position and velocity) it calculates the position and velocity of the objects. <br>
Simulation is visualized with OpenGL. Inside it is possible to move "camera". (zoom in/out, move in x, y and z axis)

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
- Optionally: Eigen/Boost

## Functionality
- Simulation of N-body problem
- Visualization of the objects
- Camera movement

## Installation/compilation
TODO

## Usage
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
mass1,position_x1,position_y1,position_z1,velocity_x1,velocity_y1,velocity_z1
mass2,position_x2,position_y2,position_z2,velocity_x2,velocity_y2,velocity_z2
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
The camera movement allows the user to move the camera in the simulation and zoom in/out. <
- [x] Sprint 4: User interface in Qt
Sprint 4 is finished (18.04.2026). <br>
In this sprint, the user interface in Qt was implemented. <br>
The user interface allows the user to interact with the simulation and change the parameters of the objects. <br>
- [x] Sprint 5: Acceleration and optimization
Sprint 5 is finished (11.05.2026). <br>
In this sprint, the acceleration and optimization were implemented. <br>
The acceleration is responsible for improving the performance of the simulation. <br>
The optimization is responsible for improving the performance of the simulation by using better algorithms and data structures. <br>
- [ ] Sprint 6: Documentation and finalization

It is meant to finish this project in one semester. (2025/2026 summer semester)