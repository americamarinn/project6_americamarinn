#include "deferredrenderer.h"
#include <GL/glew.h>

void DeferredRenderer::init() {
    shaderDeferred = new ShaderProgram();
    shaderDeferred->attachShader(":/resources/shaders/fullscreen_quad.vert", GL_VERTEX_SHADER);
    shaderDeferred->attachShader(":/resources/shaders/deferredLighting.frag", GL_FRAGMENT_SHADER);
    shaderDeferred->link();

    initQuad();
}

void DeferredRenderer::initQuad() {
    float verts[] = {
        -1.f, -1.f, 0.f, 0.f, 0.f,
        1.f, -1.f, 0.f, 1.f, 0.f,
        -1.f,  1.f, 0.f, 0.f, 1.f,
        1.f,  1.f, 0.f, 1.f, 1.f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void DeferredRenderer::drawQuad() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void DeferredRenderer::render(GBuffer* gbuf, const std::vector<SceneLightData>& lights, const glm::vec3& camPos) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    shaderDeferred->use();
    shaderDeferred->setUniform1i("gPosition", 0);
    shaderDeferred->setUniform1i("gNormal", 1);
    shaderDeferred->setUniform1i("gAlbedo", 2);
    shaderDeferred->setUniform1i("gEmissive", 3);

    shaderDeferred->setUniform3f("camPos", camPos);

    int count = lights.size() > 8 ? 8 : lights.size();
    shaderDeferred->setUniform1i("numLights", count);

    for (int i = 0; i < count; i++) {
        const SceneLightData& L = lights[i];
        glm::vec3 col = glm::vec3(L.color);

        shaderDeferred->setUniform3f("lights["+std::to_string(i)+"].pos", glm::vec3(L.pos));
        shaderDeferred->setUniform3f("lights["+std::to_string(i)+"].color", col);
        shaderDeferred->setUniform3f("lights["+std::to_string(i)+"].dir", glm::vec3(L.dir));
        shaderDeferred->setUniform1f("lights["+std::to_string(i)+"].angle", L.angle);
        shaderDeferred->setUniform1f("lights["+std::to_string(i)+"].penumbra", L.penumbra);
        shaderDeferred->setUniform3f("lights["+std::to_string(i)+"].atten", glm::vec3(L.function));
        shaderDeferred->setUniform1i("lights["+std::to_string(i)+"].type", (int)L.type);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuf->texPosition);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuf->texNormal);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gbuf->texAlbedo);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gbuf->texEmissive);

    drawQuad();
}

void DeferredRenderer::destroy() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    delete shaderDeferred;
}

