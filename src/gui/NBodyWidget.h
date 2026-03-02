//
// Created by milosz on 3/2/26.
//

#pragma once

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>

class NBodyWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT // Required by signals mechanism and Qt slots

public:
    explicit NBodyWidget(QWidget *parent = nullptr);
    ~NBodyWidget() override;

protected:
    // Called once at start. Loading shaders, and setting default state
    void initializeGL() override;

    // Called when window is resized
    void resizeGL(int w, int h) override;

    // Called at every frame. Rendering code goes here
    void paintGL() override;
};