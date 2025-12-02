#include "shaderprogram.h"
#include <QFile>
#include <QTextStream>
#include <stdexcept>
#include <iostream>

ShaderProgram::ShaderProgram(const char* vertPath, const char* fragPath) {
    GLuint vert = loadShader(vertPath, GL_VERTEX_SHADER);
    GLuint frag = loadShader(fragPath, GL_FRAGMENT_SHADER);

    id = glCreateProgram();
    glAttachShader(id, vert);
    glAttachShader(id, frag);
    glLinkProgram(id);

    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        GLint length;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

        std::string log(length, '\0');
        glGetProgramInfoLog(id, length, nullptr, log.data());
        throw std::runtime_error("Shader link error:\n" + log);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id);
}

void ShaderProgram::bind()   { glUseProgram(id); }
void ShaderProgram::unbind() { glUseProgram(0); }

GLuint ShaderProgram::loadShader(const char* path, GLenum type) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error(std::string("Failed to load shader: ") + path);

    QTextStream in(&file);
    QString contents = in.readAll();
    std::string code = contents.toStdString();

    GLuint shader = glCreateShader(type);
    const char* src = code.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::string log(length, '\0');
        glGetShaderInfoLog(shader, length, nullptr, log.data());
        throw std::runtime_error("Shader compile error:\n" + log);
    }

    return shader;
}

void ShaderProgram::setUniform1i(const char* name, int v) {
    glUniform1i(glGetUniformLocation(id, name), v);
}
void ShaderProgram::setUniform1f(const char* name, float v) {
    glUniform1f(glGetUniformLocation(id, name), v);
}
void ShaderProgram::setUniform3f(const char* name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(id, name), x, y, z);
}
void ShaderProgram::setUniformMat4(const char* name, const glm::mat4 &mat) {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]);
}

