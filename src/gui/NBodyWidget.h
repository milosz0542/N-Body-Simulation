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
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPoint>


/**
 * @class NBodyWidget
 * @brief An OpenGL widget for rendering the n-body simulation.
 *
 * This widget handles the 3D visualization of celestial bodies, including their
 * positions, trails, and a background starfield. It provides camera controls
 * (orbiting, zooming) and supports different rendering modes.
 */
class NBodyWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT // Required by signals mechanism and Qt slots

public:
    /**
     * @brief Constructs an NBodyWidget.
     * @param parent The parent widget.
     */
    explicit NBodyWidget(QWidget *parent = nullptr);

    /**
     * @brief Destroys the NBodyWidget.
     */
    ~NBodyWidget() override;

    /**
     * @brief Sets whether to draw the background starfield.
     * @param draw True to draw the starfield, false otherwise.
     */
    void setDrawStarfield(bool draw) { m_drawStarfield = draw; update(); }

    /**
     * @brief Sets whether to draw trails for the celestial bodies.
     * @param draw True to draw trails, false otherwise.
     */
    void setDrawTrails(bool draw) { m_drawTrails = draw; update(); }

    /**
     * @brief Sets whether to color the celestial bodies based on their velocity.
     * @param use True to use velocity-based coloring, false otherwise.
     *//**
 * Constructs an NBodyWidget instance.
 *
 * @param parent The parent widget for this NBodyWidget. Can be nullptr for no parent.
 */
    void setUseVelocityColor(bool use) { m_useVelocityColor = use; update(); }

    /**
     * @brief Sets whether the widget is in cinematic mode.
     * @param mode True to enable cinematic mode, false otherwise.
     */
    void setCinematicMode(bool mode) { m_cinematicMode = mode; update(); }

    /**
     * @brief Sets the gravity engine to be used for rendering.
     * @param eng A pointer to the GravityEngine instance.
     */
    void setEngine(GravityEngine *eng) { engine=eng; }

    /**
     * @brief Sets the index of the planet to be tracked by the camera.
     * @param index The index of the planet in the gravity engine's collection. Set to -1 for free camera.
     */
    void setTrackedPlanetIndex(int index) { m_trackedPlanetIndex = index; update(); }

protected:
    /**
     * @brief Initializes OpenGL resources and state.
     * Called once before the first call to paintGL() or resizeGL().
     */
    void initializeGL() override;

    /**
     * @brief Handles widget resizing by updating the projection matrix.
     * @param w The new width.
     * @param h The new height.
     */
    void resizeGL(int w, int h) override;

    /**
     * @brief Renders the 3D scene.
     * Called whenever the widget needs to be repainted.
     */
    void paintGL() override;

    /**
     * @brief Handles mouse press events for camera control.
     * @param event The mouse event.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Handles mouse move events for camera orbiting.
     * @param event The mouse event.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Handles mouse wheel events for zooming.
     * @param event The wheel event.
     */
    void wheelEvent(QWheelEvent *event) override;

private:
    GravityEngine* engine = nullptr;

    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;

    // Auxiliary buffer
    std::vector<float> gpuPositionBuffer;

    QOpenGLShaderProgram* shaderProgram;

    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;

    bool m_drawStarfield = false;
    bool m_drawTrails = false;
    bool m_useVelocityColor = true;
    bool m_cinematicMode = false;
    int m_trackedPlanetIndex = -1;

    QOpenGLShaderProgram* starShaderProgram;
    QOpenGLVertexArrayObject starVAO;
    QOpenGLBuffer starVBO;
    int numStars = 5000;

    QOpenGLShaderProgram* trailShaderProgram;
    QOpenGLVertexArrayObject trailVAO;
    QOpenGLBuffer trailVBO;

    // OpenGL Camera Parameters
    QPoint lastMousePos;

    float cameraDistance = 50.0f;
    float cameraPitch = 0.0f;
    float cameraYaw = 0.0f;
    QVector3D cameraTarget = QVector3D(0.0f, 0.0f, 0.0f);

    float targetDistance = 50.0f;
    float targetPitch = 0.0f;
    float targetYaw = 0.0f;
    QVector3D targetCameraTarget = QVector3D(0.0f, 0.0f, 0.0f);

signals:
    void frameRendered();
};