#version 330 core

in vec2 vUV;
out vec4 fragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gEmissive;

uniform int numLights;
uniform vec3 camPos;

struct Light {
    int type;
    vec3 pos;
    vec3 dir;
    vec3 atten;
    float angle;
    float penumbra;
    vec3 color;
};

uniform Light lights[8];

void main() {
    vec3 pos = texture(gPosition, vUV).xyz;
    vec3 nor = normalize(texture(gNormal, vUV).xyz);
    vec3 albedo = texture(gAlbedo, vUV).rgb;
    vec3 emissive = texture(gEmissive, vUV).rgb;

    vec3 result = emissive;

    for (int i = 0; i < numLights; i++) {
        vec3 Ldir = lights[i].pos - pos;
        float dist = length(Ldir);
        Ldir /= dist;

        float diff = max(dot(nor, Ldir), 0.0);
        float atten = 1.0 / (lights[i].atten.x + lights[i].atten.y * dist + lights[i].atten.z * dist * dist);
        vec3 contrib = albedo * diff * lights[i].color * atten;

        result += contrib;
    }

    fragColor = vec4(result, 1.0);
}
