#pragma once

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include "shaderprogram.h"

class PostProcessPass {
public:
    PostProcessPass();
    ~PostProcessPass();

    void init(int w, int h);

    GLuint extractBright(GLuint sceneTex, int w, int h);
    GLuint blur(GLuint inputTex, int w, int h);
    void composite(GLuint sceneTex, GLuint bloomTex, int w, int h);

private:
    ShaderProgram* extractShader;
    ShaderProgram* blurShader;
    ShaderProgram* compositeShader;

    GLuint quadVAO, quadVBO;

    void initQuad();
};
