#pragma once

#include <GL/glew.h>
#include "utils/shaderloader.h"
#include "utils/GBuffer.h"

class DeferredRenderer {
public:
    GBuffer *gbuf;          // pointer to the GBuffer you created
    ShaderProgram *shader;  // the shader used for deferred shading

    DeferredRenderer(GBuffer *gb);
    void render();
};
