//
// Created by milosz on 3/2/26.
//

#pragma once

#include <QMainWindow>
#include "NBodyWidget.h"
#include "core/GravityEngine.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    void loadFromCSV(QString filename);
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    float m_timeMultiplier = 1.0f;
    QTimer *simTimer;
    NBodyWidget *glWidget;
    GravityEngine engine;
};
