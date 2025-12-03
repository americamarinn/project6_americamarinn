#pragma once

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GL/glew.h>
#include <memory>
#include "shaderprogram.h"

class PostProcessPass {
public:
    PostProcessPass();
    ~PostProcessPass();

    void init(int w, int h);
    void resize(int w, int h);

    // Main entry point:
    void render(GLuint hdrColorTex);

    GLuint getOutputTexture() const { return m_finalTex; }

private:
    int m_width, m_height;

    GLuint m_extractFBO = 0;
    GLuint m_extractTex = 0;

    GLuint m_pingFBO = 0;
    GLuint m_pingTex = 0;

    GLuint m_pongFBO = 0;
    GLuint m_pongTex = 0;

    GLuint m_finalFBO = 0;
    GLuint m_finalTex = 0;

    std::unique_ptr<ShaderProgram> m_extractShader;
    std::unique_ptr<ShaderProgram> m_blurHShader;
    std::unique_ptr<ShaderProgram> m_blurVShader;
    std::unique_ptr<ShaderProgram> m_compositeShader;

    void initTextures();
    void initFBOs();
};
