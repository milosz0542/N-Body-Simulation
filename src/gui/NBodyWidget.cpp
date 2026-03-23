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

    VAO.create();
    VAO.bind();

    VBO.create();
    VBO.bind();

    VBO.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    size_t max_particles = 100000;
    VBO.allocate(max_particles * 3 * sizeof(GLfloat));

    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), nullptr);
    this->glEnableVertexAttribArray(0);

    VBO.release();
    VAO.release();
}

void NBodyWidget::resizeGL(int w, int h) {
    // "Telling" GPU what part of the window we want to render to. In this case, whole window
    glViewport(0, 0, w, h);
}

void NBodyWidget::paintGL() {
    // Clear the screen with color set in initializeGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!engine || engine->getBodies().empty()) return;

    gpuPositionBuffer = RenderUtils::preparePositionBuffer(engine->getBodies());

    // Now the magic that I do not understand begins
    VAO.bind();
    VBO.bind();
    VBO.write(0, gpuPositionBuffer.data(), gpuPositionBuffer.size() * sizeof(GLfloat));

    // Draw
    this->glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(engine->getBodies().size()));
    VBO.release();
    VAO.release();
    // And magic ends
}
