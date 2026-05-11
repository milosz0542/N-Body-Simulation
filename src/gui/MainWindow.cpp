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
#include <QFileDialog>
#include <QFormLayout>
#include <cmath>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <QGroupBox>

void MainWindow::loadFromCSV(QString filename) {
    std::ifstream file(filename.toStdString());
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename.toStdString() << std::endl;
        return;
    }

    std::string line;
    std::vector<CelestialBody> newBodies;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream ss(line);
        double x, y, z, vx, vy, vz, mass;

        if (ss >> x >> y >> z >> vx >> vy >> vz >> mass) {
            newBodies.emplace_back(Eigen::Vector3f(x, y, z), Eigen::Vector3f(vx, vy, vz), mass);
        } else {
            std::cerr << "Invalid line format: " << line << std::endl;
        }
    }

    engine.setBodies(newBodies);
    engine.saveInitialState();
    updatePlanetSelector();
    glWidget->update();
}

void MainWindow::updatePlanetSelector() {
    if (!planetSelector) return;
    
    planetSelector->blockSignals(true);
    planetSelector->clear();
    planetSelector->addItem("Wolna Kamera (Środek)");
    
    const auto& bodies = engine.getBodies();
    for (size_t i = 0; i < bodies.size(); ++i) {
        QString planetName = QString("Obiekt %1 (Masa: %2)").arg(i).arg(bodies[i].mass);
        planetSelector->addItem(planetName);
    }
    planetSelector->blockSignals(false);
    
    // Reset tracking if bodies changed
    glWidget->setTrackedPlanetIndex(-1);
}

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


    QTabWidget *tabs = new QTabWidget(this);
    mainLayout->addWidget(tabs, 1);

    // ================
    // TAB 1 - SIM
    // ================
    QWidget *simTab = new QWidget();
    QVBoxLayout *simLayout = new QVBoxLayout(simTab);

    QPushButton *startButton = new QPushButton("Start symulacji", this);
    simLayout->addWidget(startButton);

    QPushButton *loadButton = new QPushButton("Wczytaj z CSV", this);
    QPushButton *resetButton = new QPushButton("Resetuj", this);
    QPushButton *clearButton = new QPushButton("Wyczyść kosmos", this);

    simLayout->addWidget(loadButton);
    simLayout->addWidget(resetButton);
    simLayout->addWidget(clearButton);

    simLayout->addWidget(new QLabel("Prędkość symulacji", this));
    QSlider *slider = new QSlider(Qt::Horizontal, this);
    simLayout->addWidget(slider);

    QGroupBox *perfGroup = new QGroupBox("Wydajność (profilowanie)", this);
    QFormLayout *perfLayout = new QFormLayout(perfGroup);

    lblFps = new QLabel("---", this);
    lblTps = new QLabel("---", this);

    QFont monoFont("Courier New", 10, QFont::Bold);
    lblFps->setFont(monoFont);
    lblTps->setFont(monoFont);

    perfLayout->addRow("FPS:", lblFps);
    perfLayout->addRow("TPS:", lblTps);
    simLayout->addWidget(perfGroup);

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

    QSlider *trailSlider = new QSlider(Qt::Horizontal, this);
    trailSlider->setRange(1, 1000);
    trailSlider->setValue(100);

    visLayout->addWidget(new QLabel("Długość śladu:", this));
    visLayout->addWidget(trailSlider);

    visLayout->addWidget(new QLabel("Gładkość / Rozdzielczość śladu: ", this));
    QSlider *resolutionSlider = new QSlider(Qt::Horizontal, this);

    resolutionSlider->setRange(0, 50);
    resolutionSlider->setValue(5);
    visLayout->addWidget(resolutionSlider);

    QComboBox *algoSelector = new QComboBox(this);
    algoSelector->addItem("Verlet O(N^2) [OpenMP]");
    algoSelector->addItem("Barnes-Hut O(N log N)");

    visLayout->addWidget(new QLabel("Algorytm obliczania sił grawitacyjnych:", this));
    visLayout->addWidget(algoSelector);

    connect(algoSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index == 0) {
            engine.setForceAlgorithm(ForceAlgorithm::Naive);
        } else {
            engine.setForceAlgorithm(ForceAlgorithm::BarnesHut);
        }
    });

    QSlider *thetaSlider = new QSlider(Qt::Horizontal, this);
    thetaSlider->setRange(1, 20);
    thetaSlider->setValue(5);

    visLayout->addWidget(new QLabel("Precyzja Barnes-hut (parametr Theta):", this));
    visLayout->addWidget(thetaSlider);

    connect(thetaSlider, &QSlider::valueChanged, this, [this](int value) {
       engine.setTheta(value / 10.0f);
    });

    visLayout->addStretch();
    tabs->addTab(visTab, "Wizualizacja");

    // ================
    // TAB 3 - CAMERA
    // ================
    QWidget *camTab = new QWidget();
    QVBoxLayout *camLayout = new QVBoxLayout(camTab);

    camLayout->addWidget(new QLabel("Śledź obiekt", this));
    planetSelector = new QComboBox(this);
    updatePlanetSelector();
    camLayout->addWidget(planetSelector);

    QFormLayout *telemetryLayout = new QFormLayout();
    QLabel *lblMass = new QLabel("---");
    QLabel *lblVel = new QLabel("---");
    QLabel *lblPos = new QLabel("---");

    telemetryLayout->addRow("Masa:", lblMass);
    telemetryLayout->addRow("Prędkość:", lblVel);
    telemetryLayout->addRow("Pozycja:", lblPos);
    camLayout->addLayout(telemetryLayout);

    camLayout->addStretch();
    tabs->addTab(camTab, "Kamera");

    // // Setting elements to be "at top"
    // panelLayout->addStretch();
    //
    // // Adding control to layout (weight 1)
    // mainLayout->addWidget(rightPanel, 1);

    glWidget->setEngine(&engine);

    // FPS Counter
    connect(glWidget, &NBodyWidget::frameRendered, this, [this]() {
        m_frameCount++;
    });

    // Timer - Main Sim Loop
    simTimer = new QTimer(this);
    connect(simTimer, &QTimer::timeout, this, [this]() {
        const float physics_dt = 0.002f; // Constant time step for physics

        float frameSimulationTime = 0.0016f * m_timeMultiplier;

        int stepsToSimulate = static_cast<int>(std::round(frameSimulationTime / physics_dt));

        if (stepsToSimulate > 1000) stepsToSimulate = 1000; // Upper blockade

        for (int i = 0; i < stepsToSimulate; ++i) {
            engine.update(physics_dt);
            m_tickCount++;
        }

        glWidget->update();
    });

    // Stats Timer
    QTimer *statsTimer = new QTimer(this);
    connect(statsTimer, &QTimer::timeout, this, [this]() {
        lblFps->setText(QString::number(m_frameCount));
        lblTps->setText(QString::number(m_tickCount));

        if (m_frameCount < 30) {
            lblFps->setStyleSheet("color: red");
        } else {
            lblFps->setStyleSheet("color: green");
        }

        m_frameCount = 0;
        m_tickCount = 0;
    }); statsTimer->start(1000);

    // Telemetry Timer
    QTimer *telemetryTimer = new QTimer(this);
    connect(telemetryTimer, &QTimer::timeout, this, [this, lblMass, lblVel, lblPos]() {
        int idx = planetSelector->currentIndex();
        if (idx > 0 && idx <= engine.getBodies().size()) {
            const auto& b = engine.getBodies()[idx - 1];
            lblMass->setText(QString::number(b.mass, 'f', 3));
            lblVel->setText(QString::number(b.velocity.norm(), 'f', 2) + " j/s");
            lblPos->setText(QString("X:%1, Y:%2, Z:%3").arg(b.position.x(), 0, 'f', 1).arg(b.position.y(), 0, 'f', 1).arg(b.position.z(), 0, 'f', 1));
        } else {
            lblMass->setText("---"); lblVel->setText("---"); lblPos->setText("---");
        }
    }); telemetryTimer->start(100);

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

    // Load from CSV button
    connect(loadButton, &QPushButton::clicked, this, [this, startButton]() {
        QString filename = QFileDialog::getOpenFileName(this, "Wybierz Układ", "", "Pliki CSV (*.csv);;Wszystkie pliki (*)");

        // If filename is empty, then do nothing
        if (filename.isEmpty()) return;

        if (simTimer->isActive()) {
            simTimer->stop();
            startButton->setText("Start symulacji");
        }

        loadFromCSV(filename);
    });

    // Reset button
    connect(resetButton, &QPushButton::clicked, this, [this]() {
        engine.resetInitialState();
        updatePlanetSelector();
        glWidget->update();
    });

    // Clear button
    connect(clearButton, &QPushButton::clicked, this, [this]() {
        engine.setBodies({});
        updatePlanetSelector();
        glWidget->update();
    });

    // Trail slider
    connect(trailSlider, &QSlider::valueChanged, this, [this](int value) {
        engine.m_maxTrailLength = static_cast<size_t>(value);
    });

    connect(resolutionSlider, &QSlider::valueChanged, this, [this](int value) {
        float distanceThreshold = value / 10.0f;
        engine.setTrailResolution(distanceThreshold);
    });
}

MainWindow::~MainWindow() { }
