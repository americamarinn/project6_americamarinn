#version 330 core
in vec2 texCoord;
out vec4 FragColor;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gEmissive;
void main() {
    vec3 pos = texture(gPosition, texCoord).rgb;
    vec3 norm = normalize(texture(gNormal, texCoord).rgb);
    vec3 albedo = texture(gAlbedo, texCoord).rgb;
    vec3 emiss = texture(gEmissive, texCoord).rgb;

    vec3 lightDir = normalize(vec3(0.5, -1.0, 0.2));
    float diff = max(dot(norm, -lightDir), 0.0);

    vec3 outColor = albedo * diff + emiss;
    FragColor = vec4(outColor, 1.0);
}
