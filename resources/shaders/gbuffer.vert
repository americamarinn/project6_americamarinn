#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 uv_in;

out vec3 worldPos;
out vec3 worldNormal;
out vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    vec4 wp = model * vec4(pos, 1.0);
    worldPos = wp.xyz;
    worldNormal = mat3(model) * nor;
    uv = uv_in;

    gl_Position = proj * view * wp;
}
