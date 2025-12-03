#include "shaderprogram.h"
#include <QFile>
#include <QString>
#include <glm/glm.hpp>

ShaderProgram::ShaderProgram() {
    id = glCreateProgram();
}

ShaderProgram::ShaderProgram(const std::string& vs, const std::string& fs) {
    id = glCreateProgram();
    attachShader(vs, GL_VERTEX_SHADER);
    attachShader(fs, GL_FRAGMENT_SHADER);
    link();
}

void ShaderProgram::attachShader(const std::string& path, GLenum type) {
    QFile f(QString::fromStdString(path));
    f.open(QFile::ReadOnly);
    QByteArray srcData = f.readAll();
    const char* src = srcData.constData();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    glAttachShader(id, shader);
}

void ShaderProgram::link() {
    glLinkProgram(id);
}

void ShaderProgram::use() {
    glUseProgram(id);
}

void ShaderProgram::bind() {
    glUseProgram(id);
}

void ShaderProgram::setUniform1i(const std::string& name, int v) {
    glUniform1i(glGetUniformLocation(id, name.c_str()), v);
}

void ShaderProgram::setUniform1f(const std::string& name, float v) {
    glUniform1f(glGetUniformLocation(id, name.c_str()), v);
}

void ShaderProgram::setUniform3f(const std::string& name, const glm::vec3& v) {
    glUniform3f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z);
}

void ShaderProgram::setUniform3f(const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}
