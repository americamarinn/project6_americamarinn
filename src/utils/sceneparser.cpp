// sceneparser.cpp
#include "sceneparser.h"
#include "scenefilereader.h"
#include "scenedata.h"

#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

// --- Small helpers to build CTMs from SceneTransformation ---
namespace {
static glm::mat4 T(const glm::vec3 &t) {
    glm::mat4 M(1.f);
    M[3] = glm::vec4(t, 1.f);
    return M;
}

static glm::mat4 S(const glm::vec3 &s) {
    glm::mat4 M(1.f);
    M[0][0] = s.x;
    M[1][1] = s.y;
    M[2][2] = s.z;
    return M;
}

static glm::mat4 R(const glm::vec3 &axis, float radians) {
    glm::vec3 a = glm::normalize(axis);
    float c = std::cos(radians);
    float s = std::sin(radians);
    float x = a.x, y = a.y, z = a.z;

    glm::mat4 m(1.f);
    m[0][0] = c + (1 - c) * x * x;
    m[0][1] = (1 - c) * x * y - s * z;
    m[0][2] = (1 - c) * x * z + s * y;

    m[1][0] = (1 - c) * y * x + s * z;
    m[1][1] = c + (1 - c) * y * y;
    m[1][2] = (1 - c) * y * z - s * x;

    m[2][0] = (1 - c) * z * x - s * y;
    m[2][1] = (1 - c) * z * y + s * x;
    m[2][2] = c + (1 - c) * z * z;

    return m;
}

static glm::mat4 apply(const SceneTransformation &t) {
    switch (t.type) {
    case TransformationType::TRANSFORMATION_TRANSLATE:
        return T(t.translate);
    case TransformationType::TRANSFORMATION_SCALE:
        return S(t.scale);
    case TransformationType::TRANSFORMATION_ROTATE:
        return R(t.rotate, t.angle);
    case TransformationType::TRANSFORMATION_MATRIX:
        return t.matrix;
    default:
        return glm::mat4(1.f);
    }
}

// DFS that flattens lights/primitives and carries the CTM down the graph.
static void dfs(const SceneNode *node,
                const glm::mat4 &parentCTM,
                std::vector<SceneLightData> &outLights,
                std::vector<RenderShapeData> &outShapes)
{
    // Accumulate this node's local transformation(s)
    glm::mat4 M = parentCTM;
    for (const SceneTransformation *t : node->transformations) {
        M = M * apply(*t); // parent * local (matches Lab 5 expectations)
    }

    // Process lights on this node → convert to SceneLightData
    for (const SceneLight *L : node->lights) {
        SceneLightData ld{};
        ld.id       = L->id;
        ld.type     = L->type;
        ld.color    = L->color;
        ld.function = L->function;   // attenuation
        ld.angle    = L->angle;
        ld.penumbra = L->penumbra;
        ld.width    = L->width;
        ld.height   = L->height;

        // Positions (point/spot) are transformed by full M
        if (ld.type == LightType::LIGHT_POINT || ld.type == LightType::LIGHT_SPOT) {
            ld.pos = M * glm::vec4(0.f, 0.f, 0.f, 1.f);
        }

        // Directions (dir/spot) are transformed by the linear part of M
        if (ld.type == LightType::LIGHT_DIRECTIONAL || ld.type == LightType::LIGHT_SPOT) {
            glm::vec4 d4 = M * glm::vec4(L->dir.x, L->dir.y, L->dir.z, 0.f);
            ld.dir = glm::vec4(glm::normalize(glm::vec3(d4)), 0.f);
        }

        outLights.push_back(ld);
    }

    // Primitives on this node → add with current CTM
    for (const ScenePrimitive *P : node->primitives) {
        RenderShapeData rs{};
        rs.primitive = *P; // copy material/primitive data
        rs.ctm       = M;
        outShapes.push_back(rs);
    }

    // Recurse to children
    for (const SceneNode *child : node->children) {
        dfs(child, M, outLights, outShapes);
    }
}
} // namespace

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader reader(filepath);
    if (!reader.readJSON()) {
        std::cerr << "[SceneParser] Failed to read: " << filepath << "\n";
        return false;
    }

    // 1) Globals & camera (Lab 5 already exposes these)
    renderData.globalData = reader.getGlobalData();
    renderData.cameraData = reader.getCameraData();


    // 2) Validate camera data
    if (glm::length(glm::vec3(renderData.cameraData.look)) < 0.0001f) {
        std::cerr << "[SceneParser] Warning: Camera look vector is near-zero\n";
    }

    // 3) Flatten graph into lights/shapes from the root
    renderData.lights.clear();
    renderData.shapes.clear();

    const SceneNode *root = reader.getRootNode();
    if (!root) {
        std::cerr << "[SceneParser] Null root node\n";
        return false;
    }

    dfs(root, glm::mat4(1.f), renderData.lights, renderData.shapes);

    std::cout << "[SceneParser] Successfully parsed " << renderData.shapes.size()
              << " shapes and " << renderData.lights.size() << " lights\n";

    return true;
}
