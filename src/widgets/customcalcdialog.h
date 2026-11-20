#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QScrollArea>
#include "models/riggingcalculation.h"

struct ParamRowWidget {
    QWidget* container;
    QLineEdit* symbolEdit;
    QLineEdit* nameEdit;
    QLineEdit* unitEdit;
    QDoubleSpinBox* defaultSpin;
    QPushButton* removeBtn;
};

/**
 * @brief Modal dialog to create or edit a custom calculation.
 *
 * Provides:
 * - Title input (e.g. "Velocidade Média")
 * - Full formula input (e.g. "Vm = ΔS / Δt") with math symbol shortcuts
 * - Dynamic parameter definition rows (Symbol, Meaning, Unit, Default Value)
 * - Result unit input
 * - Live test evaluator to verify formula computation before saving
 */
class CustomCalcDialog : public QDialog {
    Q_OBJECT

public:
    explicit CustomCalcDialog(QWidget* parent = nullptr);

    QString title() const;
    QString formula() const;
    QString resultUnit() const;
    QString description() const;
    QVector<CalcParameter> parameters() const;

private slots:
    void onFormulaChanged(const QString& text);
    void detectSymbolsFromFormula();
    void addParameterRow(const QString& symbol = "", const QString& name = "",
                         const QString& unit = "", double defaultValue = 0.0);
    void removeParameterRow(int index);
    void insertSymbolIntoFormula(const QString& symbol);
    void updateLiveTest();
    void onSaveClicked();

private:
    void setupUi();

    QLineEdit* m_titleEdit;
    QLineEdit* m_formulaEdit;
    QLineEdit* m_resultUnitEdit;
    QLineEdit* m_descEdit;

    QWidget* m_paramsContainer;
    QVBoxLayout* m_paramsLayout;
    QVector<ParamRowWidget> m_paramRows;

    // Test area
    QWidget* m_testContainer;
    QVBoxLayout* m_testInputsLayout;
    QMap<QString, QDoubleSpinBox*> m_testSpinBoxes;
    QLabel* m_testResultLabel;
    QLabel* m_statusMessageLabel;

    QPushButton* m_detectBtn;
    QPushButton* m_addParamBtn;
    QPushButton* m_saveBtn;
    QPushButton* m_cancelBtn;
};
