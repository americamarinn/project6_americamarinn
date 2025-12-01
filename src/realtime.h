#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include "utils/sceneparser.h"
#include "utils/camera.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // (unused, but fine to keep)

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Scene and Camera
    RenderData m_renderData;
    Camera m_camera;

    // Interactive camera state (for movement)
    glm::vec3 m_camPos   = glm::vec3(0.f, 0.f, 5.f);
    glm::vec3 m_camDir   = glm::vec3(0.f, 0.f, -1.f);
    glm::vec3 m_camUp    = glm::vec3(0.f, 1.f, 0.f);
    float     m_moveSpeed = 4.f;
    float     m_turnSpeed = 0.003f;

    // Shader Program
    GLuint m_shader;

    // Shape VAO/VBO Management
    struct ShapeVAO {
        GLuint vao;
        GLuint vbo;
        int vertexCount;
    };
    std::vector<ShapeVAO> m_shapeVAOs;

    // Helper functions
    void initializeShaders();
    void loadScene();
    void generateShapeVAOs();
    void cleanupVAOs();
    std::vector<float> generateShapeData(PrimitiveType type, int param1, int param2);

    // Tick Related Variables
    int m_timer;
    QElapsedTimer m_elapsedTimer;
    // Input Related Variables
    bool m_mouseDown = false;
    glm::vec2 m_prev_mouse_pos;
    std::unordered_map<Qt::Key, bool> m_keyMap;

    // Device Correction Variables
    double m_devicePixelRatio;
};

