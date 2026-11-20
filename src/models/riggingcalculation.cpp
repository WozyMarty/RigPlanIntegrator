#include "riggingcalculation.h"
#include <QDataStream>
#include <QIODevice>
#include <cmath>

RiggingCalculation::RiggingCalculation(int id, QObject* parent)
    : QObject(parent), m_id(id), m_order(id)
{
}

void RiggingCalculation::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

void RiggingCalculation::setFormulaText(const QString& text)
{
    m_formulaText = text;
}

void RiggingCalculation::setDescription(const QString& desc)
{
    if (m_description != desc) {
        m_description = desc;
        emit descriptionChanged();
    }
}

void RiggingCalculation::setResultUnit(const QString& unit)
{
    m_resultUnit = unit;
}

void RiggingCalculation::setCollapsed(bool collapsed)
{
    if (m_collapsed != collapsed) {
        m_collapsed = collapsed;
        emit collapsedChanged();
    }
}

void RiggingCalculation::addParameter(const CalcParameter& param)
{
    m_parameters.append(param);
}

void RiggingCalculation::setParameterValue(int index, double value)
{
    if (index >= 0 && index < m_parameters.size()) {
        m_parameters[index].value = value;
        m_parameters[index].isSet = true;
        emit parameterChanged(m_id, index, value);
        compute();
    }
}

void RiggingCalculation::resetParameter(int index)
{
    if (index >= 0 && index < m_parameters.size()) {
        m_parameters[index].value = 0.0;
        m_parameters[index].isSet = false;
        emit parameterChanged(m_id, index, 0.0);
        compute();
    }
}

void RiggingCalculation::clearParameters()
{
    for (int i = 0; i < m_parameters.size(); ++i) {
        m_parameters[i].value = 0.0;
        m_parameters[i].isSet = false;
    }
    m_result = 0.0;
    m_status = Empty;
    emit statusChanged(m_status);
    emit resultChanged(m_id, 0.0);
}

double RiggingCalculation::parameterValue(int index) const
{
    if (index >= 0 && index < m_parameters.size()) {
        return m_parameters[index].effectiveValue();
    }
    return 0.0;
}

bool RiggingCalculation::allParametersFilled() const
{
    for (const auto& p : m_parameters) {
        if (!p.hasValue()) {
            return false;
        }
    }
    return !m_parameters.isEmpty();
}

void RiggingCalculation::setComputeFunction(ComputeFunc func)
{
    m_computeFunc = func;
}

void RiggingCalculation::compute()
{
    updateStatus();

    if (!m_computeFunc || !allParametersFilled()) {
        if (m_result != 0.0) {
            m_result = 0.0;
            emit resultChanged(m_id, m_result);
        }
        return;
    }

    QVector<double> values;
    values.reserve(m_parameters.size());
    for (const auto& p : m_parameters) {
        values.append(p.effectiveValue());
    }

    double oldResult = m_result;
    m_result = m_computeFunc(values);

    // Check for invalid result
    if (std::isnan(m_result) || std::isinf(m_result)) {
        m_status = Error;
        m_result = 0.0;
        emit statusChanged(m_status);
        return;
    }

    m_status = Computed;
    emit statusChanged(m_status);

    if (m_result != oldResult) {
        emit resultChanged(m_id, m_result);
    }
    emit validChanged();
}

void RiggingCalculation::setDependsOn(int calcId, int paramIndex)
{
    m_dependsOnCalcId = calcId;
    m_dependsOnParamIndex = paramIndex;
}

QByteArray RiggingCalculation::serialize() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_id << m_name << m_formulaText;
    stream << static_cast<int>(m_parameters.size());
    for (const auto& p : m_parameters) {
        stream << p.name << p.unit << p.value << p.defaultValue;
    }
    stream << m_result;
    return data;
}

QJsonObject RiggingCalculation::toJson() const
{
    QJsonObject obj;
    obj["id"] = m_id;
    obj["name"] = m_name;
    obj["formula_text"] = m_formulaText;
    obj["description"] = m_description;
    obj["result_unit"] = m_resultUnit;
    obj["is_custom"] = m_isCustom;
    obj["custom_formula"] = m_customFormula;
    obj["order"] = m_order;

    QJsonArray paramsArr;
    for (const auto& p : m_parameters) {
        QJsonObject paramObj;
        paramObj["name"] = p.name;
        paramObj["symbol"] = p.symbol;
        paramObj["unit"] = p.unit;
        paramObj["default_value"] = p.defaultValue;
        paramObj["min_value"] = p.minValue;
        paramObj["max_value"] = p.maxValue;
        paramObj["tooltip"] = p.tooltip;
        paramsArr.append(paramObj);
    }
    obj["parameters"] = paramsArr;

    return obj;
}

RiggingCalculation* RiggingCalculation::fromJson(const QJsonObject& json, QObject* parent)
{
    int id = json["id"].toInt(100);
    auto* calc = new RiggingCalculation(id, parent);
    calc->setName(json["name"].toString());
    calc->setFormulaText(json["formula_text"].toString());
    calc->setDescription(json["description"].toString());
    calc->setResultUnit(json["result_unit"].toString());
    calc->setIsCustom(json["is_custom"].toBool(false));
    calc->setCustomFormula(json["custom_formula"].toString());
    calc->setOrder(json["order"].toInt(id));

    QJsonArray paramsArr = json["parameters"].toArray();
    for (const auto& val : paramsArr) {
        QJsonObject pObj = val.toObject();
        CalcParameter p;
        p.name = pObj["name"].toString();
        p.symbol = pObj["symbol"].toString();
        p.unit = pObj["unit"].toString();
        p.defaultValue = pObj["default_value"].toDouble(0.0);
        p.minValue = pObj["min_value"].toDouble(-1e12);
        p.maxValue = pObj["max_value"].toDouble(1e12);
        p.tooltip = pObj["tooltip"].toString();
        calc->addParameter(p);
    }

    return calc;
}

void RiggingCalculation::updateStatus()
{
    bool anyFilled = false;
    bool allFilled = true;

    for (const auto& p : m_parameters) {
        if (p.hasValue()) {
            anyFilled = true;
        } else {
            allFilled = false;
        }
    }

    if (m_parameters.isEmpty()) {
        m_status = Empty;
    } else if (!anyFilled) {
        m_status = Empty;
    } else if (!allFilled) {
        m_status = Incomplete;
    }
}
