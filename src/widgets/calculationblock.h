#pragma once

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QVector>
#include "models/riggingcalculation.h"

/**
 * @brief UI Widget representing 1 of the 16 calculation blocks.
 *
 * Displays title, mathematical formula, parameter input fields,
 * real-time result, description editor, and visual status badges.
 */
class CalculationBlock : public QFrame {
    Q_OBJECT

public:
    explicit CalculationBlock(RiggingCalculation* model, QWidget* parent = nullptr);

    RiggingCalculation* model() const { return m_model; }

    void updateValuesFromModel();
    void setCollapsed(bool collapsed);
    bool isCollapsed() const { return m_isCollapsed; }

signals:
    void valueChanged(int calcId, int paramIndex, double value);
    void toggleCollapseRequested(int calcId);

private slots:
    void onParameterEdited(int index, const QString& text);
    void onModelResultChanged();
    void onModelStatusChanged(RiggingCalculation::Status status);
    void toggleCollapse();

private:
    void setupUi();
    void updateStatusIndicator();

    RiggingCalculation* m_model;
    bool m_isCollapsed = false;
    bool m_updatingFromModel = false;

    // UI Controls
    QWidget* m_headerWidget;
    QLabel* m_titleLabel;
    QLabel* m_formulaLabel;
    QLabel* m_statusBadge;
    QPushButton* m_collapseBtn;

    QWidget* m_contentWidget;
    QPlainTextEdit* m_descEdit;
    QVBoxLayout* m_paramsLayout;
    QVector<QLineEdit*> m_paramInputs;
    QLabel* m_resultLabel;
};
