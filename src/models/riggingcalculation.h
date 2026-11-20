#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <functional>

/**
 * @brief A single input parameter for a calculation.
 */
struct CalcParameter {
    QString name;       // Friendly label, e.g., "Variação do Espaço"
    QString unit;       // e.g., "m", "kg", "m/s"
    double value    = 0.0;
    double minValue = -1.0e12;
    double maxValue =  1.0e12;
    double defaultValue = 0.0;
    QString tooltip;    // Help text
    QString symbol;     // Variable symbol in formula, e.g. "ΔS", "dt"
    bool isSet      = false; // Whether user has explicitly provided a value

    bool hasValue() const { return isSet || value != 0.0 || defaultValue != 0.0; }
    double effectiveValue() const { return isSet ? value : ((defaultValue != 0.0) ? defaultValue : value); }
};

/**
 * @brief Model representing one rigging/engineering calculation.
 *
 * Each calculation has a name, a display formula, input parameters,
 * and a computation function. Results are computed in real-time.
 */
class RiggingCalculation : public QObject {
    Q_OBJECT

    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(double result READ result NOTIFY resultChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(bool collapsed READ isCollapsed WRITE setCollapsed NOTIFY collapsedChanged)

public:
    enum Status {
        Empty,          // No inputs filled
        Incomplete,     // Some inputs filled
        Computed,       // All filled, result computed
        Error           // Computation error
    };
    Q_ENUM(Status)

    explicit RiggingCalculation(int id, QObject* parent = nullptr);

    // --- Accessors ---
    int id() const { return m_id; }

    QString name() const { return m_name; }
    void setName(const QString& name);

    QString formulaText() const { return m_formulaText; }
    void setFormulaText(const QString& text);

    QString description() const { return m_description; }
    void setDescription(const QString& desc);

    QString resultUnit() const { return m_resultUnit; }
    void setResultUnit(const QString& unit);

    double result() const { return m_result; }
    bool isValid() const { return m_status == Computed; }
    Status status() const { return m_status; }

    bool isCollapsed() const { return m_collapsed; }
    void setCollapsed(bool collapsed);

    int order() const { return m_order; }
    void setOrder(int order) { m_order = order; }

    bool isCustom() const { return m_isCustom; }
    void setIsCustom(bool custom) { m_isCustom = custom; }

    QString customFormula() const { return m_customFormula; }
    void setCustomFormula(const QString& formula) { m_customFormula = formula; }

    // --- Parameters ---
    const QVector<CalcParameter>& parameters() const { return m_parameters; }
    void addParameter(const CalcParameter& param);
    void setParameterValue(int index, double value);
    void resetParameter(int index);
    void clearParameters();
    double parameterValue(int index) const;
    int parameterCount() const { return m_parameters.size(); }
    bool allParametersFilled() const;

    // --- Computation ---
    using ComputeFunc = std::function<double(const QVector<double>&)>;
    void setComputeFunction(ComputeFunc func);
    void compute();

    // --- Dependency (for linked calculations) ---
    void setDependsOn(int calcId, int paramIndex);
    int dependsOnCalcId() const { return m_dependsOnCalcId; }
    int dependsOnParamIndex() const { return m_dependsOnParamIndex; }

    // --- Serialization ---
    QByteArray serialize() const;
    QJsonObject toJson() const;
    static RiggingCalculation* fromJson(const QJsonObject& json, QObject* parent = nullptr);

signals:
    void nameChanged();
    void descriptionChanged();
    void resultChanged(int id, double result);
    void validChanged();
    void collapsedChanged();
    void parameterChanged(int calcId, int paramIndex, double value);
    void statusChanged(Status status);

private:
    void updateStatus();

    int m_id;
    QString m_name;
    QString m_formulaText;
    QString m_description;
    QString m_resultUnit;
    QVector<CalcParameter> m_parameters;
    ComputeFunc m_computeFunc;
    double m_result = 0.0;
    Status m_status = Empty;
    bool m_collapsed = false;
    int m_order = 0;
    bool m_isCustom = false;
    QString m_customFormula;

    // Dependency
    int m_dependsOnCalcId = -1;
    int m_dependsOnParamIndex = -1;
};
