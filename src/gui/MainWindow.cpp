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
#include <QTabWidget>
#include <QCheckBox>

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


    QTabWidget *tabs = new QTabWidget(this);
    mainLayout->addWidget(tabs, 1);

    // ================
    // TAB 1 - SIM
    // ================
    QWidget *simTab = new QWidget();
    QVBoxLayout *simLayout = new QVBoxLayout(simTab);

    QPushButton *startButton = new QPushButton("Start symulacji", this);
    simLayout->addWidget(startButton);

    simLayout->addWidget(new QLabel("Prędkość symulacji", this));
    QSlider *slider = new QSlider(Qt::Horizontal, this);
    simLayout->addWidget(slider);

    simLayout->addStretch();
    tabs->addTab(simTab, "Symulacja");

    // ================
    // TAB 2 - VISUALIZE
    // ================
    QWidget *visTab = new QWidget();
    QVBoxLayout *visLayout = new QVBoxLayout(visTab);

    QCheckBox *starfieldCheck = new QCheckBox("Pokaż gwiazdy w tle", this);
    QCheckBox *trailsCheck = new QCheckBox("Pokaż ślady planet", this);
    QCheckBox *speedColorCheck = new QCheckBox("Koloruj planety według prędkości", this);
    speedColorCheck->setChecked(true);

    QCheckBox *cinematicModeCheck = new QCheckBox("Tryb kinowy (wolna kamera)", this);

    visLayout->addWidget(starfieldCheck);
    visLayout->addWidget(trailsCheck);
    visLayout->addWidget(speedColorCheck);
    visLayout->addWidget(cinematicModeCheck);

    visLayout->addStretch();
    tabs->addTab(visTab, "Wizualizacja");

    // ================
    // TAB 3 - CAMERA
    // ================
    QWidget *camTab = new QWidget();
    QVBoxLayout *camLayout = new QVBoxLayout(camTab);

    camLayout->addWidget(new QLabel("Sledź obiekt", this));
    QComboBox *planetSelector = new QComboBox(this);
    planetSelector->addItem("Wolna Kamera (Środek)");

    // Loading planets to dropdown box
    const auto& bodies = engine.getBodies();
    for (size_t i = 0; i < bodies.size(); ++i) {
        QString planetName = QString("Obiekt %1 (Masa: %2)").arg(i).arg(bodies[i].mass);
        planetSelector->addItem(planetName);
    }
    camLayout->addWidget(planetSelector);

    camLayout->addStretch();
    tabs->addTab(camTab, "Kamera");

    // // Setting elements to be "at top"
    // panelLayout->addStretch();
    //
    // // Adding control to layout (weight 1)
    // mainLayout->addWidget(rightPanel, 1);

    glWidget->setEngine(&engine);

    // Timer - Main Sim Loop
    simTimer = new QTimer(this);
    connect(simTimer, &QTimer::timeout, this, [this]() {
        const float physics_dt = 0.002f; // Constant time step for physics

        float frameSimulationTime = 0.0016f * m_timeMultiplier;

        int stepsToSimulate = static_cast<int>(std::round(frameSimulationTime / physics_dt));

        if (stepsToSimulate > 1000) stepsToSimulate = 1000; // Upper blockade

        for (int i = 0; i < stepsToSimulate; ++i) {
            engine.update(physics_dt);
        }

        glWidget->update();
    });

    // Start/stop of simulation (I hate start stop in my vw golf)
    connect(startButton, &QPushButton::clicked, this, [this, startButton]() {
        if (simTimer->isActive()) {
            simTimer->stop();
            startButton->setText("Start symulacji");
        } else {
            simTimer->start(16); // ~60 FPS
            startButton->setText("Stop symulacji");
        }
    });

    // Time multiplier slider
    slider->setRange(1, 200);
    slider->setValue(100);
    connect(slider, &QSlider::valueChanged, this, [this](int value) {
        m_timeMultiplier = value / 100.0f;
    });

    // Checkboxes for visualization options
    connect(starfieldCheck, &QCheckBox::toggled, glWidget, &NBodyWidget::setDrawStarfield);
    connect(trailsCheck, &QCheckBox::toggled, glWidget, &NBodyWidget::setDrawTrails);
    connect(speedColorCheck, &QCheckBox::toggled, glWidget, &NBodyWidget::setUseVelocityColor);
    connect(cinematicModeCheck, &QCheckBox::toggled, glWidget, &NBodyWidget::setCinematicMode);

    // Planet selector
    connect(planetSelector, &QComboBox::currentIndexChanged, glWidget, [this](int index) {
        glWidget->setTrackedPlanetIndex(index - 1);
    });
}

MainWindow::~MainWindow() { }