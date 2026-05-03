#version 440 core

layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform int maxTrailLength;
uniform int startOffset;

out float vAlpha;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    int relativeIndex = gl_VertexID - startOffset;

    float safeMaxLen = max(float(maxTrailLength), 1.0);

    vAlpha = 1.0 - (float(relativeIndex) / safeMaxLen);
//    vAlpha = 1.0;

//    gl_PointSize = 1.0 + (vAlpha * 5.0);
}
