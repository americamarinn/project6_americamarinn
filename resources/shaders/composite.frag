#version 330 core
out vec4 FragColor;
in vec2 uv;

uniform sampler2D sceneTex;
uniform sampler2D bloomTex;

void main()
{
    vec3 scene = texture(sceneTex, uv).rgb;
    vec3 bloom = texture(bloomTex, uv).rgb;

    // HDR bloom combine
    vec3 color = scene + bloom;

    FragColor = vec4(color, 1.0);
}
