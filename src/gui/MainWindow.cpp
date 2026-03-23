//
// Created by milosz on 3/2/26.
//

#include "MainWindow.h"
#include "NBodyWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QWidget>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Creating main, central widget for window
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Creating layout for central widget - horizontal LEFT/RIGHT
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Creating OpenGL widget
    glWidget = new NBodyWidget();

    // Adding OpenGL widget to layout (weight 4)
    mainLayout->addWidget(glWidget, 4);

    // Creating control
    QWidget *rightPanel = new QWidget(this);

    // Creating layout for control - vertical TOP/BOTTOM
    QVBoxLayout *panelLayout = new QVBoxLayout(rightPanel);

    // Placeholders
    panelLayout->addWidget(new QLabel("Panel Sterowania", this));

    QPushButton *startButton = new QPushButton("Start symulacji", this);
    panelLayout->addWidget(startButton);

    panelLayout->addWidget(new QLabel("Prędkość symulacji", this));
    QSlider *slider = new QSlider(Qt::Horizontal, this);
    panelLayout->addWidget(slider);

    // Setting elements to be "at top"
    panelLayout->addStretch();

    // Adding control to layout (weight 1)
    mainLayout->addWidget(rightPanel, 1);

    engine.addBody(CelestialBody(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 0.0f, 0.0f), 1000.0f));
    engine.addBody(CelestialBody(Eigen::Vector3f(10.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 10.0f, 0.0f), 1.0f));
    engine.addBody(CelestialBody(Eigen::Vector3f(0.0f, 20.0f, 0.0f), Eigen::Vector3f(-7.0f, 0.0f, 0.0f), 1.0f));
    engine.addBody(CelestialBody(Eigen::Vector3f(0.0f, 0.0f, 15.0f), Eigen::Vector3f(0.0f, 5.0f, 0.0f), 0.5f));
    engine.addBody(CelestialBody(Eigen::Vector3f(15.0f, 10.0f, 0.0f), Eigen::Vector3f(-2.0f, -2.0f, 1.0f), 0.01f));

    glWidget->setEngine(&engine);

    QTimer *simTimer = new QTimer(this);

    connect(simTimer, &QTimer::timeout, this, [this]() {
        engine.update(0.0016f);

        glWidget->update();
    });

    connect(startButton, &QPushButton::clicked, this, [simTimer, startButton]() {
        if (simTimer->isActive()) {
            simTimer->stop();
            startButton->setText("Start symulacji");
        } else {
            simTimer->start(16);
            startButton->setText("Pauza");
        }
    });
}

MainWindow::~MainWindow() { }