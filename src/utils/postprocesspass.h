#pragma once
#include <GL/glew.h>
#include "shaderloader.h"

class PostProcessPass {
public:
    ShaderProgram *shader;
    GLuint fbo, texture;

    PostProcessPass(int w, int h, const char* fragShaderPath);
    void bind();
    void draw(GLuint inputTex);
};
