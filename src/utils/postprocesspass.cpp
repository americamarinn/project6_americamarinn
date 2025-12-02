#include "postprocesspass.h"
#include <iostream>

PostProcessPass::PostProcessPass()
    : extractShader(nullptr), blurShader(nullptr), compositeShader(nullptr),
    quadVAO(0), quadVBO(0)
{}

PostProcessPass::~PostProcessPass() {
    delete extractShader;
    delete blurShader;
    delete compositeShader;
}

void PostProcessPass::initQuad() {
    float quad[] = {
        -1, -1, 0,0,
        1, -1, 1,0,
        -1,  1, 0,1,
        1,  1, 1,1
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float),
                          (void*)(2*sizeof(float)));
}

void PostProcessPass::init(int w, int h) {
    extractShader = new ShaderProgram(
        ":/resources/shaders/fullscreen_quad.vert",
        ":/resources/shaders/bloom_extract.frag"
        );

    blurShader = new ShaderProgram(
        ":/resources/shaders/fullscreen_quad.vert",
        ":/resources/shaders/bloom_blur.frag"
        );

    compositeShader = new ShaderProgram(
        ":/resources/shaders/fullscreen_quad.vert",
        ":/resources/shaders/bloom_composite.frag"
        );

    initQuad();
}

// ------------------ Extract bright pixels -------------------

GLuint PostProcessPass::extractBright(GLuint sceneTex, int w, int h) {
    GLuint outTex;
    glGenTextures(1, &outTex);
    glBindTexture(GL_TEXTURE_2D, outTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                 w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, outTex, 0);

    glViewport(0,0,w,h);
    extractShader->bind();
    extractShader->setUniform1i("sceneTex", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTex);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);

    return outTex;
}

// ------------------ Gaussian Blur -------------------

GLuint PostProcessPass::blur(GLuint inputTex, int w, int h) {
    GLuint outTex;
    glGenTextures(1, &outTex);
    glBindTexture(GL_TEXTURE_2D, outTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                 w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, outTex, 0);

    glViewport(0,0,w,h);
    blurShader->bind();
    blurShader->setUniform1i("brightTex", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTex);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    return outTex;
}

// ------------------ Composite Scene + Bloom -------------------

void PostProcessPass::composite(GLuint sceneTex, GLuint bloomTex, int w, int h) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,w,h);

    compositeShader->bind();
    compositeShader->setUniform1i("sceneTex", 0);
    compositeShader->setUniform1i("bloomTex", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomTex);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
