#include "cylinder.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <cmath>

static inline void pushPosNorm(std::vector<float>& dst,
                               const glm::vec3& p,
                               const glm::vec3& n) {
    dst.push_back(p.x); dst.push_back(p.y); dst.push_back(p.z);
    dst.push_back(n.x); dst.push_back(n.y); dst.push_back(n.z);
}
static inline glm::vec3 cyl(float r, float th, float y) {
    return {r*std::cos(th), y, r*std::sin(th)};
}

void Cylinder::updateParams(int p1, int p2) {
    m_vertexData.clear();
    m_param1 = std::max(1, p1);
    m_param2 = std::max(3, p2);
    setVertexData();
}

void Cylinder::setVertexData() {
    m_vertexData.clear();

    const int stacks = m_param1;
    const int wedges = m_param2;
    const float r    = m_radius;
    const float yTop =  0.5f, yBot = -0.5f;
    const float dT   = glm::two_pi<float>() / wedges;
    const float dy   = (yTop - yBot) / stacks;

    // bottom cap (normal down)
    for (int k = 0; k < wedges; ++k) {
        float t0 = k*dT, t1 = (k+1)*dT;
        glm::vec3 C(0,yBot,0), P0 = cyl(r,t0,yBot), P1 = cyl(r,t1,yBot);
        glm::vec3 n(0,-1,0);
        // wind so face is visible from above
        pushPosNorm(m_vertexData, C,  n);
        pushPosNorm(m_vertexData, P0, n);  // ← Swapped!
        pushPosNorm(m_vertexData, P1, n);
    }

    // top cap (normal up)
    for (int k = 0; k < wedges; ++k) {
        float t0 = k*dT, t1 = (k+1)*dT;
        glm::vec3 C(0,yTop,0), P0 = cyl(r,t0,yTop), P1 = cyl(r,t1,yTop);
        glm::vec3 n(0,1,0);
        pushPosNorm(m_vertexData, C,  n);
        pushPosNorm(m_vertexData, P1, n);  // ← P1 first
        pushPosNorm(m_vertexData, P0, n);
    }

    // side surface
    for (int k = 0; k < wedges; ++k) {
        float t0 = k*dT, t1 = (k+1)*dT;
        glm::vec3 n0 = glm::normalize(glm::vec3(std::cos(t0),0,std::sin(t0)));
        glm::vec3 n1 = glm::normalize(glm::vec3(std::cos(t1),0,std::sin(t1)));

        for (int i = 0; i < stacks; ++i) {
            float y0 = yBot + i*dy;
            float y1 = yBot + (i+1)*dy;

            glm::vec3 TL = cyl(r,t0,y0);
            glm::vec3 TR = cyl(r,t1,y0);
            glm::vec3 BL = cyl(r,t0,y1);
            glm::vec3 BR = cyl(r,t1,y1);

            // tri A
            pushPosNorm(m_vertexData, TL, n0);
            pushPosNorm(m_vertexData, BL, n0);
            pushPosNorm(m_vertexData, BR, n1);
            // tri B
            pushPosNorm(m_vertexData, TL, n0);
            pushPosNorm(m_vertexData, BR, n1);
            pushPosNorm(m_vertexData, TR, n1);
        }
    }
}
