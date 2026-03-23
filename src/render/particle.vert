#version 330 core

// VBO input attribute
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aRadius;
layout (location = 2) in float aSpeed;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float vSpeed; // Pass speed to the fragment shader

void main() {
    vec4 viewSpacePosition = view * model * vec4(aPos, 1.0); // Set virtual position

    gl_Position = projection * viewSpacePosition; // Set the final position of the particle

    float safeZ = abs(viewSpacePosition.z) + 0.1;

    float calculatedSize = (aRadius * 100.0) / safeZ; // Set the point size based on the radius and distance from the camera
    gl_PointSize = clamp(calculatedSize, 6.0, 60.0); // Ensure a minimum point size for visibility

    vSpeed = aSpeed; // Pass the speed to the fragment shader for color calculation
}