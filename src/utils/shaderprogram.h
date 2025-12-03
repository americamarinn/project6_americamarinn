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

    ShaderProgram();
    ShaderProgram(const std::string& vs, const std::string& fs);

    void attachShader(const std::string& path, GLenum type);
    void link();
    void use();
    void bind(); // compatibility alias

    void setUniform1i(const std::string& name, int v);
    void setUniform1f(const std::string& name, float v);

    void setUniform3f(const std::string& name, const glm::vec3& v);
    void setUniform3f(const std::string& name, float x, float y, float z);
};
