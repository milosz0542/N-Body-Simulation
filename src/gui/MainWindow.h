//
// Created by milosz on 3/2/26.
//

#pragma once

#include <QMainWindow>
#include <QComboBox>
#include "NBodyWidget.h"
#include "core/GravityEngine.h"
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    void loadFromCSV(QString filename);
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    float m_timeMultiplier = 1.0f;
    QTimer *simTimer;
    int m_frameCount = 0;
    int m_tickCount = 0;
    QLabel *lblFps;
    QLabel *lblTps;
    NBodyWidget *glWidget;
    QComboBox *planetSelector;
    void updatePlanetSelector();
    GravityEngine engine;
};
