#include "sphere.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <cmath>

static inline void pushPosNorm(std::vector<float>& dst,
                               const glm::vec3& p,
                               const glm::vec3& n) {
    dst.push_back(p.x); dst.push_back(p.y); dst.push_back(p.z);
    dst.push_back(n.x); dst.push_back(n.y); dst.push_back(n.z);
}

void Sphere::updateParams(int param1, int param2) {
    m_vertexData.clear();
    // stacks must be at least 2 to avoid degenerate top/bottom bands
    m_param1 = std::max(2, param1);
    m_param2 = std::max(3, param2);
    setVertexData();
}

void Sphere::setVertexData() {
    m_vertexData.clear();

    const int stacks = m_param1;
    const int slices = m_param2;

    // phi ∈ [0, π], theta ∈ [0, 2π)
    const float dPhi   = glm::pi<float>() / stacks;
    const float dTheta = glm::two_pi<float>() / slices;

    for (int i = 0; i < stacks; ++i) {
        float phi0 = i * dPhi;
        float phi1 = (i + 1) * dPhi;

        float y0 =  m_radius * std::cos(phi0);
        float y1 =  m_radius * std::cos(phi1);
        float r0 =  m_radius * std::sin(phi0);
        float r1 =  m_radius * std::sin(phi1);

        for (int j = 0; j < slices; ++j) {
            float th0 = j * dTheta;
            float th1 = (j + 1) * dTheta;

            glm::vec3 TL(r0 * std::cos(th0), y0, r0 * std::sin(th0));
            glm::vec3 TR(r0 * std::cos(th1), y0, r0 * std::sin(th1));
            glm::vec3 BL(r1 * std::cos(th0), y1, r1 * std::sin(th0));
            glm::vec3 BR(r1 * std::cos(th1), y1, r1 * std::sin(th1));

            glm::vec3 nTL = glm::normalize(TL);
            glm::vec3 nTR = glm::normalize(TR);
            glm::vec3 nBL = glm::normalize(BL);
            glm::vec3 nBR = glm::normalize(BR);


            // Upper tri
            pushPosNorm(m_vertexData, TL, nTL);
            pushPosNorm(m_vertexData, BL, nBL);
            pushPosNorm(m_vertexData, BR, nBR);
            // Lower tri
            pushPosNorm(m_vertexData, TL, nTL);
            pushPosNorm(m_vertexData, BR, nBR);
            pushPosNorm(m_vertexData, TR, nTR);
            // // Upper triangle (skip at north pole)
            // if (r0 > 0.f) {
            //     pushPosNorm(m_vertexData, TL, nTL);
            //     pushPosNorm(m_vertexData, BL, nBL);
            //     pushPosNorm(m_vertexData, BR, nBR);
            // }

            // // Lower triangle (skip at south pole)
            // if (r1 > 0.f) {
            //     pushPosNorm(m_vertexData, TL, nTL);
            //     pushPosNorm(m_vertexData, BR, nBR);
            //     pushPosNorm(m_vertexData, TR, nTR);
            // }
        }
    }
}
