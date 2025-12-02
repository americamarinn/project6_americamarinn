#version 330 core
in vec2 texCoord;
out vec4 fragColor;
uniform sampler2D inputTex;
void main(){
    float offset = 1.0/1024.0;
    vec3 result =
        texture(inputTex, texCoord+vec2(0,-3*offset)).rgb*0.05+
        texture(inputTex, texCoord+vec2(0,-2*offset)).rgb*0.09+
        texture(inputTex, texCoord+vec2(0,-1*offset)).rgb*0.12+
        texture(inputTex, texCoord).rgb*0.15+
        texture(inputTex, texCoord+vec2(0,1*offset)).rgb*0.12+
        texture(inputTex, texCoord+vec2(0,2*offset)).rgb*0.09+
        texture(inputTex, texCoord+vec2(0,3*offset)).rgb*0.05;
    fragColor=vec4(result,1.0);
}
