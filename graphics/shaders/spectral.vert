#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 viewProj;
uniform mat4 model;
uniform float uScale;

void main() {
    vec3 scaledPosition = position * uScale;
    gl_Position = viewProj * model * vec4(scaledPosition, 1.0);
}