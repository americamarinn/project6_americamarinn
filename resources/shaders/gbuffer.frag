#version 330 core

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oAlbedo;
layout(location = 3) out vec4 oEmissive;

in vec3 vPos;
in vec3 vNor;

uniform vec3 cDiffuse;
uniform vec3 cEmissive;

void main() {
    oPosition = vec4(vPos, 1.0);
    oNormal = vec4(normalize(vNor), 1.0);
    oAlbedo = vec4(cDiffuse, 1.0);
    oEmissive = vec4(cEmissive, 1.0);
}
