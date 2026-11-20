#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QProgressBar>
#include <QTextEdit>
#include "utils/aivalidator.h"
#include "core/calculationengine.h"
#include "models/presetmanager.h"

/**
 * @brief Diálogo visual do Verificador de Cálculos e Regras Técnicas.
 *
 * Exibe auditoria cruzada em tempo real entre as variáveis das fórmulas
 * e as edições das páginas do Plano de Rigging, com suporte a sincronização automática.
 */
class AiValidatorDialog : public QDialog {
    Q_OBJECT

public:
    explicit AiValidatorDialog(CalculationEngine* engine,
                               PresetManager* presetManager,
                               QWidget* parent = nullptr);

    bool hasAutoSynced() const { return m_hasSynced; }

signals:
    void dataSynced();

private slots:
    void refreshValidation();
    void onAutoSyncClicked();
    void onCopyReportClicked();

private:
    void setupUi();
    void populateReport(const AiValidationReport& report);

    CalculationEngine* m_engine;
    PresetManager* m_presetManager;
    bool m_hasSynced = false;

    // UI elements
    QLabel* m_statusBadge;
    QLabel* m_scoreLabel;
    QProgressBar* m_scoreBar;
    QLabel* m_summaryStats;

    QTabWidget* m_tabWidget;
    QTableWidget* m_comparisonTable;
    QTableWidget* m_issuesTable;
    QTextEdit* m_technicalReportEdit;

    QPushButton* m_syncBtn;
    QPushButton* m_copyBtn;
};
