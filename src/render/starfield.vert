#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aBrightness;

uniform mat4 view;
uniform mat4 projection;

out float vBrightness;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);

    gl_PointSize = 1.5;

    vBrightness = aBrightness;
}
