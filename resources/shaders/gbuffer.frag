#version 330 core
layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec4 gEmissive;

in vec3 worldPos;
in vec3 worldNormal;
in vec2 texCoord;

uniform vec3 baseColor;
uniform float emissiveStrength;

void main() {
    gPosition = vec4(worldPos, 1.0);
    gNormal = vec4(normalize(worldNormal), 1.0);
    gAlbedo = vec4(baseColor, 1.0);
    gEmissive = vec4(baseColor * emissiveStrength, 1.0);
}
