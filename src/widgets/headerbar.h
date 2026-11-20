#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

/**
 * @brief Top header bar (Red theme as requested)
 *
 * Contains document title/renaming, export format selector (DOCX/PDF),
 * export action button, and tab/view navigation buttons.
 */
class HeaderBar : public QWidget {
    Q_OBJECT

public:
    explicit HeaderBar(QWidget* parent = nullptr);

    QString documentName() const;
    void setDocumentName(const QString& name);

    QString exportFormat() const;
    void setExportFormat(const QString& format);

signals:
    void documentNameChanged(const QString& name);
    void exportFormatChanged(const QString& format);
    void exportRequested();
    void togglePresetGalleryRequested();
    void toggleAiAssistantRequested();

private:
    void setupUi();

    QLineEdit* m_fileNameEdit;
    QComboBox* m_formatCombo;
    QPushButton* m_exportButton;
    QPushButton* m_presetsButton;
    QPushButton* m_aiButton;
};
