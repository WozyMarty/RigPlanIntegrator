#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>
#include "calculationblock.h"
#include "core/calculationengine.h"

/**
 * @brief Right panel container holding the calculation blocks.
 *
 * Provides:
 * - Calculation preset switcher (e.g. "Operação Velocidade & Pressão")
 * - Manage / Select which calculations are active
 * - Add new custom calculations with custom formulas
 * - Live search filtering, bulk expand/collapse/clear
 * - Real-time progress tracking
 */
class CalculationPanel : public QWidget {
    Q_OBJECT

public:
    explicit CalculationPanel(QWidget* parent = nullptr);
    explicit CalculationPanel(CalculationEngine* engine, QWidget* parent = nullptr);

    void setEngine(CalculationEngine* engine);
    void refreshAllBlocks();
    void rebuildBlocks();

signals:
    void parameterValueChanged(int calcId, int paramIndex, double value);

private slots:
    void filterBlocks(const QString& query);
    void updateProgressCounter();
    void updatePresetCombo();
    void onPresetComboActivated(int index);
    void onSelectCalculationsClicked();
    void onNewCalculationClicked();
    void onExpandAllClicked();
    void onCollapseAllClicked();
    void onClearAllClicked();

private:
    void setupUi();

    CalculationEngine* m_engine;

    QLabel* m_titleLabel;
    QLabel* m_progressLabel;

    QComboBox* m_presetCombo;
    QPushButton* m_selectCalcsBtn;
    QPushButton* m_newCalcBtn;

    QLineEdit* m_searchEdit;
    QPushButton* m_expandAllBtn;
    QPushButton* m_collapseAllBtn;
    QPushButton* m_clearAllBtn;

    QScrollArea* m_scrollArea;
    QWidget* m_scrollContainer;
    QVBoxLayout* m_blocksLayout;

    QVector<CalculationBlock*> m_blocks;
};
