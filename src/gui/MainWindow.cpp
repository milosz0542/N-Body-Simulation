//
// Created by milosz on 3/2/26.
//

#include "MainWindow.h"
#include "NBodyWidget.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QWidget>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Engine init
    engine.addBody(CelestialBody(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 0.0f, 0.0f), 1000.0f));
    engine.addBody(CelestialBody(Eigen::Vector3f(10.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 10.0f, 0.0f), 1.0f));
    engine.addBody(CelestialBody(Eigen::Vector3f(0.0f, 20.0f, 0.0f), Eigen::Vector3f(-7.0f, 0.0f, 0.0f), 1.0f));
    engine.addBody(CelestialBody(Eigen::Vector3f(0.0f, 0.0f, 15.0f), Eigen::Vector3f(0.0f, 5.0f, 0.0f), 0.5f));
    engine.addBody(CelestialBody(Eigen::Vector3f(15.0f, 10.0f, 0.0f), Eigen::Vector3f(-2.0f, -2.0f, 1.0f), 0.01f));
    // engine.addBody(CelestialBody(Eigen::Vector3f(10.0f, 0.0f, 15.0f), Eigen::Vector3f(0.0f, 0.0f, 0.0f), 100.0f));

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

    panelLayout->addWidget(new QLabel("Tryb kamery", this));
    QComboBox *planetSelector = new QComboBox(this);
    planetSelector->addItem("Wolna Kamera (Środek)");

    const auto& bodies = engine.getBodies();
    for (size_t i = 0; i < bodies.size(); ++i) {
        QString planetName = QString("Obiekt %1 (Masa: %2)").arg(i).arg(bodies[i].mass);
        planetSelector->addItem(planetName);
    }

    panelLayout->addWidget(planetSelector);

    connect(planetSelector, &QComboBox::currentIndexChanged, this, [this](int index) {
        glWidget->setTrackedPlanetIndex(index - 1); // -1 because first item is "Free Camera"
    });

    // Setting elements to be "at top"
    panelLayout->addStretch();

    // Adding control to layout (weight 1)
    mainLayout->addWidget(rightPanel, 1);

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