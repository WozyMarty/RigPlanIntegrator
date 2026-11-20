#include "calculationpreset.h"
#include <QUuid>

CalculationPreset::CalculationPreset()
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_createdAt(QDateTime::currentDateTime())
    , m_modifiedAt(QDateTime::currentDateTime())
{
}

CalculationPreset::CalculationPreset(const QString& name, const QVector<int>& calcIds, const QString& description)
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_name(name)
    , m_description(description)
    , m_calculationIds(calcIds)
    , m_createdAt(QDateTime::currentDateTime())
    , m_modifiedAt(QDateTime::currentDateTime())
{
}

QJsonObject CalculationPreset::toJson() const
{
    QJsonObject obj;
    obj["id"] = m_id;
    obj["name"] = m_name;
    obj["description"] = m_description;
    obj["is_builtin"] = m_isBuiltin;
    obj["is_favorite"] = m_isFavorite;
    obj["created_at"] = m_createdAt.toString(Qt::ISODate);
    obj["modified_at"] = m_modifiedAt.toString(Qt::ISODate);

    QJsonArray idsArr;
    for (int id : m_calculationIds) {
        idsArr.append(id);
    }
    obj["calculation_ids"] = idsArr;

    return obj;
}

CalculationPreset CalculationPreset::fromJson(const QJsonObject& json)
{
    CalculationPreset p;
    p.m_id = json["id"].toString(p.m_id);
    p.m_name = json["name"].toString("Sem Nome");
    p.m_description = json["description"].toString();
    p.m_isBuiltin = json["is_builtin"].toBool(false);
    p.m_isFavorite = json["is_favorite"].toBool(false);
    p.m_createdAt = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
    p.m_modifiedAt = QDateTime::fromString(json["modified_at"].toString(), Qt::ISODate);

    p.m_calculationIds.clear();
    QJsonArray idsArr = json["calculation_ids"].toArray();
    for (const auto& val : idsArr) {
        p.m_calculationIds.append(val.toInt());
    }

    return p;
}

CalculationPreset CalculationPreset::createMemorialRiggingPreset()
{
    CalculationPreset p("Memorial Içamento Chiller (MD 45007)", {1, 2, 3, 4, 5, 6, 7, 8}, "Memorial de cálculo com lingada, manilhas, sapatas e RMET");
    p.setId("preset_memorial_chiller");
    p.setIsBuiltin(true);
    p.setIsFavorite(true);
    return p;
}

CalculationPreset CalculationPreset::createAllPreset(const QVector<int>& allIds)
{
    CalculationPreset p("Todos os Cálculos (Completo)", allIds, "Contém todos os cálculos disponíveis do sistema");
    p.setId("preset_all");
    p.setIsBuiltin(true);
    p.setIsFavorite(true);
    return p;
}

CalculationPreset CalculationPreset::createSpeedAndPressurePreset()
{
    // Multiplication (3), Linear Velocity (13), Pressure (15)
    CalculationPreset p("Operação Velocidade & Pressão", {3, 13, 15}, "Perfil focado em velocidade linear, pressão e multiplicação");
    p.setId("preset_speed_pressure");
    p.setIsBuiltin(true);
    p.setIsFavorite(true);
    return p;
}

CalculationPreset CalculationPreset::createBasicOperationsPreset()
{
    // Addition (1), Subtraction (2), Multiplication (3), Division (4)
    CalculationPreset p("Operações Básicas", {1, 2, 3, 4}, "Adição, subtração, multiplicação e divisão fundamental");
    p.setId("preset_basic_ops");
    p.setIsBuiltin(true);
    return p;
}

CalculationPreset CalculationPreset::createMechanicsPreset()
{
    // Work (10), Power (11), Torque (12), Force (14), Pressure (15)
    CalculationPreset p("Mecânica & Içamento", {10, 11, 12, 14, 15}, "Cálculos de dinâmica, força, pressão e trabalho mecânico");
    p.setId("preset_mechanics");
    p.setIsBuiltin(true);
    return p;
}
