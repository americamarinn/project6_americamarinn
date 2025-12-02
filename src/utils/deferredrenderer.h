#pragma once
#include "gbuffer.h"
#include "shaderprogram.h"

class DeferredRenderer {
public:
    DeferredRenderer();
    ~DeferredRenderer();
    void init();
    void render(GBuffer* gbuf, int w, int h);

private:
    ShaderProgram* shaderDeferred;
    GLuint quadVAO;
    GLuint quadVBO;
};
