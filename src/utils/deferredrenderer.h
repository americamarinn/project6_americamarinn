#pragma once
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "gbuffer.h"
#include "shaderprogram.h"
#include "sceneparser.h"

class DeferredRenderer {
public:
    ShaderProgram* shaderDeferred;
    GLuint quadVAO, quadVBO;

    void init();
    void render(GBuffer* gbuf, const std::vector<SceneLightData>& lights, const glm::vec3& camPos);
    void destroy();
private:
    void initQuad();
    void drawQuad();
};
