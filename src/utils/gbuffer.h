#pragma once
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>

class GBuffer {
public:
    GLuint fbo;
    GLuint texPosition;
    GLuint texNormal;
    GLuint texAlbedo;
    GLuint texEmissive;
    GLuint depthRBO;

    void init(int w, int h);
    void bind();
    void unbind();
};
