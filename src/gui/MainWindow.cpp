//
// Created by milosz on 3/2/26.
//

#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QWidget>

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
}

MainWindow::~MainWindow() { }