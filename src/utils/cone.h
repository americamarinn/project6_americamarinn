#pragma once
#include <vector>
#include <glm/glm.hpp>

class Cone {
public:
    void updateParams(int param1, int param2); // stacks, wedges
    std::vector<float> generateShape() const { return m_vertexData; }

private:
    void setVertexData();
    static inline void insertVec3(std::vector<float> &dst, const glm::vec3 &v) {
        dst.push_back(v.x); dst.push_back(v.y); dst.push_back(v.z);
    }

    std::vector<float> m_vertexData;
    int   m_param1 = 1;  // stacks along height
    int   m_param2 = 3;  // wedges around
    float m_radius = 0.5f;
};
