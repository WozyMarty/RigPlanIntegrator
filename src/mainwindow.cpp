#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/docxexporter.h"
#include "core/pdfexporter.h"
#include "utils/constants.h"
#include <QFileDialog>
#include <QMessageBox>
#include "widgets/exportdialog.h"
#include "widgets/aivalidatordialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    m_calcEngine = new CalculationEngine(this);
    m_calcEngine->initialize();

    m_presetManager = new PresetManager(this);
    m_presetManager->initialize();

    ui->setupUi(this);

    setupUi();
    connectSignals();

    setWindowTitle("RigPlan Integrator - Conversão & Integração de Planos de Rigging");
    resize(1280, 800);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupUi() {
    ui->mainSplitter->setStretchFactor(0, 3);
    ui->mainSplitter->setStretchFactor(1, 1);

    ui->docPreview->setEngineAndPresetManager(m_calcEngine, m_presetManager);
    ui->calcPanel->setEngine(m_calcEngine);
    ui->presetGallery->setManager(m_presetManager);
    ui->presetEditor->setManager(m_presetManager);

    ui->statusBar->setStyleSheet("background-color: #161B22; color: #8B949E;");
    ui->statusBar->showMessage("RigPlan Integrator v1.0.0 — Sistema Conectado e Pronto.");
}

void MainWindow::connectSignals() {
    // Header actions
    connect(ui->headerBar, &HeaderBar::documentNameChanged, ui->docPreview, &DocumentPreview::setDocumentName);
    connect(ui->headerBar, &HeaderBar::exportRequested, this, &MainWindow::onExportRequested);
    connect(ui->headerBar, &HeaderBar::togglePresetGalleryRequested, this, &MainWindow::onTogglePresetGallery);
    connect(ui->headerBar, &HeaderBar::toggleAiAssistantRequested, this, &MainWindow::onToggleAiAssistant);

    // Calculation panel changes propagate to engine and preview
    connect(ui->calcPanel, &CalculationPanel::parameterValueChanged, m_calcEngine, &CalculationEngine::setParameterValue);

    // Gallery & Editor navigation
    connect(ui->docPreview, &DocumentPreview::openPresetGalleryRequested, this, &MainWindow::showPresetGallery);
    connect(ui->presetGallery, &PresetGallery::backToMainRequested, this, &MainWindow::showMainView);
    connect(ui->presetGallery, &PresetGallery::editPresetRequested, this, &MainWindow::onEditPresetRequested);
    connect(ui->presetGallery, &PresetGallery::createNewPresetRequested, this, &MainWindow::onCreateNewPresetRequested);

    connect(ui->presetEditor, &PresetEditor::backToGalleryRequested, this, &MainWindow::showPresetGallery);
    connect(ui->presetEditor, &PresetEditor::presetSaved, this, [this](const FormatPreset&) {
        ui->presetGallery->refreshGallery();
        ui->docPreview->updatePreview();
    });
}

void MainWindow::showMainView() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::showPresetGallery() {
    ui->presetGallery->refreshGallery();
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::onTogglePresetGallery() {
    if (ui->stackedWidget->currentIndex() == 0) {
        showPresetGallery();
    } else {
        showMainView();
    }
}

void MainWindow::onEditPresetRequested(const FormatPreset& preset) {
    ui->presetEditor->loadPreset(preset, false);
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::onCreateNewPresetRequested() {
    FormatPreset newP = FormatPreset::createDefault();
    newP.setName("Novo Estilo Customizado");
    ui->presetEditor->loadPreset(newP, true);
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::onExportRequested() {
    QString docName = ui->headerBar->documentName();
    if (docName.isEmpty()) docName = "Plano_de_Rigging";

    QString format = ui->headerBar->exportFormat();

    ExportDialog dlg(
        ui->docPreview->sections(),
        m_presetManager,
        m_calcEngine,
        docName,
        format,
        this
    );

    if (dlg.exec() == QDialog::Accepted) {
        if (dlg.wasExportSuccessful()) {
            ui->statusBar->showMessage(QString("Exportado com sucesso: %1").arg(dlg.exportedFilePath()), 6000);
        }
    }
}

void MainWindow::onToggleAiAssistant() {
    AiValidatorDialog dlg(m_calcEngine, m_presetManager, this);
    connect(&dlg, &AiValidatorDialog::dataSynced, this, [this]() {
        ui->docPreview->updatePreview();
        ui->statusBar->showMessage("Variáveis sincronizadas com sucesso pelo Verificador.", 4000);
    });
    dlg.exec();
}
