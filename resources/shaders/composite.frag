#version 330 core

in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D sceneTex;
uniform sampler2D bloomTex;

void main()
{
    vec3 sceneColor = texture(sceneTex, texCoord).rgb;
    vec3 bloomColor = texture(bloomTex, texCoord).rgb;

    // Simply add bloom — can be tuned or multiplied
    vec3 finalColor = sceneColor + bloomColor;

    fragColor = vec4(finalColor, 1.0);
}
