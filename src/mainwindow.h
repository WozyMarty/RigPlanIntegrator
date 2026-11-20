#pragma once

#include <QMainWindow>
#include "core/calculationengine.h"
#include "models/presetmanager.h"
#include "widgets/headerbar.h"
#include "widgets/calculationpanel.h"
#include "widgets/documentpreview.h"
#include "widgets/presetgallery.h"
#include "widgets/preseteditor.h"

namespace Ui {
class MainWindow;
}

/**
 * @brief Main application window for RigPlan Integrator.
 *
 * Integrates HeaderBar, DocumentPreview, CalculationPanel,
 * and PresetGallery/Editor into a modern, dual-view UI via Qt Designer mainwindow.ui.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onExportRequested();
    void onTogglePresetGallery();
    void onToggleAiAssistant();
    void onEditPresetRequested(const FormatPreset& preset);
    void onCreateNewPresetRequested();
    void showMainView();
    void showPresetGallery();

private:
    void setupUi();
    void connectSignals();

    Ui::MainWindow* ui;

    CalculationEngine* m_calcEngine;
    PresetManager* m_presetManager;
};
