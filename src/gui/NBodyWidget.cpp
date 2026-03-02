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
}

void NBodyWidget::resizeGL(int w, int h) {
    // "Telling" GPU what part of the window we want to render to. In this case, whole window
    glViewport(0, 0, w, h);
}

void NBodyWidget::paintGL() {
    // Clear the screen with color set in initializeGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
