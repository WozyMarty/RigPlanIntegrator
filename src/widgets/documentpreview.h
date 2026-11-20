#pragma once

#include <QWidget>
#include <QTextBrowser>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include "core/documentgenerator.h"
#include "models/presetmanager.h"
#include "core/calculationengine.h"
#include "models/documentsection.h"

/**
 * @brief Main central workspace widget displaying live continuous document preview.
 *
 * Renders multiple continuous sections (Plano de Rigging, Memorial, custom styles,
 * CREA images, and SOLIDWORKS CAD attachments) with live refresh and order management.
 */
class DocumentPreview : public QWidget {
    Q_OBJECT

public:
    explicit DocumentPreview(QWidget* parent = nullptr);
    explicit DocumentPreview(CalculationEngine* engine,
                            PresetManager* presetManager,
                            QWidget* parent = nullptr);

    void setEngineAndPresetManager(CalculationEngine* engine, PresetManager* presetManager);
    void setDocumentName(const QString& name);
    QTextDocument* documentForExport() const;

    const QVector<DocumentSection>& sections() const { return m_sections; }
    void setSections(const QVector<DocumentSection>& sections);

    void updatePreview();

signals:
    void openPresetGalleryRequested();
    void aiAssistantRequested();

private slots:
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void onPresetSelected(const FormatPreset& preset);

    // Continuous Document & Attachment Actions
    void onImportCrea();
    void onDeleteCrea();
    void onImportSolidworks();
    void onDeleteSolidworks();
    void onAddStyleRequested();
    void onOpenPageOrder();
    void onOpenAiValidator();

private:
    void setupUi();
    void updateAttachmentButtonStates();
    void updateAiBadge();

    CalculationEngine* m_engine;
    PresetManager* m_presetManager;
    DocumentGenerator m_generator;

    QVector<DocumentSection> m_sections;
    QString m_documentName = "Plano_de_Rigging_001";
    int m_zoomPercent = 100;

    // Controls
    QLabel* m_presetBadge;
    QPushButton* m_changePresetBtn;
    QPushButton* m_addStyleBtn;
    QPushButton* m_editPlanBtn;
    QPushButton* m_selectCraneBtn;

    // Attachment buttons
    QPushButton* m_importCreaBtn;
    QPushButton* m_deleteCreaBtn;
    QPushButton* m_importSwBtn;
    QPushButton* m_deleteSwBtn;
    QPushButton* m_orderBtn;
    QPushButton* m_aiStatusBtn;

    // Zoom controls
    QPushButton* m_zoomOutBtn;
    QPushButton* m_zoomInBtn;
    QLabel* m_zoomLabel;
    QFrame* m_headerEditFrame;

    // Editable Header Controls
    QLineEdit* m_titleEdit;
    QLineEdit* m_subtitleEdit;
    QLineEdit* m_artEdit;
    QLineEdit* m_riggingEdit;

    QTextBrowser* m_textBrowser;
};
