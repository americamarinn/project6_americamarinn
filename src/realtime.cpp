#include "realtime.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QMouseEvent>
#include <QKeyEvent>
#include "settings.h"
#include "utils/cube.h"
#include "utils/cone.h"
#include "utils/cylinder.h"
#include "utils/sphere.h"



Realtime::Realtime(QWidget* parent)
    : QOpenGLWidget(parent) {
    m_camPos = glm::vec3(0,0,5);
    m_camDir = glm::vec3(0,0,-1);
    m_camUp  = glm::vec3(0,1,0);
}

void Realtime::initializeGL() {
    glewInit();
    m_dpr = devicePixelRatio();
    gbuffer.init(width()*m_dpr, height()*m_dpr);
    deferred.init();
    m_timer = startTimer(16);
}

void Realtime::paintGL() {
    renderGeometryPass();
    deferred.render(&gbuffer, m_renderData.lights, m_camPos);
}

void Realtime::resizeGL(int w, int h) {
    gbuffer.init(w*m_dpr, h*m_dpr);
    float aspect = float(w)/float(h);
    m_camera.setProjectionMatrix(aspect, 0.1f, 100.f, m_renderData.cameraData.heightAngle);
}

void Realtime::loadScene() {
    SceneParser::parse(settings.sceneFilePath, m_renderData);

    glm::vec3 pos = glm::vec3(m_renderData.cameraData.pos);
    glm::vec3 look = glm::vec3(m_renderData.cameraData.look);
    glm::vec3 up = glm::normalize(glm::vec3(m_renderData.cameraData.up));

    m_camPos = pos;
    m_camDir = glm::normalize(look - pos);
    m_camUp = up;

    float aspect = float(width()) / float(height());
    m_camera.setViewMatrix(m_camPos, m_camDir, m_camUp);
    m_camera.setProjectionMatrix(aspect, 0.1f, 100.f, m_renderData.cameraData.heightAngle);

    generateShapeVAOs();
}

void Realtime::generateShapeVAOs() {
    cleanupVAOs();

    for (auto &s : m_renderData.shapes) {

        std::vector<float> data;

        switch (s.primitive.type) {

        case PrimitiveType::PRIMITIVE_CUBE: {
            Cube c;
            c.updateParams(settings.shapeParameter1);  // ONLY 1 argument
            data = c.generateShape();
            break;
        }

        case PrimitiveType::PRIMITIVE_SPHERE: {
            Sphere sph;
            sph.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            data = sph.generateShape();
            break;
        }

        case PrimitiveType::PRIMITIVE_CYLINDER: {
            Cylinder cyl;
            cyl.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            data = cyl.generateShape();
            break;
        }

        case PrimitiveType::PRIMITIVE_CONE: {
            Cone cn;
            cn.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            data = cn.generateShape();
            break;
        }

        default:
            continue;
        }

        ShapeVAO V;
        V.count = data.size() / 6;
        V.ctm = s.ctm;
        V.mat = s.primitive.material;

        glGenVertexArrays(1, &V.vao);
        glGenBuffers(1, &V.vbo);

        glBindVertexArray(V.vao);
        glBindBuffer(GL_ARRAY_BUFFER, V.vbo);

        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        m_shapes.push_back(V);
    }
}




void Realtime::renderGeometryPass() {
    gbuffer.bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint s = deferred.shaderDeferred->id;
    glUseProgram(s);

    for (auto& V : m_shapes) {
        glUniformMatrix4fv(glGetUniformLocation(s, "model"), 1, GL_FALSE, &V.ctm[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(s, "view"), 1, GL_FALSE, &m_camera.getViewMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(s, "proj"), 1, GL_FALSE, &m_camera.getProjMatrix()[0][0]);

        glm::vec3 cDiffuse = glm::vec3(V.mat.cDiffuse);
        glm::vec3 cEmissive = glm::vec3(V.mat.cEmissive);

        glUniform3fv(glGetUniformLocation(s, "cDiffuse"), 1, &cDiffuse[0]);
        glUniform3fv(glGetUniformLocation(s, "cEmissive"), 1, &cEmissive[0]);

        glBindVertexArray(V.vao);
        glDrawArrays(GL_TRIANGLES, 0, V.count);
    }

    gbuffer.unbind();
}

void Realtime::cleanupVAOs() {
    for (auto& s : m_shapes) {
        glDeleteBuffers(1, &s.vbo);
        glDeleteVertexArrays(1, &s.vao);
    }
    m_shapes.clear();
}

void Realtime::timerEvent(QTimerEvent*) {
    update();
}

void Realtime::sceneChanged() {
    makeCurrent();
    loadScene();
    doneCurrent();
    update();
}

void Realtime::settingsChanged() {
    makeCurrent();
    generateShapeVAOs();
    doneCurrent();
    update();
}

void Realtime::saveViewportImage(const std::string& path) {
    QImage img = grabFramebuffer();
    img.save(QString::fromStdString(path));
}


void Realtime::keyPressEvent(QKeyEvent* e) {}
void Realtime::keyReleaseEvent(QKeyEvent* e) {}
void Realtime::mousePressEvent(QMouseEvent* e) {}
void Realtime::mouseReleaseEvent(QMouseEvent* e) {}
void Realtime::mouseMoveEvent(QMouseEvent* e) {}
void Realtime::finish() {}
