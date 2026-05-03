#version 440 core

in float vAlpha;
out vec4 FragColor;

void main() {
    FragColor = vec4(0.3, 0.6, 1.0, vAlpha); // Light blue color with alpha from vertex shader
}
