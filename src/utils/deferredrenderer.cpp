#include "deferredrenderer.h"
#include <GL/glew.h>

DeferredRenderer::DeferredRenderer()
    : shaderDeferred(nullptr), quadVAO(0), quadVBO(0)
{}

DeferredRenderer::~DeferredRenderer() {
    delete shaderDeferred;
}

void DeferredRenderer::init() {
    shaderDeferred = new ShaderProgram(
        ":/resources/shaders/fullscreen_quad.vert",
        ":/resources/shaders/deferredLighting.frag"
        );

    float quadVertices[] = {
        -1, -1, 0, 0, 0,
        1, -1, 0, 1, 0,
        -1,  1, 0, 0, 1,
        1,  1, 0, 1, 1
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),
                 quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          5*sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          5*sizeof(float), (void*)(3*sizeof(float)));
}

void DeferredRenderer::render(GBuffer* gbuf, int w, int h) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,w,h);

    shaderDeferred->bind();
    shaderDeferred->setUniform1i("posTex", 0);
    shaderDeferred->setUniform1i("normalTex", 1);
    shaderDeferred->setUniform1i("albedoTex", 2);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, gbuf->posTex);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, gbuf->normalTex);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, gbuf->albedoTex);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

