#include <QApplication>
#include <QPushButton>
#include <QMainWindow>
#include <QWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QSlider>
#include <QDoubleSpinBox>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <glm/glm.hpp>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <vector>
#include <memory>
#include <random>
#include <cmath>
#include <omp.h>

#include "gui/NBodyWidget.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    NBodyWidget window;
    window.resize(800, 600);
    window.setWindowTitle("N-Body Simulation");
    window.show();

    return a.exec();
}