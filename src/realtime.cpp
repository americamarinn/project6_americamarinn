#include "realtime.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "settings.h"
#include "utils/cube.h"
#include "utils/cone.h"
#include "utils/sphere.h"
#include "utils/cylinder.h"

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W] = false;
    m_keyMap[Qt::Key_A] = false;
    m_keyMap[Qt::Key_S] = false;
    m_keyMap[Qt::Key_D] = false;
    m_keyMap[Qt::Key_Space] = false;
    m_keyMap[Qt::Key_Control] = false;

    m_camPos = glm::vec3(0.f, 0.f, 5.f);
    m_camDir = glm::vec3(0.f, 0.f, -1.f);
    m_camUp  = glm::vec3(0.f, 1.f, 0.f);
}

void Realtime::finish() {
    killTimer(m_timer);
    makeCurrent();
    cleanupVAOs();
    doneCurrent();
}

// ============================
// INITIALIZE GL
// ============================
void Realtime::initializeGL() {
    m_devicePixelRatio = devicePixelRatio();
    glewInit();
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0,
               size().width()*m_devicePixelRatio,
               size().height()*m_devicePixelRatio);

    int W = size().width()*m_devicePixelRatio;
    int H = size().height()*m_devicePixelRatio;

    // === GBUFFER ===
    gbuffer = new GBuffer(W, H);

    // === Deferred renderer ===
    deferred = new DeferredRenderer();
    deferred->init();

    // === Post-process bloom ===
    post = new PostProcessPass();
    post->init();

    // === Load gbuffer shader ===
    shaderGBuffer = new ShaderProgram(
        ":/resources/shaders/gbuffer.vert",
        ":/resources/shaders/gbuffer.frag"
        );

    m_timer = startTimer(1000 / 60);
    m_elapsed.start();
}

// ============================
// RESIZE
// ============================
void Realtime::resizeGL(int w, int h) {
    int W = w * m_devicePixelRatio;
    int H = h * m_devicePixelRatio;
    glViewport(0,0,W,H);

    // Rebuild GBuffer at new size
    delete gbuffer;
    gbuffer = new GBuffer(W, H);

    float aspect = float(w)/float(h);
    float fov = 45.f * 3.14159f/180.f;

    m_camera.setProjectionMatrix(aspect,
                                 settings.nearPlane,
                                 settings.farPlane,
                                 fov);
}

// ============================
// GEOMETRY PASS
// ============================
void Realtime::renderGeometryPass() {
    gbuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderGBuffer->bind();
    shaderGBuffer->setUniformMat4("view", m_camera.getViewMatrix());
    shaderGBuffer->setUniformMat4("proj", m_camera.getProjMatrix());

    for (auto &obj : m_shapeVAOs) {
        shaderGBuffer->setUniformMat4("model", obj.ctm);

        glm::vec3 baseColor = glm::vec3(obj.mat.cDiffuse);
        glm::vec3 emissive  = glm::vec3(obj.mat.cEmissive);

        shaderGBuffer->setUniform3f("baseColor",
                                    baseColor.x, baseColor.y, baseColor.z);
        shaderGBuffer->setUniform3f("emissiveColor",
                                    emissive.x, emissive.y, emissive.z);

        glBindVertexArray(obj.vao);
        glDrawArrays(GL_TRIANGLES, 0, obj.vertexCount);
    }

    shaderGBuffer->unbind();
    gbuffer->unbind();
}

// ============================
// MAIN RENDER LOOP
// ============================
void Realtime::paintGL() {
    int W = size().width() * m_devicePixelRatio;
    int H = size().height() * m_devicePixelRatio;

    // 1. Geometry → GBuffer
    renderGeometryPass();

    // 2. Deferred lighting → full screen
    deferred->render(gbuffer, W, H);

    // 3. Bloom
    GLuint bright = post->extractBright(gbuffer->emissiveTex, W, H);
    GLuint blurred = post->blur(bright, W, H);
    post->composite(gbuffer->albedoTex, blurred, W, H);


    glBindVertexArray(0);
}

// ============================
// SCENE LOADING
// ============================
void Realtime::loadScene() {
    cleanupVAOs();

    std::string path = settings.sceneFilePath;
    if (path.empty()) return;

    if (!SceneParser::parse(path, m_renderData)) {
        std::cerr << "Failed to parse scene\n";
        return;
    }

    // Camera
    glm::vec3 pos = glm::vec3(m_renderData.cameraData.pos);
    glm::vec3 look = glm::vec3(m_renderData.cameraData.look);
    glm::vec3 up = glm::vec3(m_renderData.cameraData.up);

    m_camPos = pos;
    m_camDir = glm::normalize(look - pos);
    m_camUp  = glm::normalize(up);

    m_camera.setViewMatrix(m_camPos, m_camDir, m_camUp);

    float aspect = float(width())/float(height());
    float fov = m_renderData.cameraData.heightAngle;

    m_camera.setProjectionMatrix(aspect,
                                 settings.nearPlane,
                                 settings.farPlane,
                                 fov);

    generateShapeVAOs();
}

// ============================
// SHAPE VAO GENERATION
// ============================
void Realtime::generateShapeVAOs() {
    m_shapeVAOs.clear();

    for (const auto &shape : m_renderData.shapes) {
        ShapeVAO vao;

        std::vector<float> data = generateShapeData(
            shape.primitive.type,
            settings.shapeParameter1,
            settings.shapeParameter2
            );

        vao.ctm = shape.ctm;
        vao.mat = shape.primitive.material;

        glGenVertexArrays(1, &vao.vao);
        glGenBuffers(1, &vao.vbo);

        glBindVertexArray(vao.vao);
        glBindBuffer(GL_ARRAY_BUFFER, vao.vbo);

        glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float),
                     data.data(), GL_STATIC_DRAW);

        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false,
                              6*sizeof(float), (void*)0);

        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false,
                              6*sizeof(float), (void*)(3*sizeof(float)));

        glBindVertexArray(0);
        vao.vertexCount = data.size()/6;

        m_shapeVAOs.push_back(vao);
    }
}

std::vector<float> Realtime::generateShapeData(PrimitiveType t, int p1, int p2) {
    switch (t) {

    case PrimitiveType::PRIMITIVE_CUBE: {
        Cube c;
        c.updateParams(p1);
        return c.generateShape();
    }

    case PrimitiveType::PRIMITIVE_CONE: {
        Cone c;
        c.updateParams(p1, p2);
        return c.generateShape();
    }

    case PrimitiveType::PRIMITIVE_CYLINDER: {
        Cylinder c;
        c.updateParams(p1, p2);
        return c.generateShape();
    }

    case PrimitiveType::PRIMITIVE_SPHERE: {
        Sphere s;
        s.updateParams(p1, p2);
        return s.generateShape();
    }

    case PrimitiveType::PRIMITIVE_MESH:
        // Mesh rendering not implemented in your project
        return {};

    default:
        return {};
    }
}


void Realtime::cleanupVAOs() {
    for (auto &vao : m_shapeVAOs) {
        glDeleteBuffers(1, &vao.vbo);
        glDeleteVertexArrays(1, &vao.vao);
    }
    m_shapeVAOs.clear();
}

// ============================
// INPUT
// ============================
void Realtime::keyPressEvent(QKeyEvent *e) {
    m_keyMap[(Qt::Key)e->key()] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *e) {
    m_keyMap[(Qt::Key)e->key()] = false;
}

void Realtime::mousePressEvent(QMouseEvent *e) {
    if (e->buttons() & Qt::LeftButton) {
        m_mouseDown = true;
        m_prevMouse = glm::vec2(e->position().x(), e->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *e) {
    if (!(e->buttons() & Qt::LeftButton))
        m_mouseDown = false;
}

void Realtime::mouseMoveEvent(QMouseEvent *e) {
    if (!m_mouseDown) return;

    float dx = e->position().x() - m_prevMouse.x;
    float dy = e->position().y() - m_prevMouse.y;
    m_prevMouse = glm::vec2(e->position().x(), e->position().y());

    float yaw = -dx * m_turnSpeed;
    float pitch = -dy * m_turnSpeed;

    glm::vec3 right = glm::normalize(glm::cross(m_camDir, m_camUp));

    glm::mat4 yawMat = glm::rotate(glm::mat4(1.f), yaw, m_camUp);
    glm::mat4 pitchMat = glm::rotate(glm::mat4(1.f), pitch, right);

    glm::vec3 dir = glm::normalize(glm::vec3(yawMat * pitchMat * glm::vec4(m_camDir,0)));
    m_camDir = dir;

    right = glm::normalize(glm::cross(m_camDir, glm::vec3(0,1,0)));
    m_camUp = glm::normalize(glm::cross(right, m_camDir));

    m_camera.setViewMatrix(m_camPos, m_camDir, m_camUp);
}

void Realtime::timerEvent(QTimerEvent*) {
    float dt = m_elapsed.elapsed()*0.001f;
    m_elapsed.restart();

    glm::vec3 f = glm::normalize(m_camDir);
    glm::vec3 r = glm::normalize(glm::cross(f, m_camUp));
    glm::vec3 u = glm::normalize(m_camUp);

    if (m_keyMap[Qt::Key_W]) m_camPos += f * m_moveSpeed * dt;
    if (m_keyMap[Qt::Key_S]) m_camPos -= f * m_moveSpeed * dt;
    if (m_keyMap[Qt::Key_D]) m_camPos += r * m_moveSpeed * dt;
    if (m_keyMap[Qt::Key_A]) m_camPos -= r * m_moveSpeed * dt;
    if (m_keyMap[Qt::Key_Space]) m_camPos += u * m_moveSpeed * dt;
    if (m_keyMap[Qt::Key_Control]) m_camPos -= u * m_moveSpeed * dt;

    m_camera.setViewMatrix(m_camPos, m_camDir, m_camUp);
    update();
}

// ============================
// Settings changed
// ============================
void Realtime::sceneChanged() {
    makeCurrent();
    loadScene();
    doneCurrent();
    update();
}

void Realtime::settingsChanged() {
    makeCurrent();
    if (!m_renderData.shapes.empty())
        generateShapeVAOs();
    doneCurrent();
    update();
}

void Realtime::saveViewportImage(std::string filePath) {
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture, 0);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight,
                 GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored();

    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}

