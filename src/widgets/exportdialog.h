#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include "models/documentsection.h"
#include "models/presetmanager.h"
#include "core/calculationengine.h"

/**
 * @brief Diálogo moderno de exportação contínua (DOCX / PDF).
 *
 * Já inicia com os estilos e imagens presentes na Pré-Visualização devidamente selecionados,
 * permitindo ao usuário marcar outros estilos adicionais, importar imagens (CREA / SolidWorks)
 * e escolher o destino final do arquivo.
 */
class ExportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExportDialog(const QVector<DocumentSection>& previewSections,
                          PresetManager* presetManager,
                          CalculationEngine* engine,
                          const QString& defaultDocName,
                          const QString& defaultFormat,
                          QWidget* parent = nullptr);

    QString exportedFilePath() const { return m_exportedFilePath; }
    bool wasExportSuccessful() const { return m_exportSuccess; }

private slots:
    void onBrowsePath();
    void onFormatChanged(const QString& format);
    void moveUp();
    void moveDown();
    void onImportCrea();
    void onImportSolidworks();
    void onImportCustomImage();
    void onExecuteExport();
    void updateTable();

private:
    void setupUi();
    void populateInitialSections();

    QVector<DocumentSection> m_previewSections;
    QVector<DocumentSection> m_exportItems;
    PresetManager* m_presetManager;
    CalculationEngine* m_engine;

    QString m_documentName;
    QString m_format;
    QString m_exportedFilePath;
    bool m_exportSuccess = false;

    // Controls
    QLineEdit* m_fileNameEdit;
    QComboBox* m_formatCombo;
    QLineEdit* m_pathEdit;
    QPushButton* m_browseBtn;
    QTableWidget* m_table;
    QLabel* m_summaryLabel;
    QPushButton* m_exportBtn;
};
