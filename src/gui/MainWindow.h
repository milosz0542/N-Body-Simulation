/**
 * @file MainWindow.h
 * @brief Declaration of the main GUI window for the N-body simulation application.
 *
 * This file defines the @ref MainWindow class, which owns the primary UI shell,
 * simulation controls, and rendering widget integration.
 */
#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QTimer>

#include "NBodyWidget.h"
#include "core/GravityEngine.h"

/**
 * @class MainWindow
 * @brief Top-level application window for simulation control and visualization.
 *
 * MainWindow manages:
 * - Simulation timing and update loop integration
 * - UI labels for runtime metrics (FPS/TPS)
 * - Planet/body selection UI controls
 * - The OpenGL simulation view via @ref NBodyWidget
 * - A local @ref GravityEngine instance used by the GUI workflow
 *
 * The class derives from QMainWindow and uses Qt's object system.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Load simulation bodies from a CSV file into the engine/UI context.
     * @param filename Path to the input CSV file.
     *
     * Expected parsing/validation behavior is implementation-defined in MainWindow.cpp.
     */
    void loadFromCSV(QString filename);

    /**
     * @brief Construct the main window.
     * @param parent Optional Qt parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroy the main window and owned UI resources.
     */
    ~MainWindow() override;

private:
    /**
     * @brief Scalar applied to simulation time step progression.
     */
    float m_timeMultiplier = 1.0f;

    /**
     * @brief Timer driving periodic simulation ticks/updates.
     */
    QTimer *simTimer;

    /**
     * @brief Frame counter used for FPS statistics.
     */
    int m_frameCount = 0;

    /**
     * @brief Tick/update counter used for TPS statistics.
     */
    int m_tickCount = 0;

    /**
     * @brief Label displaying current frames-per-second metric.
     */
    QLabel *lblFps;

    /**
     * @brief Label displaying current ticks-per-second metric.
     */
    QLabel *lblTps;

    /**
     * @brief OpenGL widget responsible for rendering the N-body scene.
     */
    NBodyWidget *glWidget;

    /**
     * @brief Combo box used to select a body/planet in the UI.
     */
    QComboBox *planetSelector;

    /**
     * @brief Refresh the planet selector entries to match current engine state.
     */
    void updatePlanetSelector();

    /**
     * @brief Physics engine instance used by this window.
     */
    GravityEngine engine;
};