#pragma once
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class ShaderProgram {
public:
    GLuint id;

    ShaderProgram(const char* vertPath, const char* fragPath);
    ~ShaderProgram();

    void bind();
    void unbind();

    // Uniforms
    void setUniform1i(const char* name, int v);
    void setUniform1f(const char* name, float v);
    void setUniform3f(const char* name, float x, float y, float z);
    void setUniformMat4(const char* name, const glm::mat4 &mat);

private:
    GLuint loadShader(const char* path, GLenum type);
};
