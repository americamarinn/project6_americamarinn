#include "postprocesspass.h"
#include <iostream>

PostProcessPass::PostProcessPass()
    : m_width(0), m_height(0) {}

PostProcessPass::~PostProcessPass() {
    glDeleteFramebuffers(1, &m_extractFBO);
    glDeleteTextures(1, &m_extractTex);

    glDeleteFramebuffers(1, &m_pingFBO);
    glDeleteTextures(1, &m_pingTex);

    glDeleteFramebuffers(1, &m_pongFBO);
    glDeleteTextures(1, &m_pongTex);

    glDeleteFramebuffers(1, &m_finalFBO);
    glDeleteTextures(1, &m_finalTex);
}

void PostProcessPass::init(int w, int h) {
    m_width = w;
    m_height = h;

    // Load shaders
    m_extractShader = std::make_unique<ShaderProgram>(
        "resources/shaders/bloom_extract.frag",
        "resources/shaders/fullscreen_quad.vert");

    m_blurHShader = std::make_unique<ShaderProgram>(
        "resources/shaders/blur_h.frag",
        "resources/shaders/fullscreen_quad.vert");

    m_blurVShader = std::make_unique<ShaderProgram>(
        "resources/shaders/blur_v.frag",
        "resources/shaders/fullscreen_quad.vert");

    m_compositeShader = std::make_unique<ShaderProgram>(
        "resources/shaders/composite.frag",
        "resources/shaders/fullscreen_quad.vert");

    initTextures();
    initFBOs();
}

void PostProcessPass::resize(int w, int h) {
    m_width = w;
    m_height = h;
    initTextures();
    initFBOs();
}

void PostProcessPass::initTextures() {
    auto makeTex = [&](GLuint &tex) {
        if (tex == 0) glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                     m_width, m_height, 0,
                     GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    };

    makeTex(m_extractTex);
    makeTex(m_pingTex);
    makeTex(m_pongTex);
    makeTex(m_finalTex);
}

void PostProcessPass::initFBOs() {
    auto makeFBO = [&](GLuint &fbo, GLuint tex) {
        if (fbo == 0) glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, tex, 0);

        GLenum att[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, att);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "[PostProcessPass] FBO incomplete!\n";
        }
    };

    makeFBO(m_extractFBO, m_extractTex);
    makeFBO(m_pingFBO, m_pingTex);
    makeFBO(m_pongFBO, m_pongTex);
    makeFBO(m_finalFBO, m_finalTex);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessPass::render(GLuint hdrColorTex) {
    // ---- 1. Extract bright pixels ----
    glBindFramebuffer(GL_FRAMEBUFFER, m_extractFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    m_extractShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrColorTex);
    m_extractShader->setUniform1i("sceneTex", 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // ---- 2. Blur horizontally ----
    glBindFramebuffer(GL_FRAMEBUFFER, m_pingFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    m_blurHShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_extractTex);
    m_blurHShader->setUniform1i("image", 0);
    m_blurHShader->setUniform1f("texelSize", 1.0f / m_width);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // ---- 3. Blur vertically ----
    glBindFramebuffer(GL_FRAMEBUFFER, m_pongFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    m_blurVShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pingTex);
    m_blurVShader->setUniform1i("image", 0);
    m_blurVShader->setUniform1f("texelSize", 1.0f / m_height);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // ---- 4. Composite bloom + original scene ----
    glBindFramebuffer(GL_FRAMEBUFFER, m_finalFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    m_compositeShader->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrColorTex);
    m_compositeShader->setUniform1i("sceneTex", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_pongTex);
    m_compositeShader->setUniform1i("bloomTex", 1);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
