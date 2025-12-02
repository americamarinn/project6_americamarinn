#version 330 core
in vec2 texCoord;
out vec4 fragColor;
uniform sampler2D inputTex;
uniform float threshold = 1.0;
void main(){
    vec3 col = texture(inputTex, texCoord).rgb;
    float luminance = dot(col, vec3(0.2126,0.7152,0.0722));
    vec3 bright = (luminance > threshold)? col: vec3(0.0);
    fragColor = vec4(bright,1.0);
}
