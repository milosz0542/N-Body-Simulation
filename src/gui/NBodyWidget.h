//
// Created by milosz on 3/2/26.
//

#pragma once

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <vector>
#include "core/GravityEngine.h"
#include "render/RenderUtils.h"

class NBodyWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT // Required by signals mechanism and Qt slots

public:
    explicit NBodyWidget(QWidget *parent = nullptr);
    ~NBodyWidget() override;

    void setDrawStarfield(bool draw) { m_drawStarfield = draw; update(); }
    void setDrawTrails(bool draw) { m_drawTrails = draw; update(); }
    void setUseVelocityColor(bool use) { m_useVelocityColor = use; update(); }

    void setEngine(GravityEngine *eng) { engine=eng; };
protected:
    // Called once at start. Loading shaders, and setting default state
    void initializeGL() override;

    // Called when window is resized
    void resizeGL(int w, int h) override;

    // Called at every frame. Rendering code goes here
    void paintGL() override;

private:
    GravityEngine* engine = nullptr;

    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;

    // Auxiliary buffer
    std::vector<float> gpuPositionBuffer;

    QOpenGLShaderProgram* shaderProgram;

    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;

    bool m_drawStarfield = true;
    bool m_drawTrails = true;
    bool m_useVelocityColor = true;

    QOpenGLShaderProgram* starShaderProgram;
    QOpenGLVertexArrayObject starVAO;
    QOpenGLBuffer starVBO;
    int numStars = 10000;

    QOpenGLShaderProgram* trailShaderProgram;
    QOpenGLVertexArrayObject trailVAO;
    QOpenGLBuffer trailVBO;
};