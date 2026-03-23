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
        float r = 80.0f;

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
    trailShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/trail.vert");
    trailShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/trail.frag");
    trailShaderProgram->link();

    trailVAO.create();
    trailVAO.bind();

    trailVBO.create();
    trailVBO.bind();
    trailVBO.setUsagePattern(QOpenGLBuffer::DynamicDraw); // Dynamic draw, because trail updates every frame

    trailVBO.allocate(100000 * 100 * 4 * sizeof(GLfloat));

    // Attrib 0 (X, Y, Z)
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    this->glEnableVertexAttribArray(0);

    // Attrib 1 (Alpha)
    this->glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    this->glEnableVertexAttribArray(1);

    trailVBO.release();
    trailVAO.release();
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

    viewMatrix.setToIdentity();
    viewMatrix.translate(0.0f, 0.0f, -50.0f);

    // Send matrix to uniform
    shaderProgram->setUniformValue("projection", projectionMatrix);
    shaderProgram->setUniformValue("view", viewMatrix);

    QMatrix4x4 modelMatrix;
    shaderProgram->setUniformValue("model", modelMatrix);

    shaderProgram->setUniformValue("useVelocityColor", m_useVelocityColor);

    gpuPositionBuffer = RenderUtils::preparePositionBuffer(engine->getBodies());

    // Now the magic that I do not understand begins
    VAO.bind();
    VBO.bind();
    VBO.write(0, gpuPositionBuffer.data(), gpuPositionBuffer.size() * sizeof(GLfloat));

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

        trailVAO.bind();
        trailVBO.bind();

        std::vector<float> trailData;
        const auto& bodies = engine->getBodies();

        size_t totalPoints = 0;
        for (const auto& body : bodies) totalPoints += body.getTrailHistory().size();
        trailData.reserve(totalPoints * 4);

        for (const auto& body : bodies) {
            const auto history = body.getTrailHistory();
            size_t histSize = history.size();

            for (size_t i = 0; i < histSize; ++i) {
                trailData.push_back(history[i].x());
                trailData.push_back(history[i].y());
                trailData.push_back(history[i].z());

                float alpha = 1.0f - (static_cast<float>(i) / static_cast<float>(histSize));
                trailData.push_back(alpha);
            }
        }

        if (!trailData.empty()) {
            trailVBO.write(0, trailData.data(), trailData.size() * sizeof(GLfloat));
        }

        int offset = 0;

        for (const auto& body : bodies) {
            int count = body.getTrailHistory().size();
            if (count > 1) {
                // Draw LINE STRIP
                this->glDrawArrays(GL_LINE_STRIP, offset, count);
            }
            offset += count;
        }

        trailVBO.release();
        trailVAO.release();
        trailShaderProgram->release();
    }

    shaderProgram->release();
    // And magic ends
}
