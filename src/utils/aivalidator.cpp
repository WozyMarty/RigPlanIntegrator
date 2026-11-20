#include "aivalidator.h"
#include "core/calculationengine.h"
#include <cmath>
#include <QMap>

AiValidator::AiValidator(QObject* parent)
    : QObject(parent)
{
}

AiValidationReport AiValidator::validate(
    const RiggingPlanData& planData,
    const QVector<RiggingCalculation*>& calculations,
    const FormatPreset& preset)
{
    AiValidationReport report;
    report.score = 100;

    // Index calculations by ID
    QMap<int, const RiggingCalculation*> calcMap;
    for (const auto* c : calculations) {
        if (c) calcMap[c->id()] = c;
    }

    auto getCalc = [&](int id) -> const RiggingCalculation* {
        return calcMap.value(id, nullptr);
    };

    const auto* c1 = getCalc(1); // Cinta 3m
    const auto* c2 = getCalc(2); // Cinta 4m
    const auto* c3 = getCalc(3); // Manilha carga
    const auto* c4 = getCalc(4); // Manilha balancim
    const auto* c5 = getCalc(5); // Sapatas
    const auto* c6 = getCalc(6); // RMET
    const auto* c7 = getCalc(7); // Capacidade guindaste
    const RiggingCalculation* windCalc = nullptr;
    for (const auto* c : calculations) {
        if (c && c->name().contains("vento", Qt::CaseInsensitive)) {
            windCalc = c;
            break;
        }
    }

    // =========================================================================
    // 1. COMPARISON: Carga Líquida / Equipamento (Plano vs Cálculo 1)
    // =========================================================================
    double planCargaKg = planData.cargaPeso;
    double calc1PaTon = c1 ? c1->parameterValue(1) : (planCargaKg / 1000.0);
    double calc1PaKg = calc1PaTon * 1000.0;
    bool cargaMatch = std::abs(planCargaKg - calc1PaKg) < 1.0;

    AiVariableComparison compCarga;
    compCarga.variableName = "Carga Líquida da Peça (Chiller)";
    compCarga.planValue = QString("%1 kg").arg(QString::number(planCargaKg, 'f', 0));
    compCarga.formulaValue = QString("Pa = %1 t (%2 kg)")
                                 .arg(QString::number(calc1PaTon, 'f', 2).replace('.', ','))
                                 .arg(QString::number(calc1PaKg, 'f', 0));
    compCarga.isConsistent = cargaMatch;
    compCarga.statusMessage = cargaMatch
        ? "Coerência total entre o Plano de Rigging e a fórmula da lingada inferior."
        : "Divergência detectada! O peso do equipamento diverge da variável da fórmula.";
    report.comparisons.append(compCarga);

    if (!cargaMatch) {
        AiValidationIssue issue;
        issue.severity = AiIssueSeverity::Critical;
        issue.category = "Consistência Carga / Memorial";
        issue.title = "Divergência de Carga Líquida entre Plano e Fórmula 1";
        issue.description = QString(
            "O campo editável 'Peso (kg)' na tabela de Dados da Carga do Plano de Rigging está com %1 kg, "
            "mas a variável 'Pa' no Cálculo 1 (Cinta 3m) está com %2 t (%3 kg). "
            "Essa divergência pode levar ao cálculo incorreto da tensão na lingada."
        ).arg(planCargaKg).arg(calc1PaTon).arg(calc1PaKg);
        issue.sourcePlan = QString("Plano de Rigging: %1 kg").arg(planCargaKg);
        issue.sourceFormula = QString("Cálculo 1 (Pa): %1 t").arg(calc1PaTon);
        issue.canAutoSync = true;
        issue.syncProposal = QString("Harmonizar Pa para %1 t (%2 kg)").arg(planCargaKg / 1000.0, 0, 'f', 2).arg(planCargaKg);
        report.issues.append(issue);
        report.criticalCount++;
        report.score -= 25;
    }

    // =========================================================================
    // 2. COMPARISON: Balancim / Dispositivos (Plano vs Cálculo 2)
    // =========================================================================
    double planBalancimKg = planData.pesoTotalBalancim();
    double calc2PaTotalTon = c2 ? c2->parameterValue(1) : 3.70;
    double calc2PaTotalKg = calc2PaTotalTon * 1000.0;
    double expectedPaTotalKg = planCargaKg + planBalancimKg;
    bool balancimMatch = std::abs(expectedPaTotalKg - calc2PaTotalKg) < 10.0;

    AiVariableComparison compBalancim;
    compBalancim.variableName = "Dispositivo / Balancim + Carga";
    compBalancim.planValue = QString("Balancim: %1 kg | Total: %2 kg")
                                 .arg(QString::number(planBalancimKg, 'f', 0))
                                 .arg(QString::number(expectedPaTotalKg, 'f', 0));
    compBalancim.formulaValue = QString("Pa+Pb = %1 t (%2 kg)")
                                    .arg(QString::number(calc2PaTotalTon, 'f', 2).replace('.', ','))
                                    .arg(QString::number(calc2PaTotalKg, 'f', 0));
    compBalancim.isConsistent = balancimMatch;
    compBalancim.statusMessage = balancimMatch
        ? "Carga acumulada (peça + balancim) consistente com a fórmula da lingada superior."
        : "A carga combinada do balancim diverge do valor considerado no Cálculo 2.";
    report.comparisons.append(compBalancim);

    if (!balancimMatch) {
        AiValidationIssue issue;
        issue.severity = AiIssueSeverity::Warning;
        issue.category = "Consistência Carga / Memorial";
        issue.title = "Divergência na Carga Total da Lingada Superior (Cálculo 2)";
        issue.description = QString(
            "No Plano de Rigging a soma Peça (%1 kg) + Balancim (%2 kg) resulta em %3 kg, "
            "enquanto o Cálculo 2 utiliza %4 kg (%5 t)."
        ).arg(planCargaKg).arg(planBalancimKg).arg(expectedPaTotalKg).arg(calc2PaTotalKg).arg(calc2PaTotalTon);
        issue.sourcePlan = QString("Soma no Plano: %1 kg").arg(expectedPaTotalKg);
        issue.sourceFormula = QString("Cálculo 2 (Pa+Pb): %1 t").arg(calc2PaTotalTon);
        issue.canAutoSync = true;
        issue.syncProposal = QString("Ajustar Cálculo 2 para %1 t").arg(expectedPaTotalKg / 1000.0, 0, 'f', 2);
        report.issues.append(issue);
        report.warningCount++;
        report.score -= 15;
    }

    // =========================================================================
    // 3. COMPARISON: Carga Bruta vs Esforço Sapatas (Pc no Cálculo 5)
    // =========================================================================
    double planCargaBrutaKg = planData.cargaBruta();
    double calc5PcKg = c5 ? c5->parameterValue(3) : planCargaBrutaKg;
    bool cargaBrutaMatch = std::abs(planCargaBrutaKg - calc5PcKg) < 60.0;

    AiVariableComparison compCargaBruta;
    compCargaBruta.variableName = "Carga Bruta Total Içada";
    compCargaBruta.planValue = QString("%1 kg").arg(QString::number(planCargaBrutaKg, 'f', 0));
    compCargaBruta.formulaValue = QString("Pc = %1 kgf").arg(QString::number(calc5PcKg, 'f', 0));
    compCargaBruta.isConsistent = cargaBrutaMatch;
    compCargaBruta.statusMessage = cargaBrutaMatch
        ? "Carga bruta total do içamento compatível com o cálculo de momento e patolas."
        : "Carga bruta difere do valor Pc aplicado no cálculo de esforço nas sapatas.";
    report.comparisons.append(compCargaBruta);

    if (!cargaBrutaMatch) {
        AiValidationIssue issue;
        issue.severity = AiIssueSeverity::Warning;
        issue.category = "Operação do Guindaste";
        issue.title = "Carga Bruta Diverge do Parâmetro Pc (Sapatas)";
        issue.description = QString(
            "A composição da carga bruta no Plano de Rigging é de %1 kg (Peça + Lingadas + Balancim + Extras), "
            "enquanto a variável 'Pc' no cálculo de sapatas está com %2 kgf."
        ).arg(planCargaBrutaKg, 0, 'f', 0).arg(calc5PcKg, 0, 'f', 0);
        issue.sourcePlan = QString("Carga Bruta: %1 kg").arg(planCargaBrutaKg, 0, 'f', 0);
        issue.sourceFormula = QString("Cálculo 5 (Pc): %1 kgf").arg(calc5PcKg, 0, 'f', 0);
        issue.canAutoSync = true;
        issue.syncProposal = QString("Sincronizar Pc para %1 kgf").arg(std::round(planCargaBrutaKg));
        report.issues.append(issue);
        report.warningCount++;
        report.score -= 10;
    }

    // =========================================================================
    // 4. COMPARISON: Raio de Operação (Plano vs Cálculo 5 & 7)
    // =========================================================================
    double planRaio = planData.raioOperacao;
    double calc5Ro = c5 ? c5->parameterValue(2) : planRaio;
    double calc7Raio = c7 ? c7->parameterValue(0) : planRaio;
    bool raioMatch = (std::abs(planRaio - calc5Ro) < 0.05) && (std::abs(planRaio - calc7Raio) < 0.05);

    AiVariableComparison compRaio;
    compRaio.variableName = "Raio de Operação";
    compRaio.planValue = QString("%1 m").arg(QString::number(planRaio, 'f', 1).replace('.', ','));
    compRaio.formulaValue = QString("Ro = %1 m | C7 = %2 m")
                                .arg(QString::number(calc5Ro, 'f', 1).replace('.', ','))
                                .arg(QString::number(calc7Raio, 'f', 1).replace('.', ','));
    compRaio.isConsistent = raioMatch;
    compRaio.statusMessage = raioMatch
        ? "Raio de operação perfeitamente uniforme entre o Plano, cálculo de sapatas e capacidade."
        : "O raio de operação diverge entre as seções do projeto.";
    report.comparisons.append(compRaio);

    if (!raioMatch) {
        AiValidationIssue issue;
        issue.severity = AiIssueSeverity::Critical;
        issue.category = "Operação do Guindaste";
        issue.title = "Divergência de Raio de Operação";
        issue.description = QString(
            "O raio de operação no Plano é %1 m, mas o cálculo de sapatas utiliza %2 m "
            "e a capacidade utiliza %3 m."
        ).arg(planRaio).arg(calc5Ro).arg(calc7Raio);
        issue.sourcePlan = QString("Plano: %1 m").arg(planRaio);
        issue.sourceFormula = QString("Fórmulas: %1 m / %2 m").arg(calc5Ro).arg(calc7Raio);
        issue.canAutoSync = true;
        issue.syncProposal = QString("Padronizar raio em %1 m").arg(planRaio);
        report.issues.append(issue);
        report.criticalCount++;
        report.score -= 20;
    }

    // =========================================================================
    // 5. TAXA DE UTILIZAÇÃO DO GUINDASTE
    // =========================================================================
    double capCargaRaioTon = planData.capCargaNoRaio;
    if (c7 && c7->isValid() && c7->result() > 0) {
        // Se c7 calculou, usa o valor com maior precisão
        capCargaRaioTon = c7->result() / 1000.0;
    }
    double tuGuindaste = (capCargaRaioTon > 0) ? (planData.cargaBrutaTon() / capCargaRaioTon) * 100.0 : 0.0;

    AiVariableComparison compTuGuindaste;
    compTuGuindaste.variableName = "Taxa de Utilização do Guindaste (TU)";
    compTuGuindaste.planValue = QString("%1 % (Capacidade: %2 t)")
                                    .arg(QString::number(tuGuindaste, 'f', 1).replace('.', ','))
                                    .arg(QString::number(capCargaRaioTon, 'f', 2).replace('.', ','));
    compTuGuindaste.formulaValue = (tuGuindaste <= 80.0) ? "Limite Seguro: <= 80%" : "Alerta: > 80%";
    compTuGuindaste.isConsistent = (tuGuindaste <= 85.0);
    compTuGuindaste.statusMessage = (tuGuindaste <= 80.0)
        ? "Dentro da margem de segurança operacional recomendada."
        : (tuGuindaste <= 100.0 ? "Atenção: Operação acima de 80% da capacidade tabelada." : "CRÍTICO: SOBRECARGA!");
    report.comparisons.append(compTuGuindaste);

    if (tuGuindaste > 100.0) {
        AiValidationIssue issue;
        issue.severity = AiIssueSeverity::Critical;
        issue.category = "Normas Regulamentadoras (NR-11/NR-12)";
        issue.title = "SOBRECARGA DO GUINDASTE (> 100%)";
        issue.description = QString(
            "A carga bruta a ser içada (%1 t) excede a capacidade de carga do guindaste no raio (%2 t). "
            "Taxa de utilização calculada em %3%. Operação estritamente proibida por risco de tombamento iminente."
        ).arg(planData.cargaBrutaTon(), 0, 'f', 2).arg(capCargaRaioTon, 0, 'f', 2).arg(tuGuindaste, 0, 'f', 1);
        issue.sourcePlan = QString("Carga: %1 t").arg(planData.cargaBrutaTon(), 0, 'f', 2);
        issue.sourceFormula = QString("Capacidade: %1 t").arg(capCargaRaioTon, 0, 'f', 2);
        issue.canAutoSync = false;
        report.issues.append(issue);
        report.criticalCount++;
        report.score -= 50;
    } else if (tuGuindaste > 80.0) {
        AiValidationIssue issue;
        issue.severity = AiIssueSeverity::Warning;
        issue.category = "Normas Regulamentadoras (NR-11/NR-12)";
        issue.title = "Taxa de Utilização Elevada do Guindaste";
        issue.description = QString(
            "A taxa de utilização está em %1% (acima da margem preventiva de 80% recomendada pela boa prática de Rigging). "
            "Recomenda-se acompanhamento presencial do Rigger e redução da velocidade de giro."
        ).arg(tuGuindaste, 0, 'f', 1);
        issue.sourcePlan = QString("TU: %1%").arg(tuGuindaste, 0, 'f', 1);
        issue.sourceFormula = "Margem recomendada: <= 80%";
        issue.canAutoSync = false;
        report.issues.append(issue);
        report.warningCount++;
        report.score -= 10;
    }

    // =========================================================================
    // 6. TAXA DE UTILIZAÇÃO DAS CINTAS E MANILHAS (NR-11 / NBR 15637 / NBR 13541)
    // =========================================================================
    auto checkTu = [&](const RiggingCalculation* calc, const QString& name, double maxTu) {
        if (!calc || !calc->isValid()) return;
        double result = calc->result();
        double tu = 0.0;
        if (calc->id() == 1 || calc->id() == 2) {
            double cap = calc->parameterValue(2);
            if (cap > 0) tu = (result / cap) * 100.0;
        } else if (calc->id() == 3 || calc->id() == 4) {
            double cap = calc->parameterValue(1);
            double te = calc->parameterValue(0);
            if (cap > 0) tu = (te / cap) * 100.0;
        }

        if (tu > 100.0) {
            AiValidationIssue issue;
            issue.severity = AiIssueSeverity::Critical;
            issue.category = "Acessórios de Rigging";
            issue.title = QString("Sobrecarga em Acessório: %1").arg(name);
            issue.description = QString("A taxa de utilização calculada é de %1% (limite 100%). Risco de rompimento do acessório.")
                                    .arg(tu, 0, 'f', 1);
            issue.sourceFormula = QString("%1: TU = %2%").arg(name).arg(tu, 0, 'f', 1);
            report.issues.append(issue);
            report.criticalCount++;
            report.score -= 30;
        } else if (tu > maxTu) {
            AiValidationIssue issue;
            issue.severity = AiIssueSeverity::Warning;
            issue.category = "Acessórios de Rigging";
            issue.title = QString("Taxa de Utilização Elevada: %1").arg(name);
            issue.description = QString("A taxa de utilização está em %1%, superior ao critério de projeto de %2%.")
                                    .arg(tu, 0, 'f', 1).arg(maxTu, 0, 'f', 0);
            issue.sourceFormula = QString("%1: TU = %2%").arg(name).arg(tu, 0, 'f', 1);
            report.issues.append(issue);
            report.warningCount++;
            report.score -= 10;
        }
    };

    checkTu(c1, "Cinta tubular 3m x 6t", 85.0);
    checkTu(c2, "Cinta tubular 4m x 6t", 85.0);
    checkTu(c3, "Manilhas (carga x balancim)", 85.0);
    checkTu(c4, "Manilhas (balancim x gancho)", 85.0);

    // =========================================================================
    // 7. ÂNGULOS DE LINGADA (Cálculos 1 e 2)
    // =========================================================================
    if (c1) {
        double alpha1 = c1->parameterValue(0);
        if (alpha1 > 60.0) {
            AiValidationIssue issue;
            issue.severity = AiIssueSeverity::Warning;
            issue.category = "Acessórios de Rigging";
            issue.title = "Ângulo de Lingada Excessivo (Cálculo 1)";
            issue.description = QString("O ângulo α está em %1°. Ângulos acima de 60° aumentam drasticamente a tração nos cabos/cintas.")
                                    .arg(alpha1, 0, 'f', 1);
            report.issues.append(issue);
            report.warningCount++;
            report.score -= 5;
        }
    }

    // =========================================================================
    // 8. RESISTÊNCIA DO TERRENO (RMET - Cálculo 6)
    // =========================================================================
    if (c6 && c6->isValid()) {
        double rmet = c6->result();
        if (rmet > 3.5) {
            AiValidationIssue issue;
            issue.severity = AiIssueSeverity::Warning;
            issue.category = "Operação do Guindaste";
            issue.title = "Alta Resistência Exigida do Solo (RMET)";
            issue.description = QString(
                "A pressão nas sapatas exige RMET de %1 kgf/cm². "
                "Para terrenos comuns ou asfalto, recomenda-se aumentar a área dos dormentes (AP) "
                "para diminuir a pressão exercida e evitar afundamento."
            ).arg(rmet, 0, 'f', 2);
            issue.sourceFormula = QString("RMET = %1 kgf/cm²").arg(rmet, 0, 'f', 2);
            report.issues.append(issue);
            report.warningCount++;
            report.score -= 5;
        }
    }

    // =========================================================================
    // 9. VELOCIDADE DO VENTO
    // =========================================================================
    if (windCalc && windCalc->isValid()) {
        double wind = windCalc->result();
        if (wind > 12.8) {
            AiValidationIssue issue;
            issue.severity = AiIssueSeverity::Warning;
            issue.category = "Normas Regulamentadoras (NR-11/NR-12)";
            issue.title = "Velocidade do Vento Acima do Limite Seguro";
            issue.description = QString("Vento de projeto em %1 m/s (acima de 12.8 m/s / 46 km/h). Risco de desestabilização da carga.")
                                    .arg(wind, 0, 'f', 1);
            report.issues.append(issue);
            report.warningCount++;
            report.score -= 10;
        }
    }

    // =========================================================================
    // 10. VERIFICAÇÃO ARITMÉTICA DAS TABELAS DE LINGADA E BALANCIM
    // =========================================================================
    for (const auto& item : planData.lingadaItems) {
        double expectedTotal = item.qtd * item.unitario;
        if (std::abs(item.total() - expectedTotal) > 0.05) {
            AiValidationIssue issue;
            issue.severity = AiIssueSeverity::Critical;
            issue.category = "Acessórios de Rigging";
            issue.title = QString("Erro Aritmético na Tabela de Lingada: %1").arg(item.descricao);
            issue.description = QString("Quantidade x Unitário (%1 x %2) não confere com o Total (%3).")
                                    .arg(item.qtd).arg(item.unitario).arg(item.total());
            report.issues.append(issue);
            report.criticalCount++;
            report.score -= 10;
        }
    }

    // =========================================================================
    // 11. IDENTIFICAÇÃO E ART
    // =========================================================================
    if (planData.artNumero.trimmed().isEmpty()) {
        AiValidationIssue issue;
        issue.severity = AiIssueSeverity::Warning;
        issue.category = "Conformidade Normativa";
        issue.title = "Número da ART em Branco";
        issue.description = "A Anotação de Responsabilidade Técnica (ART) é obrigatória para validade jurídica do Plano de Rigging.";
        report.issues.append(issue);
        report.warningCount++;
        report.score -= 5;
    }

    // Score clamp
    if (report.score < 0) report.score = 0;
    if (report.score > 100) report.score = 100;

    if (report.criticalCount > 0) {
        report.statusTitle = "Crítico: Inconsistências Identificadas";
        report.statusBadgeColor = "#F85149";
    } else if (report.warningCount > 0) {
        report.statusTitle = "Atenção: Alertas Operacionais";
        report.statusBadgeColor = "#D29922";
    } else {
        report.statusTitle = "100% Protegido & Conforme";
        report.statusBadgeColor = "#3FB950";
    }

    return report;
}

bool AiValidator::autoSyncVariables(
    RiggingPlanData& planData,
    CalculationEngine* engine,
    FormatPreset& preset)
{
    if (!engine) return false;

    bool changed = false;

    Q_UNUSED(preset)

    // 1. Sync Pa in Calculation 1 to match planData.cargaPeso
    double targetPaTon = planData.cargaPeso / 1000.0;
    auto* c1 = engine->calculationById(1);
    if (c1 && std::abs(c1->parameterValue(1) - targetPaTon) > 0.001) {
        engine->setParameterValue(1, 1, targetPaTon);
        changed = true;
    }

    // 2. Sync Pa+Pb in Calculation 2 to match (cargaPeso + pesoTotalBalancim)
    double targetPaTotalTon = (planData.cargaPeso + planData.pesoTotalBalancim()) / 1000.0;
    auto* c2 = engine->calculationById(2);
    if (c2 && std::abs(c2->parameterValue(1) - targetPaTotalTon) > 0.001) {
        engine->setParameterValue(2, 1, targetPaTotalTon);
        changed = true;
    }

    // 3. Sync Pc in Calculation 5 to match planData.cargaBruta
    double targetPc = std::round(planData.cargaBruta());
    auto* c5 = engine->calculationById(5);
    if (c5 && std::abs(c5->parameterValue(3) - targetPc) > 1.0) {
        engine->setParameterValue(5, 3, targetPc);
        changed = true;
    }

    // 4. Sync Ro in Calculation 5 to match planData.raioOperacao
    if (c5 && std::abs(c5->parameterValue(2) - planData.raioOperacao) > 0.01) {
        engine->setParameterValue(5, 2, planData.raioOperacao);
        changed = true;
    }

    // 5. Sync Raio in Calculation 7 to match planData.raioOperacao
    auto* c7 = engine->calculationById(7);
    if (c7 && std::abs(c7->parameterValue(0) - planData.raioOperacao) > 0.01) {
        engine->setParameterValue(7, 0, planData.raioOperacao);
        changed = true;
    }

    // 6. Sync ART across preset header if needed
    if (!planData.artNumero.isEmpty() && preset.artNumber() != planData.artNumero) {
        preset.setArtNumber(planData.artNumero);
        changed = true;
    }

    if (!planData.planoNumero.isEmpty() && preset.riggingNumber() != planData.planoNumero) {
        preset.setRiggingNumber(planData.planoNumero);
        changed = true;
    }

    if (changed) {
        preset.setPlanData(planData);
    }

    return changed;
}
