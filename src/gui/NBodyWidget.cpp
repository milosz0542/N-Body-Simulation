//
// Created by milosz on 3/2/26.
//

#include "NBodyWidget.h"

NBodyWidget::NBodyWidget(QWidget *parent) : QOpenGLWidget(parent) { }

NBodyWidget::~NBodyWidget() = default;

void NBodyWidget::initializeGL() {
    initializeOpenGLFunctions();

    // Color of "cleaning" screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glEnable(GL_PROGRAM_POINT_SIZE); // Enable setting point size in vertex shader

    glEnable(GL_BLEND); // Enable Blend "mechanism"

    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Defining Blend Function

    // Loading Shaders
    shaderProgram = new QOpenGLShaderProgram(this);

    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/particle.vert")) {
        qCritical() << "Could not load vertex shader: " << shaderProgram->log();
    }

    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/particle.frag")) {
        qCritical() << "Could not load fragment shader: " << shaderProgram->log();
    }

    if (!shaderProgram->link()) {
        qCritical() << "Could not link shader program: " << shaderProgram->log();
    }

    VAO.create();
    VAO.bind();

    VBO.create();
    VBO.bind();

    VBO.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    size_t max_particles = 100000;
    VBO.allocate(max_particles * 5 * sizeof(GLfloat));

    int stride = 5 * sizeof(GLfloat);

    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    this->glEnableVertexAttribArray(0);

    this->glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    this->glEnableVertexAttribArray(1);

    this->glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(GLfloat)));
    this->glEnableVertexAttribArray(2);

    VBO.release();
    VAO.release();

    // Initialize Star Shader
    starShaderProgram = new QOpenGLShaderProgram(this);
    if (!starShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/starfield.vert")) {
        qCritical() << "Could not load starfield vertex shader: " << starShaderProgram->log();
    }
    if (!starShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/starfield.frag")) {
        qCritical() << "Could not load starfield fragment shader: " << starShaderProgram->log();
    }
    if (!starShaderProgram->link()) {
        qCritical() << "Could not link starfield shader program: " << starShaderProgram->log();
    }

    // Generate star data
    std::vector<GLfloat> starData;
    starData.reserve(numStars * 4); // X, Y, Z, Brightness

    for (int i = 0; i < numStars; ++i) {
        float u = (std::rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        float theta = (std::rand() / (float)RAND_MAX) * 2.0f * M_PI;

        // Sphere radius
        float r = 800.0f;

        float x = r * std::sqrt(1.0f - u * u) * std::cos(theta);
        float y = r * std::sqrt(1.0f - u * u) * std::sin(theta);
        float z = r * u;

        // Random Brightness (0.2-1.0)
        float brightness = 0.2f + 0.8f * (std::rand() / static_cast<float>(RAND_MAX));

        starData.push_back(x);
        starData.push_back(y);
        starData.push_back(z);
        starData.push_back(brightness);
    }

    starVAO.create();
    starVAO.bind();

    starVBO.create();
    starVBO.bind();
    starVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
    starVBO.allocate(starData.data(), starData.size() * sizeof(GLfloat));

    // Attrib 0 (X, Y, Z)
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    this->glEnableVertexAttribArray(0);

    // Attrib 1 (Brightness)
    this->glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    this->glEnableVertexAttribArray(1);

    starVBO.release();
    starVAO.release();

    // Initialize trail shader
    trailShaderProgram = new QOpenGLShaderProgram(this);
    bool trailShaderOk = true;
    if (!trailShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/trail.vert")) {
        qCritical() << "Could not load trail vertex shader:" << trailShaderProgram->log();
        trailShaderOk = false;
    }
    if (!trailShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/trail.frag")) {
        qCritical() << "Could not load trail fragment shader:" << trailShaderProgram->log();
        trailShaderOk = false;
    }
    if (trailShaderOk && !trailShaderProgram->link()) {
        qCritical() << "Could not link trail shader program:" << trailShaderProgram->log();
        trailShaderOk = false;
    }
    if (!trailShaderOk) {
        // Do not proceed with trail initialization if shader program is invalid
        return;
    }

    glGenBuffers(1, &m_trailVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_trailVBO);

    GLsizeiptr bufferSize = MAX_BODIES * MAX_TRAIL_LENGTH * 3 * sizeof(float);

    GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    glBufferStorage(GL_ARRAY_BUFFER, bufferSize, nullptr, flags);

    m_trailMappedPtr = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, bufferSize, flags);

    trailVAO.create();
    trailVAO.bind();

    glBindBuffer(GL_ARRAY_BUFFER, m_trailVBO);

    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    this->glEnableVertexAttribArray(0);

    engine->m_maxTrailLength = MAX_TRAIL_LENGTH;
}

void NBodyWidget::resizeGL(int w, int h) {
    // "Telling" GPU what part of the window we want to render to. In this case, whole window
    glViewport(0, 0, w, h);

    float aspect_ratio = static_cast<float>(w) / static_cast<float>(h);

    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0f, aspect_ratio, 0.1f, 1000.0f);
}

void NBodyWidget::paintGL() {
    // Clear the screen with color set in initializeGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!engine || engine->getBodies().empty()) return;

    if (m_drawStarfield) {
        starShaderProgram->bind();

        // Magic trick
        QMatrix4x4 starViewMatrix = viewMatrix;
        starViewMatrix.setColumn(3, QVector4D(0.0f, 0.0f, 0.0f, 1.0f));

        starShaderProgram->setUniformValue("projection", projectionMatrix);
        starShaderProgram->setUniformValue("view", starViewMatrix);

        starVAO.bind();

        glEnable(GL_PROGRAM_POINT_SIZE);

        this->glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(numStars));

        starVAO.release();
        starShaderProgram->release();
    }

    // Activate shaders
    shaderProgram->bind();

    if (m_trackedPlanetIndex >= 0 && m_trackedPlanetIndex < engine->getBodies().size()) {
        Eigen::Vector3f planetPos = engine->getBodies()[m_trackedPlanetIndex].position;
        targetCameraTarget = QVector3D(planetPos.x(), planetPos.y(), planetPos.z());
    } else {
        targetCameraTarget = QVector3D(0.0f, 0.0f, 0.0f);
    }

    // Camera interpolate
    float lerpFactor = 0.1f;

    if (m_cinematicMode) {
        targetYaw += 0.5f;
    }

    cameraDistance += (targetDistance - cameraDistance) * lerpFactor;
    cameraPitch += (targetPitch - cameraPitch) * lerpFactor;
    cameraYaw += (targetYaw - cameraYaw) * lerpFactor;

    cameraTarget.setX(cameraTarget.x() + (targetCameraTarget.x() - cameraTarget.x()) * lerpFactor);
    cameraTarget.setY(cameraTarget.y() + (targetCameraTarget.y() - cameraTarget.y()) * lerpFactor);
    cameraTarget.setZ(cameraTarget.z() + (targetCameraTarget.z() - cameraTarget.z()) * lerpFactor);

    // Building inteligent matrix
    viewMatrix.setToIdentity();

    // 1. Move camera back from the target
    viewMatrix.translate(0.0f, 0.0f, -cameraDistance);

    // 2. Rotate around the target (pitch and then yaw)
    viewMatrix.rotate(cameraPitch, 1.0f, 0.0f, 0.0f);
    viewMatrix.rotate(cameraYaw, 0.0f, 1.0f, 0.0f);

    // 3. Move the origin to the target position
    viewMatrix.translate(-cameraTarget.x(), -cameraTarget.y(), -cameraTarget.z());

    // Send matrix to uniform
    shaderProgram->setUniformValue("projection", projectionMatrix);
    shaderProgram->setUniformValue("view", viewMatrix);

    QMatrix4x4 modelMatrix;
    shaderProgram->setUniformValue("model", modelMatrix);

    shaderProgram->setUniformValue("useVelocityColor", m_useVelocityColor);

    RenderUtils::fillPositionBuffer(engine->getBodies(), m_gpuPositionBuffer);

    // Now the magic that I do not understand begins
    VAO.bind();
    VBO.bind();
    VBO.write(0, m_gpuPositionBuffer.data(), m_gpuPositionBuffer.size() * sizeof(GLfloat));

    // Draw
    this->glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(engine->getBodies().size()));
    VBO.release();
    VAO.release();

    if (m_drawTrails) {
        trailShaderProgram->bind();
        trailShaderProgram->setUniformValue("projection", projectionMatrix);
        trailShaderProgram->setUniformValue("view", viewMatrix);

        QMatrix4x4 identityModel;
        trailShaderProgram->setUniformValue("model", identityModel);

        const auto& bodies = engine->getBodies();

        int shaderTrailLength = (engine->m_maxTrailLength > 0) ? engine->m_maxTrailLength : 100;
        trailShaderProgram->setUniformValue("maxTrailLength", shaderTrailLength);

        // SAVE TO VRAM
        if (m_trailMappedPtr) {
            int floatIndex = 0;
            for (const auto& body : bodies) {
                const auto history = body.getTrailHistory();
                size_t histSize = history.size();

                for (size_t i = 0; i < histSize; ++i) {
                    m_trailMappedPtr[floatIndex++] = history[i].x();
                    m_trailMappedPtr[floatIndex++] = history[i].y();
                    m_trailMappedPtr[floatIndex++] = history[i].z();
                }
            }
        }

        trailVAO.bind();

        int offset = 0;
        for (const auto& body : bodies) {
            int count = body.getTrailHistory().size();
            if (count > 1) {
                trailShaderProgram->setUniformValue("startOffset", offset);

                this->glDrawArrays(GL_LINE_STRIP, offset, count);
            }
            offset += count;
        }


        trailVAO.release();
        trailShaderProgram->release();
    }

    shaderProgram->release();
    // And magic ends

    emit frameRendered(); // Emits signal for FPS/TPS show.
}

void NBodyWidget::mousePressEvent(QMouseEvent *event) {
    lastMousePos = event->pos();
}

void NBodyWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->position().x() - lastMousePos.x();
    int dy = event->position().y() - lastMousePos.y();

    if (event->buttons() & Qt::LeftButton) {
        // Orbiting
        float sensitivity = 0.25f;

        targetYaw += dx * sensitivity;
        targetPitch += dy * sensitivity;

        if (targetPitch > 89.0f) targetPitch = 89.0f;
        if (targetPitch < -89.0f) targetPitch = -89.0f;

        update();
    } else if (event->buttons() & Qt::RightButton) {
        // Panning relative to the camera view
        if (m_trackedPlanetIndex != -1) m_trackedPlanetIndex = -1;

        // Sensivity proportional to distance
        float panSensitivity = targetDistance * 0.001f;

        // 1. Calculating world-space panning vectors based on view matrix orientation
        // View matrix is LookAt * Translate(-target). We invert the rotation part of the matrix.
        QMatrix4x4 rotationOnly = viewMatrix;
        rotationOnly.setColumn(3, QVector4D(0, 0, 0, 1));
        QMatrix4x4 invRotation = rotationOnly.inverted();

        // Screen space axes
        QVector3D screenX = (invRotation * QVector4D(1, 0, 0, 0)).toVector3D();
        QVector3D screenY = (invRotation * QVector4D(0, 1, 0, 0)).toVector3D();

        // 2. Adjust target position by the panned amount
        targetCameraTarget -= screenX * dx * panSensitivity;
        targetCameraTarget += screenY * dy * panSensitivity;

        update();
    }

    lastMousePos = event->pos();
}

void NBodyWidget::wheelEvent(QWheelEvent *event) {
    float scrollAmount = event->angleDelta().y() / 120.0f;

    // Zooming velocity proportional to the distance
    float zoomFactor = 0.15f;

    targetDistance -= scrollAmount * targetDistance * zoomFactor;

    if (targetDistance < 0.1f) targetDistance = 0.1f;
    if (targetDistance > 5000.0f) targetDistance = 5000.0f;

    update();
}