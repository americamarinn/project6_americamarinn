#pragma once
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>

class GBuffer {
public:
    GBuffer(int w, int h);
    ~GBuffer();

    void bind();
    void unbind();
    void resize(int w, int h);

    GLuint fbo;
    GLuint posTex;
    GLuint normalTex;
    GLuint albedoTex;
    GLuint emissiveTex;
    GLuint depthRbo;

private:
    void createAttachment(GLuint &tex, GLint internalFormat, GLenum format, int w, int h);
};
