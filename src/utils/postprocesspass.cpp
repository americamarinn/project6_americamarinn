#include "PostProcessPass.h"

PostProcessPass::PostProcessPass(int w, int h, const char* fragShaderPath)
{
    shader = new ShaderProgram(":/shaders/fullscreen_quad.vert", fragShaderPath);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                 w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessPass::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessPass::draw(GLuint inputTex) {
    shader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTex);
    shader->setUniform("inputTex", 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    shader->unbind();
}
