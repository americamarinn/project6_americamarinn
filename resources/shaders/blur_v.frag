#version 330 core
out vec4 FragColor;
in vec2 uv;

uniform sampler2D image;
uniform float texelSize;

void main() {
    vec3 result = vec3(0.0);
    float kernel[5] = float[](0.204164, 0.304005, 0.093913, 0.020642, 0.001966);

    for (int i = -4; i <= 4; ++i) {
        float w = kernel[abs(i)];
        result += w * texture(image, uv + vec2(0, i * texelSize)).rgb;
    }

    FragColor = vec4(result, 1.0);
}
