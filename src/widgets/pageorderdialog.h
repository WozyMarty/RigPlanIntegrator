#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "models/documentsection.h"
#include "models/presetmanager.h"

/**
 * @brief Diálogo para ordenar, incluir e gerenciar as páginas e seções da Pré-Visualização.
 */
class PageOrderDialog : public QDialog {
    Q_OBJECT

public:
    explicit PageOrderDialog(const QVector<DocumentSection>& sections,
                             PresetManager* presetManager,
                             QWidget* parent = nullptr);

    QVector<DocumentSection> sections() const { return m_sections; }

signals:
    void sectionsChanged(const QVector<DocumentSection>& sections);
    void requestImportCrea();
    void requestImportSolidworks();

private slots:
    void moveUp();
    void moveDown();
    void addStylePreset();
    void removeSelected();
    void onImportCreaClicked();
    void onImportSolidworksClicked();
    void updateTable();

private:
    void setupUi();

    QVector<DocumentSection> m_sections;
    PresetManager* m_presetManager;

    QTableWidget* m_table;
    QPushButton* m_upBtn;
    QPushButton* m_downBtn;
    QPushButton* m_addStyleBtn;
    QPushButton* m_importCreaBtn;
    QPushButton* m_importSolidworksBtn;
    QPushButton* m_removeBtn;
    QLabel* m_summaryLabel;
};
