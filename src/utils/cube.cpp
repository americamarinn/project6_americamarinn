#include "cube.h"
#include <glm/glm.hpp>

static inline void pushPosNorm(std::vector<float>& dst,
                               const glm::vec3& p,
                               const glm::vec3& n) {
    dst.push_back(p.x); dst.push_back(p.y); dst.push_back(p.z);
    dst.push_back(n.x); dst.push_back(n.y); dst.push_back(n.z);
}

static glm::vec3 lerp(const glm::vec3 &a, const glm::vec3 &b, float t) {
    return a + t * (b - a);
}

void Cube::updateParams(int param1) {
    m_vertexData.clear();
    m_param1 = std::max(1, param1);
    setVertexData();
}

void Cube::makeTile(glm::vec3 tl, glm::vec3 tr, glm::vec3 bl, glm::vec3 br) {
    glm::vec3 n = glm::normalize(glm::cross(bl - tl, br - tl));
    // Triangle 1
    pushPosNorm(m_vertexData, tl, n);
    pushPosNorm(m_vertexData, bl, n);
    pushPosNorm(m_vertexData, br, n);
    // Triangle 2
    pushPosNorm(m_vertexData, tl, n);
    pushPosNorm(m_vertexData, br, n);
    pushPosNorm(m_vertexData, tr, n);
}

void Cube::makeFace(glm::vec3 tl, glm::vec3 tr, glm::vec3 bl, glm::vec3 br) {
    for (int row = 0; row < m_param1; ++row) {
        float vTop = float(row) / m_param1;
        float vBot = float(row + 1) / m_param1;

        glm::vec3 lTop = lerp(tl, bl, vTop);
        glm::vec3 lBot = lerp(tl, bl, vBot);
        glm::vec3 rTop = lerp(tr, br, vTop);
        glm::vec3 rBot = lerp(tr, br, vBot);

        for (int col = 0; col < m_param1; ++col) {
            float uL = float(col) / m_param1;
            float uR = float(col + 1) / m_param1;

            glm::vec3 pTL = lerp(lTop, rTop, uL);
            glm::vec3 pTR = lerp(lTop, rTop, uR);
            glm::vec3 pBL = lerp(lBot, rBot, uL);
            glm::vec3 pBR = lerp(lBot, rBot, uR);

            makeTile(pTL, pTR, pBL, pBR);
        }
    }
}

void Cube::setVertexData() {
    const float s = 0.5f;
    // +Z
    makeFace({-s, s, s}, { s, s, s}, {-s,-s, s}, { s,-s, s});
    // -Z
    makeFace({ s, s,-s}, {-s, s,-s}, { s,-s,-s}, {-s,-s,-s});
    // -X
    makeFace({-s, s,-s}, {-s, s, s}, {-s,-s,-s}, {-s,-s, s});
    // +X
    makeFace({ s, s, s}, { s, s,-s}, { s,-s, s}, { s,-s,-s});
    // +Y
    makeFace({-s, s,-s}, { s, s,-s}, {-s, s, s}, { s, s, s});
    // -Y
    makeFace({-s,-s, s}, { s,-s, s}, {-s,-s,-s}, { s,-s,-s});
}
