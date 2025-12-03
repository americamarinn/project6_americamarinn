#pragma once
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <QOpenGLWidget>
#include <QElapsedTimer>
#include <unordered_map>

#include "utils/sceneparser.h"
#include "utils/camera.h"
#include "utils/gbuffer.h"
#include "utils/deferredrenderer.h"

class Realtime : public QOpenGLWidget {
public:
    Realtime(QWidget* parent = nullptr);
    void finish();
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(const std::string& path);


protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void timerEvent(QTimerEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void keyReleaseEvent(QKeyEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;

private:
    RenderData m_renderData;
    Camera m_camera;

    glm::vec3 m_camPos;
    glm::vec3 m_camDir;
    glm::vec3 m_camUp;

    bool m_mouseDown;
    glm::vec2 m_prevMouse;

    std::unordered_map<Qt::Key, bool> m_keys;

    int m_timer;
    QElapsedTimer m_elapsed;

    double m_dpr;

    struct ShapeVAO {
        GLuint vao;
        GLuint vbo;
        int count;
        glm::mat4 ctm;
        SceneMaterial mat;
    };
    std::vector<ShapeVAO> m_shapes;

    GBuffer gbuffer;
    DeferredRenderer deferred;

    void loadScene();
    void generateShapeVAOs();
    void cleanupVAOs();

    void renderGeometryPass();

};
