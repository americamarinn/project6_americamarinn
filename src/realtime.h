#pragma once
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <QOpenGLWidget>
#include <QElapsedTimer>
#include <QTime>
#include <unordered_map>

#include "utils/sceneparser.h"
#include "utils/camera.h"

#include "utils/gbuffer.h"
#include "utils/deferredrenderer.h"
#include "utils/postprocesspass.h"
#include "utils/shaderprogram.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();

    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent*) override;
    void keyReleaseEvent(QKeyEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;

private:
    // Geometry pass
    void renderGeometryPass();

    // Scene data
    RenderData m_renderData;
    Camera m_camera;


    // Camera state
    glm::vec3 m_camPos;
    glm::vec3 m_camDir;
    glm::vec3 m_camUp;
    float m_moveSpeed = 4.f;
    float m_turnSpeed = 0.003f;

    // Input state
    bool m_mouseDown = false;
    glm::vec2 m_prevMouse;
    std::unordered_map<Qt::Key, bool> m_keyMap;

    // Timing
    int m_timer;
    QElapsedTimer m_elapsed;

    double m_devicePixelRatio;

    // === NEW DEFERRED PIPELINE ===
    GBuffer* gbuffer = nullptr;
    DeferredRenderer* deferred = nullptr;
    PostProcessPass* post = nullptr;

    ShaderProgram* shaderGBuffer = nullptr;

    // CPU-side VAO management for shapes
    struct ShapeVAO {
        GLuint vao;
        GLuint vbo;
        int vertexCount;
        glm::mat4 ctm;
        SceneMaterial mat;
    };
    std::vector<ShapeVAO> m_shapeVAOs;

    void loadScene();
    void generateShapeVAOs();
    std::vector<float> generateShapeData(PrimitiveType type, int p1, int p2);
    void cleanupVAOs();
};

