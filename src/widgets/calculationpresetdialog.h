#pragma once

#include <QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "core/calculationengine.h"

/**
 * @brief Dialog to select which calculations are active and manage calculation presets.
 */
class CalculationPresetDialog : public QDialog {
    Q_OBJECT

public:
    explicit CalculationPresetDialog(CalculationEngine* engine, QWidget* parent = nullptr);

    QVector<int> selectedCalculationIds() const;

private slots:
    void onPresetComboChanged(int index);
    void onSelectAllClicked();
    void onDeselectAllClicked();
    void onSaveNewPresetClicked();
    void onDeletePresetClicked();
    void onFilterChanged(const QString& text);
    void onApplyClicked();

private:
    void setupUi();
    void populatePresetsCombo();
    void populateChecklist();
    void updateCheckedFromIds(const QVector<int>& ids);

    CalculationEngine* m_engine;

    QComboBox* m_presetsCombo;
    QPushButton* m_deletePresetBtn;

    QLineEdit* m_searchEdit;
    QListWidget* m_calcListWidget;
    QLabel* m_countLabel;

    QPushButton* m_selectAllBtn;
    QPushButton* m_deselectAllBtn;

    QLineEdit* m_newPresetNameEdit;
    QPushButton* m_savePresetBtn;

    QPushButton* m_cancelBtn;
    QPushButton* m_applyBtn;
};
