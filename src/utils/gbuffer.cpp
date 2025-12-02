#include "gbuffer.h"
#include <iostream>

GBuffer::GBuffer(int w, int h)
    : fbo(0), posTex(0), normalTex(0), albedoTex(0), emissiveTex(0), depthRbo(0)
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    createAttachment(posTex, GL_RGBA16F, GL_RGBA, w, h);
    createAttachment(normalTex, GL_RGBA16F, GL_RGBA, w, h);
    createAttachment(albedoTex, GL_RGBA, GL_RGBA, w, h);
    createAttachment(emissiveTex, GL_RGBA16F, GL_RGBA, w, h);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedoTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, emissiveTex, 0);

    GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                             GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);

    glGenRenderbuffers(1, &depthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthRbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[GBuffer] FBO incomplete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GBuffer::~GBuffer() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &posTex);
    glDeleteTextures(1, &normalTex);
    glDeleteTextures(1, &albedoTex);
    glDeleteTextures(1, &emissiveTex);
    glDeleteRenderbuffers(1, &depthRbo);
}

void GBuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
void GBuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void GBuffer::resize(int w, int h) {
    *this = GBuffer(w, h);
}

void GBuffer::createAttachment(GLuint &tex, GLint internalFormat, GLenum format, int w, int h) {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
