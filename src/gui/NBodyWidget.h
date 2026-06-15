/**
 * @file NBodyWidget.h
 * @brief Declaration of the OpenGL widget used to render the N-body simulation scene.
 *
 * This file defines @ref NBodyWidget, a Qt/OpenGL widget responsible for drawing:
 * - Celestial body point sprites
 * - Optional body trails
 * - Optional background starfield
 *
 * It also handles user camera interaction (orbit + zoom) and optional planet tracking.
 */
#pragma once

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPoint>

#include <vector>

#include "core/GravityEngine.h"
#include "render/RenderUtils.h"

/**
 * @class NBodyWidget
 * @brief OpenGL view widget for real-time visualization of an N-body simulation.
 *
 * Responsibilities:
 * - Manage OpenGL resources (VAO/VBO/shaders)
 * - Render simulation bodies
 * - Render optional trails and starfield
 * - Maintain and apply camera state
 * - Support user input for camera movement
 * - Emit frame completion signal for FPS-style monitoring
 *
 * The widget expects an external @ref GravityEngine pointer via @ref setEngine.
 */
class NBodyWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_4_Core {
    Q_OBJECT

public:
    /**
     * @brief Construct the rendering widget.
     * @param parent Optional parent widget.
     */
    explicit NBodyWidget(QWidget *parent = nullptr);

    /**
     * @brief Destroy widget and associated graphics resources.
     */
    ~NBodyWidget() override;

    /**
     * @brief Enable or disable background starfield rendering.
     * @param draw true to render starfield; false to hide it.
     */
    void setDrawStarfield(bool draw) { m_drawStarfield = draw; update(); }

    /**
     * @brief Enable or disable body trail rendering.
     * @param draw true to render trails; false to hide them.
     */
    void setDrawTrails(bool draw) { m_drawTrails = draw; update(); }

    /**
     * @brief Enable or disable velocity-based body coloring.
     * @param use true to color by speed; false to use default coloring.
     */
    void setUseVelocityColor(bool use) { m_useVelocityColor = use; update(); }

    /**
     * @brief Enable or disable cinematic camera behavior.
     * @param mode true to enable cinematic mode; false otherwise.
     */
    void setCinematicMode(bool mode) { m_cinematicMode = mode; update(); }

    /**
     * @brief Assign the physics engine data source for rendering.
     * @param eng Pointer to active simulation engine (non-owning).
     *
     * @note The caller must ensure @p eng remains valid while used by this widget.
     */
    void setEngine(GravityEngine *eng) { engine = eng; }

    /**
     * @brief Select a body index for camera tracking.
     * @param index Body index in engine storage; use -1 for free/orbit camera.
     */
    void setTrackedPlanetIndex(int index) { m_trackedPlanetIndex = index; update(); }

protected:
    /**
     * @brief Initialize OpenGL context-dependent state/resources.
     *
     * Called once by Qt before first render/resize.
     */
    void initializeGL() override;

    /**
     * @brief Handle viewport resize and projection update.
     * @param w New viewport width in pixels.
     * @param h New viewport height in pixels.
     */
    void resizeGL(int w, int h) override;

    /**
     * @brief Render one frame of the simulation scene.
     *
     * Called by Qt whenever the widget is repainted.
     */
    void paintGL() override;

    /**
     * @brief Handle mouse button press (camera interaction start).
     * @param event Mouse event payload.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Handle mouse movement (camera orbit/manipulation).
     * @param event Mouse event payload.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Handle wheel scrolling (camera zoom).
     * @param event Wheel event payload.
     */
    void wheelEvent(QWheelEvent *event) override;

private:
    /**
     * @brief Physics engine used as data source for body state.
     */
    GravityEngine* engine = nullptr;

    /**
     * @brief Maximum number of bodies supported by current GPU-side buffers.
     */
    const int MAX_BODIES = 10000;

    /**
     * @brief Maximum trail length supported by rendering pipeline.
     */
    const int MAX_TRAIL_LENGTH = 1000;

    /**
     * @brief Vertex array object for body rendering.
     */
    QOpenGLVertexArrayObject VAO;

    /**
     * @brief Vertex buffer object for body rendering attributes.
     */
    QOpenGLBuffer VBO;

    /**
     * @brief Raw OpenGL buffer handle used for trail geometry streaming.
     */
    GLuint m_trailVBO = 0;

    /**
     * @brief Persistently/temporarily mapped trail buffer pointer (if mapping is active).
     */
    float *m_trailMappedPtr = nullptr;

    /**
     * @brief Shader program used for body rendering.
     */
    QOpenGLShaderProgram* shaderProgram;

    /**
     * @brief Projection matrix for camera lens transform.
     */
    QMatrix4x4 projectionMatrix;

    /**
     * @brief View matrix for camera transform.
     */
    QMatrix4x4 viewMatrix;

    /**
     * @brief Toggle for starfield rendering.
     */
    bool m_drawStarfield = false;

    /**
     * @brief Toggle for trail rendering.
     */
    bool m_drawTrails = false;

    /**
     * @brief Toggle for velocity-color mode.
     */
    bool m_useVelocityColor = true;

    /**
     * @brief Toggle for cinematic camera mode.
     */
    bool m_cinematicMode = false;

    /**
     * @brief Index of tracked body; -1 means no tracking.
     */
    int m_trackedPlanetIndex = -1;

    /**
     * @brief Shader program for starfield rendering.
     */
    QOpenGLShaderProgram* starShaderProgram;

    /**
     * @brief VAO for starfield geometry.
     */
    QOpenGLVertexArrayObject starVAO;

    /**
     * @brief VBO for starfield geometry.
     */
    QOpenGLBuffer starVBO;

    /**
     * @brief Number of star particles in generated background.
     */
    int numStars = 5000;

    /**
     * @brief Shader program for trail rendering.
     */
    QOpenGLShaderProgram* trailShaderProgram;

    /**
     * @brief VAO for trail rendering.
     */
    QOpenGLVertexArrayObject trailVAO;

    /**
     * @brief VBO wrapper for trail rendering.
     */
    QOpenGLBuffer trailVBO;

    /**
     * @brief Last mouse position (for drag delta computation).
     */
    QPoint lastMousePos;

    /**
     * @brief Current camera distance from target.
     */
    float cameraDistance = 50.0f;

    /**
     * @brief Current camera pitch angle.
     */
    float cameraPitch = 0.0f;

    /**
     * @brief Current camera yaw angle.
     */
    float cameraYaw = 0.0f;

    /**
     * @brief Current camera look-at target point.
     */
    QVector3D cameraTarget = QVector3D(0.0f, 0.0f, 0.0f);

    /**
     * @brief Smoothed/desired camera distance.
     */
    float targetDistance = 50.0f;

    /**
     * @brief Smoothed/desired camera pitch.
     */
    float targetPitch = 0.0f;

    /**
     * @brief Smoothed/desired camera yaw.
     */
    float targetYaw = 0.0f;

    /**
     * @brief Smoothed/desired camera target position.
     */
    QVector3D targetCameraTarget = QVector3D(0.0f, 0.0f, 0.0f);

    /**
     * @brief CPU-side staging buffer for body vertex attributes.
     */
    std::vector<GLfloat> m_gpuPositionBuffer;

signals:
    /**
     * @brief Emitted after a frame has been rendered.
     *
     * Useful for frame counters and FPS measurement in UI/controller layers.
     */
    void frameRendered();
};