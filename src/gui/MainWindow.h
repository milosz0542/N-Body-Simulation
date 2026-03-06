//
// Created by milosz on 3/2/26.
//

#pragma once

#include <QMainWindow>
#include "NBodyWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    NBodyWidget *glWidget;
};
