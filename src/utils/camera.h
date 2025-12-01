#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera();

    // View setup: position, look direction, and up vector (all vec3)
    void setViewMatrix(const glm::vec3 &pos,
                       const glm::vec3 &look,
                       const glm::vec3 &up);

    // Projection setup: aspect ratio, near/far planes, and vertical FOV (radians)
    void setProjectionMatrix(float aspect,
                             float nearPlane,
                             float farPlane,
                             float heightAngle);

    // Accessors used by Realtime
    const glm::mat4 &getViewMatrix()  const { return m_view; }
    const glm::mat4 &getProjMatrix()  const { return m_proj; }
    glm::vec3        getPosition()    const { return m_pos;  }
    glm::vec3        getLook()        const { return m_look; }

    glm::mat4 getProjectionMatrix() const { return m_proj; }

    // Optional movement helpers (match your existing cpp)
    void translate(const glm::vec3 &delta);
    void rotateAroundUp(float angle);
    void rotateAroundRight(float angle);

private:
    void rebuildView();

    glm::vec3 m_pos;
    glm::vec3 m_look;
    glm::vec3 m_up;

    float m_aspect;
    float m_near;
    float m_far;
    float m_fovy;

    glm::mat4 m_view;
    glm::mat4 m_proj;
};
