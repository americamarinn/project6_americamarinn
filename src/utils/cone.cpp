#include "cone.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <cmath>

static inline void pushPosNorm(std::vector<float>& dst,
                               const glm::vec3& p, const glm::vec3& n) {
    dst.push_back(p.x); dst.push_back(p.y); dst.push_back(p.z);
    dst.push_back(n.x); dst.push_back(n.y); dst.push_back(n.z);
}
static inline glm::vec3 cyl(float r, float t, float y) {
    return {r*std::cos(t), y, r*std::sin(t)};
}
static inline glm::vec3 coneNorm(const glm::vec3& p) {
    // implicit normal (from handout): x, -(1/4)(2y-1), z then normalize
    return glm::normalize(glm::vec3(2.f*p.x, -(0.5f*p.y - 0.25f)*2.f, 2.f*p.z));
}
static inline float radiusAtY(float y) {
    float t = (y + 0.5f);            // 0 at base → 1 at tip
    return 0.5f * (1.f - t);         // 0.5 → 0
}

void Cone::updateParams(int p1, int p2) {
    m_vertexData.clear();
    m_param1 = std::max(1, p1);
    m_param2 = std::max(3, p2);
    setVertexData();
}

void Cone::setVertexData() {
    m_vertexData.clear();
    const int stacks = m_param1;
    const int wedges = m_param2;
    const float dT = glm::two_pi<float>() / wedges;
    const float yBot = -0.5f, yTop = 0.5f;
    const float dy = (yTop - yBot) / stacks;

    // bottom cap (y = -0.5)
    const glm::vec3 nCap(0, -1, 0);
    for (int k = 0; k < wedges; ++k) {
        float t0 = k*dT, t1 = (k+1)*dT;
        glm::vec3 C(0, yBot, 0), P0 = cyl(0.5f, t0, yBot), P1 = cyl(0.5f, t1, yBot);
        // flip winding so face is visible from above
        pushPosNorm(m_vertexData, C,  nCap);
        pushPosNorm(m_vertexData, P0, nCap);  // ← Swapped!
        pushPosNorm(m_vertexData, P1, nCap);
    }

    // side surface
    for (int k = 0; k < wedges; ++k) {
        float t0 = k*dT, t1 = (k+1)*dT;
        float tMid = 0.5f*(t0+t1);
        glm::vec3 tipDir = glm::normalize(glm::vec3(std::cos(tMid), 0, std::sin(tMid)));

        for (int i = 0; i < stacks; ++i) {
            float y0 = yBot + i*dy, y1 = yBot + (i+1)*dy;
            float r0 = radiusAtY(y0), r1 = radiusAtY(y1);

            glm::vec3 TL = cyl(r0, t0, y0);
            glm::vec3 TR = cyl(r0, t1, y0);
            glm::vec3 BL = cyl(r1, t0, y1);
            glm::vec3 BR = cyl(r1, t1, y1);

            glm::vec3 nTL = (r0==0.f) ? glm::normalize(glm::vec3(tipDir.x,1,tipDir.z)) : coneNorm(TL);
            glm::vec3 nTR = (r0==0.f) ? glm::normalize(glm::vec3(tipDir.x,1,tipDir.z)) : coneNorm(TR);
            glm::vec3 nBL = (r1==0.f) ? glm::normalize(glm::vec3(tipDir.x,1,tipDir.z)) : coneNorm(BL);
            glm::vec3 nBR = (r1==0.f) ? glm::normalize(glm::vec3(tipDir.x,1,tipDir.z)) : coneNorm(BR);

            // tri A
            pushPosNorm(m_vertexData, TL, nTL);
            pushPosNorm(m_vertexData, BL, nBL);
            pushPosNorm(m_vertexData, BR, nBR);
            // tri B
            pushPosNorm(m_vertexData, TL, nTL);
            pushPosNorm(m_vertexData, BR, nBR);
            pushPosNorm(m_vertexData, TR, nTR);
        }
    }
}
