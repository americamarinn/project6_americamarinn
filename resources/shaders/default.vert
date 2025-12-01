#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 wsPosition;
out vec3 wsNormal;

void main() {
    vec4 worldPosition = model * vec4(position, 1.0);
    wsPosition = worldPosition.xyz;

    wsNormal = mat3(model) * normal;
    gl_Position = proj * view * worldPosition;
}
