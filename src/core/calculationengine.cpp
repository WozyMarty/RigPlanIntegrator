#include "calculationengine.h"
#include "utils/constants.h"
#include "utils/expressionevaluator.h"
#include "utils/appdatapath.h"
#include <cmath>
#include <QDataStream>
#include <QIODevice>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

CalculationEngine::CalculationEngine(QObject* parent)
    : QObject(parent)
    , m_validator(new DataValidator(this))
{
    connect(m_validator, &DataValidator::validationError,
            this, &CalculationEngine::validationError);
}

CalculationEngine::~CalculationEngine()
{
    qDeleteAll(m_calculations);
}

void CalculationEngine::initialize()
{
    initializeDefaultCalculations();
    loadCustomCalculations();
    loadPresets();
    ensureDefaultPresets();

    // Default to Memorial preset if present, else first preset
    if (!m_presets.isEmpty()) {
        bool found = false;
        for (const auto& p : m_presets) {
            if (p.id() == "preset_memorial_chiller") {
                applyPreset("preset_memorial_chiller");
                found = true;
                break;
            }
        }
        if (!found) {
            applyPreset(m_presets.first().id());
        }
    } else {
        QVector<int> allIds;
        for (auto* c : m_calculations) allIds.append(c->id());
        setActiveCalculations(allIds);
    }
}

void CalculationEngine::initializeDefaultCalculations()
{
    // ============================================================
    // 1-8. MEMORIAL DE CÁLCULO GUINDASTES RIBAS (MD 45007 / CHILLER)
    // ============================================================

    // 1. Cinta tubular 3m x 6t
    auto* c1 = new RiggingCalculation(1, this);
    c1->setName("Cinta tubular 3m x 6t");
    c1->setFormulaText("Te = 1/cos α · (Pa/2) → Te = 1/cos 28° · (3,45/2) t → Te = 1,95 t");
    c1->setDescription("Considerando, no máximo, 85% de taxa de utilização (TU).");
    c1->setResultUnit("t");
    c1->addParameter({"Ângulo α", "°", 28.0, 0.0, 89.0, 28.0, "Ângulo de inclinação da lingada", "α"});
    c1->addParameter({"Carga Pa", "t", 3.45, 0.0, 1000.0, 3.45, "Peso do equipamento (chiller)", "Pa"});
    c1->addParameter({"Capacidade útil", "t", 4.2, 0.1, 1000.0, 4.2, "Capacidade da cinta considerando fator", "Cap"});
    c1->setComputeFunction([](const QVector<double>& v) {
        double rad = v[0] * Constants::PI / 180.0;
        double cosVal = std::cos(rad);
        if (std::abs(cosVal) < 1e-6) return 0.0;
        return (1.0 / cosVal) * (v[1] / 2.0);
    });
    registerCalculation(c1);

    // 2. Cinta tubular 4m x 6t
    auto* c2 = new RiggingCalculation(2, this);
    c2->setName("Cinta tubular 4m x 6t");
    c2->setFormulaText("Te = 1/cos α · ((Pa+Pb)/2) → Te = 1/cos 41° · ((3,45+0,25)/2) t → Te = 2,5 t");
    c2->setDescription("Considerando, no máximo, 85% de taxa de utilização (TU).");
    c2->setResultUnit("t");
    c2->addParameter({"Ângulo α", "°", 41.0, 0.0, 89.0, 41.0, "Ângulo de inclinação da lingada", "α"});
    c2->addParameter({"Carga total", "t", 3.70, 0.0, 1000.0, 3.70, "Peso total (3,45t chiller + 0,25t balancim)", "Pa"});
    c2->addParameter({"Capacidade útil", "t", 4.2, 0.1, 1000.0, 4.2, "Capacidade da cinta considerando fator", "Cap"});
    c2->setComputeFunction([](const QVector<double>& v) {
        double rad = v[0] * Constants::PI / 180.0;
        double cosVal = std::cos(rad);
        if (std::abs(cosVal) < 1e-6) return 0.0;
        return (1.0 / cosVal) * (v[1] / 2.0);
    });
    registerCalculation(c2);

    // 3. Manilhas (carga x balancim)
    auto* c3 = new RiggingCalculation(3, this);
    c3->setName("Manilhas (carga x balancim)");
    c3->setFormulaText("TU = Te / CM = 0,98 / 5,5 = 0,18 ≅ 18%");
    c3->setDescription("Considerando, no máximo, 85% de taxa de utilização (TU)\nDa tabela de manilhas curvas selecionada Ø 7/8” Grau 6 = 5,5 tf");
    c3->setResultUnit("%");
    c3->addParameter({"Tensão atuante (Te)", "tf", 0.98, 0.0, 1000.0, 0.98, "Tensão na manilha inferior", "Te"});
    c3->addParameter({"Capacidade nominal", "tf", 5.5, 0.1, 1000.0, 5.5, "Capacidade curva Ø 7/8\" Grau 6", "CM"});
    c3->setComputeFunction([](const QVector<double>& v) {
        if (v[1] <= 0) return 0.0;
        return (v[0] / v[1]) * 100.0;
    });
    registerCalculation(c3);

    // 4. Manilhas (balancim x gancho)
    auto* c4 = new RiggingCalculation(4, this);
    c4->setName("Manilhas (balancim x gancho)");
    c4->setFormulaText("TU = Te / CM = 5,0 / 7,2 = 0,70 ≅ 70%");
    c4->setDescription("Considerando, no máximo, 85% de taxa de utilização (TU)\nDa tabela de manilhas curvas selecionada Ø 1” Grau 6 = 7,2 tf");
    c4->setResultUnit("%");
    c4->addParameter({"Tensão atuante (Te)", "tf", 5.0, 0.0, 1000.0, 5.0, "Tensão atuante no conjunto superior", "Te"});
    c4->addParameter({"Capacidade nominal", "tf", 7.2, 0.1, 1000.0, 7.2, "Capacidade curva Ø 1\" Grau 6", "CM"});
    c4->setComputeFunction([](const QVector<double>& v) {
        if (v[1] <= 0) return 0.0;
        return (v[0] / v[1]) * 100.0;
    });
    registerCalculation(c4);

    // 5. Esforço nas sapatas
    auto* c5 = new RiggingCalculation(5, this);
    c5->setName("Esforço nas sapatas");
    c5->setFormulaText("M = Pc · Ro | F = M / (a+b) | ES = (Pg/2) + F");
    c5->setDescription("Distância eixo da coluna → sapata esquerda: a=2,95 m\nAbertura entre sapatas: b=5,7 m\nRaio da operação: Ro=5,0 m\nCarga içada: Pc=3700 kgf\nPeso do guindaste + caminhão: Pg=11.550 kgf");
    c5->setResultUnit("kgf");
    c5->addParameter({"Distância a", "m", 2.95, 0.0, 100.0, 2.95, "Distância eixo coluna à sapata", "a"});
    c5->addParameter({"Abertura b", "m", 5.70, 0.1, 100.0, 5.70, "Abertura entre patolas", "b"});
    c5->addParameter({"Raio operacional (Ro)", "m", 5.0, 0.1, 100.0, 5.0, "Raio de içamento", "Ro"});
    c5->addParameter({"Carga içada (Pc)", "kgf", 3700.0, 0.0, 1e6, 3700.0, "Carga içada total", "Pc"});
    c5->addParameter({"Peso guindaste (Pg)", "kgf", 11550.0, 0.0, 1e6, 11550.0, "Peso guindaste + caminhão", "Pg"});
    c5->setComputeFunction([](const QVector<double>& v) {
        double a = v[0];
        double b = v[1];
        double ro = v[2];
        double pc = v[3];
        double pg = v[4];
        double m = pc * ro;
        double f = (a + b > 0) ? (m / (a + b)) : 0.0;
        return (pg / 2.0) + f;
    });
    registerCalculation(c5);

    // 6. Resistência mínima exigida do terreno (RMET)
    auto* c6 = new RiggingCalculation(6, this);
    c6->setName("Resistência mínima exigida do terreno (RMET)");
    c6->setFormulaText("AP = LD × CD × ND | RMET = ES / AP");
    c6->setDescription("LD = Largura do dormente ׀ CD = Comprimento do dormente ׀ ND = Quantidade de dormentes\nES = Esforço nas sapatas (kgf) e AP = Área de patolamento (cm²)");
    c6->setResultUnit("kgf/cm²");
    c6->addParameter({"Largura dormente (LD)", "cm", 50.0, 1.0, 1000.0, 50.0, "Largura do dormente", "LD"});
    c6->addParameter({"Comprimento dormente (CD)", "cm", 50.0, 1.0, 1000.0, 50.0, "Comprimento do dormente", "CD"});
    c6->addParameter({"Quantidade dormentes (ND)", "un", 1.0, 1.0, 100.0, 1.0, "Quantidade de pranchas", "ND"});
    c6->addParameter({"Esforço na sapata (ES)", "kgf", 7914.0, 0.0, 1e6, 7914.0, "Esforço na sapata", "ES"});
    c6->setComputeFunction([](const QVector<double>& v) {
        double ap = v[0] * v[1] * v[2];
        if (ap < 1e-4) return 0.0;
        return v[3] / ap;
    });
    registerCalculation(c6);

    // 7. Capacidade de carga
    auto* c7 = new RiggingCalculation(7, this);
    c7->setName("Capacidade de carga");
    c7->setFormulaText("Raio de operação 5,0 m → ~ 7.948 kgf (interpolado)");
    c7->setDescription("Capacidade tabelada nominal no raio operacional de 5,0 m.");
    c7->setResultUnit("kgf");
    c7->addParameter({"Raio de operação", "m", 5.0, 0.1, 100.0, 5.0, "Raio operacional", "Ro"});
    c7->addParameter({"Capacidade na tabela", "kgf", 7948.0, 0.0, 1e6, 7948.0, "Capacidade tabelada interpolada", "Cap"});
    c7->setComputeFunction([](const QVector<double>& v) {
        return v[1];
    });
    registerCalculation(c7);

    // 8. Gráfico de carga MD 45007
    auto* c8 = new RiggingCalculation(8, this);
    c8->setName("Gráfico de carga MD 45007");
    c8->setFormulaText("Diagrama de alcance e capacidades Madal Palfinger MD 45007");
    c8->setDescription("Consulta do diagrama operacional com lanças estendidas.");
    c8->setResultUnit("m");
    c8->addParameter({"Alcance horizontal", "m", 17.51, 1.0, 100.0, 17.51, "Alcance máximo", "Alc"});
    c8->setComputeFunction([](const QVector<double>& v) {
        return v[0];
    });
    registerCalculation(c8);

    // ============================================================
    // 9-24. OPERAÇÕES GENÉRICAS (MATEMÁTICA / FÍSICA)
    // ============================================================

    // 9. Adição
    auto* c9 = new RiggingCalculation(9, this);
    c9->setName("Adição");
    c9->setFormulaText("R = A + B");
    c9->setDescription("Soma de dois valores");
    c9->setResultUnit("");
    c9->addParameter({"A", "", 0, -1e12, 1e12, 0, "Primeiro valor", "A"});
    c9->addParameter({"B", "", 0, -1e12, 1e12, 0, "Segundo valor", "B"});
    c9->setComputeFunction([](const QVector<double>& v) { return v[0] + v[1]; });
    registerCalculation(c9);

    // 10. Subtração
    auto* c10 = new RiggingCalculation(10, this);
    c10->setName("Subtração");
    c10->setFormulaText("R = A - B");
    c10->setDescription("Diferença entre dois valores");
    c10->setResultUnit("");
    c10->addParameter({"A", "", 0, -1e12, 1e12, 0, "Minuendo", "A"});
    c10->addParameter({"B", "", 0, -1e12, 1e12, 0, "Subtraendo", "B"});
    c10->setComputeFunction([](const QVector<double>& v) { return v[0] - v[1]; });
    registerCalculation(c10);

    // 11. Multiplicação
    auto* c11 = new RiggingCalculation(11, this);
    c11->setName("Multiplicação");
    c11->setFormulaText("R = A × B");
    c11->setDescription("Produto de dois valores");
    c11->setResultUnit("");
    c11->addParameter({"A", "", 0, -1e12, 1e12, 0, "Primeiro fator", "A"});
    c11->addParameter({"B", "", 0, -1e12, 1e12, 0, "Segundo fator", "B"});
    c11->setComputeFunction([](const QVector<double>& v) { return v[0] * v[1]; });
    registerCalculation(c11);

    // 12. Divisão
    auto* c12 = new RiggingCalculation(12, this);
    c12->setName("Divisão");
    c12->setFormulaText("R = A ÷ B");
    c12->setDescription("Quociente de dois valores");
    c12->setResultUnit("");
    c12->addParameter({"A", "", 0, -1e12, 1e12, 0, "Dividendo", "A"});
    c12->addParameter({"B", "", 0, -1e12, 1e12, 0, "Divisor (≠ 0)", "B"});
    c12->setComputeFunction([](const QVector<double>& v) {
        if (std::abs(v[1]) < 1e-10) return std::numeric_limits<double>::quiet_NaN();
        return v[0] / v[1];
    });
    registerCalculation(c12);

    // 13. Potência
    auto* c13 = new RiggingCalculation(13, this);
    c13->setName("Potência");
    c13->setFormulaText("R = base ^ exp");
    c13->setDescription("Exponenciação");
    c13->setResultUnit("");
    c13->addParameter({"base", "", 0, -1e6, 1e6, 0, "Base", "base"});
    c13->addParameter({"exp", "", 0, -100, 100, 0, "Expoente", "exp"});
    c13->setComputeFunction([](const QVector<double>& v) { return std::pow(v[0], v[1]); });
    registerCalculation(c13);

    // 14. Raiz Quadrada
    auto* c14 = new RiggingCalculation(14, this);
    c14->setName("Raiz Quadrada");
    c14->setFormulaText("R = √A");
    c14->setDescription("Raiz quadrada de um valor positivo");
    c14->setResultUnit("");
    c14->addParameter({"A", "", 0, 0, 1e12, 0, "Valor (≥ 0)", "A"});
    c14->setComputeFunction([](const QVector<double>& v) {
        if (v[0] < 0) return std::numeric_limits<double>::quiet_NaN();
        return std::sqrt(v[0]);
    });
    registerCalculation(c14);

    // 15. Hipotenusa
    auto* c15 = new RiggingCalculation(15, this);
    c15->setName("Hipotenusa");
    c15->setFormulaText("c = √(a² + b²)");
    c15->setDescription("Teorema de Pitágoras");
    c15->setResultUnit("m");
    c15->addParameter({"Cateto a", "m", 0, 0, 1e6, 0, "Primeiro cateto", "a"});
    c15->addParameter({"Cateto b", "m", 0, 0, 1e6, 0, "Segundo cateto", "b"});
    c15->setComputeFunction([](const QVector<double>& v) { return std::hypot(v[0], v[1]); });
    registerCalculation(c15);

    // 16. Área do Círculo
    auto* c16 = new RiggingCalculation(16, this);
    c16->setName("Área do Círculo");
    c16->setFormulaText("A = π × r²");
    c16->setDescription("Área de seção circular");
    c16->setResultUnit("m²");
    c16->addParameter({"Raio", "m", 0, 0, 1e6, 0, "Raio do círculo", "r"});
    c16->setComputeFunction([](const QVector<double>& v) { return Constants::PI * v[0] * v[0]; });
    registerCalculation(c16);

    setupDependencies();
}

void CalculationEngine::registerCalculation(RiggingCalculation* calc)
{
    m_calculations.append(calc);
    m_calcMap[calc->id()] = calc;
    m_activeIds.insert(calc->id());

    connect(calc, &RiggingCalculation::resultChanged,
            this, &CalculationEngine::onResultChanged);
    connect(calc, &RiggingCalculation::descriptionChanged,
            this, [this, calc]() {
                emit calculationUpdated(calc->id(), calc->result());
            });
}

void CalculationEngine::setupDependencies()
{
    m_dependencies.insert(5, {6, 3});
    auto* rmet = calculationById(6);
    if (rmet) {
        rmet->setDependsOn(5, 3);
    }
}

RiggingCalculation* CalculationEngine::calculationById(int id) const
{
    return m_calcMap.value(id, nullptr);
}

QVector<RiggingCalculation*> CalculationEngine::activeCalculations() const
{
    QVector<RiggingCalculation*> active;
    for (auto* calc : m_calculations) {
        if (m_activeIds.contains(calc->id())) {
            active.append(calc);
        }
    }
    return active;
}

QVector<int> CalculationEngine::activeCalculationIds() const
{
    QVector<int> ids;
    for (auto* calc : m_calculations) {
        if (m_activeIds.contains(calc->id())) {
            ids.append(calc->id());
        }
    }
    return ids;
}

void CalculationEngine::setActiveCalculations(const QVector<int>& ids)
{
    m_activeIds.clear();
    for (int id : ids) {
        if (m_calcMap.contains(id)) {
            m_activeIds.insert(id);
        }
    }

    emit activeCalculationsChanged();
    emit progressChanged(completedCount(), activeCount());

    if (allCompleted()) {
        emit allCalculationsReady();
    }
}

bool CalculationEngine::isCalculationActive(int id) const
{
    return m_activeIds.contains(id);
}

void CalculationEngine::setCalculationActive(int id, bool active)
{
    if (active) {
        m_activeIds.insert(id);
    } else {
        m_activeIds.remove(id);
    }

    emit activeCalculationsChanged();
    emit progressChanged(completedCount(), activeCount());
}

CalculationPreset CalculationEngine::presetById(const QString& id) const
{
    for (const auto& p : m_presets) {
        if (p.id() == id) return p;
    }
    return CalculationPreset();
}

CalculationPreset CalculationEngine::currentPreset() const
{
    return presetById(m_currentPresetId);
}

void CalculationEngine::applyPreset(const QString& presetId)
{
    for (const auto& p : m_presets) {
        if (p.id() == presetId) {
            m_currentPresetId = presetId;
            setActiveCalculations(p.calculationIds());
            emit presetApplied(p);
            return;
        }
    }
}

void CalculationEngine::savePreset(const CalculationPreset& preset)
{
    bool found = false;
    for (int i = 0; i < m_presets.size(); ++i) {
        if (m_presets[i].id() == preset.id()) {
            m_presets[i] = preset;
            m_presets[i].touch();
            found = true;
            break;
        }
    }
    if (!found) {
        m_presets.append(preset);
    }

    savePresets();
    emit calculationPresetsChanged();
}

void CalculationEngine::deletePreset(const QString& presetId)
{
    for (int i = 0; i < m_presets.size(); ++i) {
        if (m_presets[i].id() == presetId) {
            if (m_presets[i].isBuiltin()) return; // Cannot delete builtin presets
            m_presets.removeAt(i);
            savePresets();
            emit calculationPresetsChanged();
            return;
        }
    }
}

RiggingCalculation* CalculationEngine::addCustomCalculation(const QString& title,
                                                            const QString& formula,
                                                            const QVector<CalcParameter>& params,
                                                            const QString& resultUnit,
                                                            const QString& description)
{
    int newId = 101;
    for (auto* c : m_calculations) {
        if (c->id() >= newId) {
            newId = c->id() + 1;
        }
    }

    auto* calc = new RiggingCalculation(newId, this);
    calc->setName(title);
    calc->setFormulaText(formula);
    calc->setCustomFormula(formula);
    calc->setIsCustom(true);
    calc->setDescription(description);
    calc->setResultUnit(resultUnit);

    for (const auto& p : params) {
        calc->addParameter(p);
    }

    setupCustomCalculationCompute(calc);
    registerCalculation(calc);
    m_activeIds.insert(newId);

    saveCustomCalculations();

    emit customCalculationAdded(calc);
    emit activeCalculationsChanged();
    emit progressChanged(completedCount(), activeCount());

    return calc;
}

void CalculationEngine::removeCustomCalculation(int id)
{
    auto* calc = calculationById(id);
    if (!calc || !calc->isCustom()) return;

    m_activeIds.remove(id);
    m_calcMap.remove(id);
    m_calculations.removeAll(calc);

    calc->deleteLater();
    saveCustomCalculations();

    emit customCalculationRemoved(id);
    emit activeCalculationsChanged();
    emit progressChanged(completedCount(), activeCount());
}

void CalculationEngine::setupCustomCalculationCompute(RiggingCalculation* calc)
{
    if (!calc) return;

    QString formulaExpr = ExpressionEvaluator::extractFormulaExpression(calc->customFormula());
    QVector<QString> paramSymbols;
    for (const auto& p : calc->parameters()) {
        paramSymbols.append(p.symbol.isEmpty() ? p.name : p.symbol);
    }

    calc->setComputeFunction([formulaExpr, paramSymbols](const QVector<double>& values) {
        QMap<QString, double> vars;
        for (int i = 0; i < paramSymbols.size() && i < values.size(); ++i) {
            vars[paramSymbols[i]] = values[i];
        }
        return ExpressionEvaluator::evaluate(formulaExpr, vars);
    });
}

void CalculationEngine::setParameterValue(int calcId, int paramIndex, double value)
{
    auto* calc = calculationById(calcId);
    if (calc) {
        calc->setParameterValue(paramIndex, value);
    }
}

void CalculationEngine::moveCalculation(int fromIndex, int toIndex)
{
    if (fromIndex < 0 || fromIndex >= m_calculations.size()) return;
    if (toIndex < 0 || toIndex >= m_calculations.size()) return;
    if (fromIndex == toIndex) return;

    auto* calc = m_calculations.takeAt(fromIndex);
    m_calculations.insert(toIndex, calc);

    for (int i = 0; i < m_calculations.size(); ++i) {
        m_calculations[i]->setOrder(i);
    }
}

int CalculationEngine::completedCount() const
{
    int count = 0;
    for (const auto* calc : activeCalculations()) {
        if (calc->status() == RiggingCalculation::Computed) {
            ++count;
        }
    }
    return count;
}

void CalculationEngine::recalculateAll()
{
    for (auto* calc : activeCalculations()) {
        calc->compute();
    }
}

void CalculationEngine::collapseAll()
{
    for (auto* calc : activeCalculations()) {
        calc->setCollapsed(true);
    }
}

void CalculationEngine::expandAll()
{
    for (auto* calc : activeCalculations()) {
        calc->setCollapsed(false);
    }
}

void CalculationEngine::clearAll()
{
    for (auto* calc : activeCalculations()) {
        calc->clearParameters();
    }
}

QByteArray CalculationEngine::serializeAll() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    auto active = activeCalculations();
    stream << static_cast<int>(active.size());
    for (const auto* calc : active) {
        stream << calc->serialize();
    }
    return data;
}

void CalculationEngine::onResultChanged(int calcId, double result)
{
    emit calculationUpdated(calcId, result);
    propagateDependency(calcId, result);
    emit progressChanged(completedCount(), activeCount());

    if (allCompleted()) {
        emit allCalculationsReady();
    }
}

void CalculationEngine::propagateDependency(int sourceCalcId, double result)
{
    auto targets = m_dependencies.values(sourceCalcId);
    for (const auto& target : targets) {
        int targetCalcId = target.first;
        int targetParamIndex = target.second;
        auto* targetCalc = calculationById(targetCalcId);
        if (targetCalc) {
            targetCalc->setParameterValue(targetParamIndex, result);
        }
    }
}

QString CalculationEngine::dataDirectory() const
{
    QString appData = applicationDataLocation();
    QDir dir(appData);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return appData;
}

void CalculationEngine::loadCustomCalculations()
{
    QFile file(dataDirectory() + "/custom_calculations.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) return;

    QJsonArray arr = doc.array();
    for (const auto& val : arr) {
        if (!val.isObject()) continue;
        RiggingCalculation* calc = RiggingCalculation::fromJson(val.toObject(), this);
        setupCustomCalculationCompute(calc);
        registerCalculation(calc);
    }
}

void CalculationEngine::saveCustomCalculations()
{
    QJsonArray arr;
    for (auto* calc : m_calculations) {
        if (calc->isCustom()) {
            arr.append(calc->toJson());
        }
    }

    QFile file(dataDirectory() + "/custom_calculations.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
        file.close();
    }
}

void CalculationEngine::loadPresets()
{
    m_presets.clear();

    QFile file(dataDirectory() + "/calc_presets.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) return;

    QJsonArray arr = doc.array();
    for (const auto& val : arr) {
        if (val.isObject()) {
            m_presets.append(CalculationPreset::fromJson(val.toObject()));
        }
    }
}

void CalculationEngine::savePresets()
{
    QJsonArray arr;
    for (const auto& p : m_presets) {
        arr.append(p.toJson());
    }

    QFile file(dataDirectory() + "/calc_presets.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
        file.close();
    }
}

void CalculationEngine::ensureDefaultPresets()
{
    QVector<int> allIds;
    for (auto* c : m_calculations) allIds.append(c->id());

    bool hasMemorial = false;
    bool hasAll = false;
    bool hasSpeed = false;
    for (const auto& p : m_presets) {
        if (p.id() == "preset_memorial_chiller") hasMemorial = true;
        if (p.id() == "preset_all") hasAll = true;
        if (p.id() == "preset_speed_pressure") hasSpeed = true;
    }

    if (!hasMemorial) {
        m_presets.prepend(CalculationPreset::createMemorialRiggingPreset());
    }
    if (!hasAll) {
        m_presets.append(CalculationPreset::createAllPreset(allIds));
    }
    if (!hasSpeed) {
        m_presets.append(CalculationPreset::createSpeedAndPressurePreset());
        m_presets.append(CalculationPreset::createBasicOperationsPreset());
        m_presets.append(CalculationPreset::createMechanicsPreset());
    }

    if (m_currentPresetId.isEmpty() || !hasMemorial) {
        m_currentPresetId = "preset_memorial_chiller";
    }

    savePresets();
}
