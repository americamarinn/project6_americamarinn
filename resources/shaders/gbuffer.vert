#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 vPos;
out vec3 vNor;

void main() {
    vec4 wp = model * vec4(aPos, 1.0);
    vPos = wp.xyz;
    vNor = mat3(model) * aNor;
    gl_Position = proj * view * wp;
}
