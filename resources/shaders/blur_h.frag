#version 330 core
in vec2 texCoord;
out vec4 fragColor;
uniform sampler2D inputTex;
void main(){
    float offset = 1.0/1024.0;
    vec3 result =
        texture(inputTex, texCoord+vec2(-3*offset,0)).rgb*0.05+
        texture(inputTex, texCoord+vec2(-2*offset,0)).rgb*0.09+
        texture(inputTex, texCoord+vec2(-1*offset,0)).rgb*0.12+
        texture(inputTex, texCoord).rgb*0.15+
        texture(inputTex, texCoord+vec2(1*offset,0)).rgb*0.12+
        texture(inputTex, texCoord+vec2(2*offset,0)).rgb*0.09+
        texture(inputTex, texCoord+vec2(3*offset,0)).rgb*0.05;
    fragColor=vec4(result,1.0);
}
