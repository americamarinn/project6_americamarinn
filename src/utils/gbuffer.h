#pragma once
#include <GL/glew.h>

class GBuffer {
public:
    GLuint fbo;

    GLuint posTex;      // world position
    GLuint normalTex;   // normal
    GLuint albedoTex;   // base color
    GLuint emissiveTex; // neon emissive
    GLuint depthRBO;    // depth buffer

    int width, height;

    GBuffer(int w, int h);
    void init();
    void bindForWriting(); // geometry pass
    void bindForReading(); // deferred pass
};
