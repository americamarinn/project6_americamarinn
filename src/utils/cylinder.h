#pragma once
#include <vector>
#include <glm/glm.hpp>

class Cylinder {
public:
    // param1 = stacks (height), param2 = wedges (around)
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() const { return m_vertexData; }

private:
    void setVertexData();
    static inline void insertVec3(std::vector<float> &dst, const glm::vec3 &v) {
        dst.push_back(v.x); dst.push_back(v.y); dst.push_back(v.z);
    }

    std::vector<float> m_vertexData;
    int   m_param1 = 1;
    int   m_param2 = 16;
    float m_radius = 0.5f;
};
