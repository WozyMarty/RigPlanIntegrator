#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include "models/riggingplandata.h"
#include "models/riggingcalculation.h"
#include "models/formatpreset.h"

class CalculationEngine;

enum class AiIssueSeverity {
    Info = 0,
    Warning = 1,
    Critical = 2
};

struct AiValidationIssue {
    AiIssueSeverity severity = AiIssueSeverity::Info;
    QString category;     // "Consistência Carga / Memorial", "Operação do Guindaste", "Acessórios de Rigging", "Normas Regulamentadoras (NR-11/NR-12)"
    QString title;
    QString description;
    QString sourcePlan;   // Ex: "Plano: Carga Líquida = 3450 kg"
    QString sourceFormula;// Ex: "Fórmula 1: Pa = 3.45 t"
    bool canAutoSync = false;
    QString syncProposal;
};

struct AiVariableComparison {
    QString variableName;
    QString planValue;
    QString formulaValue;
    bool isConsistent = true;
    QString statusMessage;
};

struct AiValidationReport {
    int score = 100; // 0 a 100%
    QString statusTitle;
    QString statusBadgeColor; // "#3FB950" (green), "#D29922" (yellow), "#F85149" (red)
    QVector<AiValidationIssue> issues;
    QVector<AiVariableComparison> comparisons;
    int criticalCount = 0;
    int warningCount = 0;
    int infoCount = 0;

    bool isSafe() const { return criticalCount == 0 && warningCount == 0; }
};

/**
 * @brief Motor do Verificador de Cálculos e Regras Técnicas
 *
 * Realiza a conferência cruzada determinística entre as variáveis das fórmulas
 * do Memorial de Cálculo e as edições das páginas estilo "Plano de Rigging",
 * garantindo integridade matemática, física e normativa (NR-11, NR-12, ASME B30.5).
 */
class AiValidator : public QObject {
    Q_OBJECT

public:
    explicit AiValidator(QObject* parent = nullptr);

    /**
     * @brief Executa auditoria completa e gera o relatório técnico do verificador.
     */
    static AiValidationReport validate(
        const RiggingPlanData& planData,
        const QVector<RiggingCalculation*>& calculations,
        const FormatPreset& preset);

    /**
     * @brief Sincroniza automaticamente variáveis divergentes entre Plano de Rigging e Fórmulas.
     * @return true se alguma alteração foi sincronizada
     */
    static bool autoSyncVariables(
        RiggingPlanData& planData,
        CalculationEngine* engine,
        FormatPreset& preset);
};
