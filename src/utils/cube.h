#pragma once
#include <vector>
#include <glm/glm.hpp>

class Cube {
public:
    void updateParams(int param1);
    std::vector<float> generateShape() const { return m_vertexData; }

private:
    void setVertexData();
    static inline void insertVec3(std::vector<float> &dst, const glm::vec3 &v) {
        dst.push_back(v.x); dst.push_back(v.y); dst.push_back(v.z);
    }
    void makeTile(glm::vec3 tl, glm::vec3 tr, glm::vec3 bl, glm::vec3 br);
    void makeFace(glm::vec3 tl, glm::vec3 tr, glm::vec3 bl, glm::vec3 br);

    std::vector<float> m_vertexData;
    int m_param1 = 1;
};
