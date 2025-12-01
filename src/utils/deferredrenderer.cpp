#include "utils/DeferredRenderer.h"
#include <GL/glew.h>

DeferredRenderer::DeferredRenderer(GBuffer *gb)
    : gbuf(gb)
{
    shader = new ShaderProgram(":/shaders/fullscreen_quad.vert",
                               ":/shaders/deferredLighting.frag");
}

void DeferredRenderer::render() {
    shader->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuf->posTex);
    shader->setUniform("gPosition", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuf->normalTex);
    shader->setUniform("gNormal", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gbuf->albedoTex);
    shader->setUniform("gAlbedo", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gbuf->emissiveTex);
    shader->setUniform("gEmissive", 3);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    shader->unbind();
}
