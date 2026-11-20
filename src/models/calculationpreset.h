#pragma once

#include <QString>
#include <QVector>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>

/**
 * @brief Represents a named set of selected calculations to be active.
 *
 * For example: Preset 1 with "Velocidade", "Pressão" and "Multiplicação".
 * Allows users to choose, create, and persist calculation profiles.
 */
class CalculationPreset {
public:
    CalculationPreset();
    explicit CalculationPreset(const QString& name, const QVector<int>& calcIds, const QString& description = "");

    QString id() const { return m_id; }
    void setId(const QString& id) { m_id = id; }

    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    QString description() const { return m_description; }
    void setDescription(const QString& desc) { m_description = desc; }

    QVector<int> calculationIds() const { return m_calculationIds; }
    void setCalculationIds(const QVector<int>& ids) { m_calculationIds = ids; }

    bool isBuiltin() const { return m_isBuiltin; }
    void setIsBuiltin(bool builtin) { m_isBuiltin = builtin; }

    bool isFavorite() const { return m_isFavorite; }
    void setIsFavorite(bool fav) { m_isFavorite = fav; }

    QDateTime createdAt() const { return m_createdAt; }
    QDateTime modifiedAt() const { return m_modifiedAt; }
    void touch() { m_modifiedAt = QDateTime::currentDateTime(); }

    QJsonObject toJson() const;
    static CalculationPreset fromJson(const QJsonObject& json);

    // Factory presets
    static CalculationPreset createMemorialRiggingPreset();
    static CalculationPreset createAllPreset(const QVector<int>& allIds);
    static CalculationPreset createSpeedAndPressurePreset();
    static CalculationPreset createBasicOperationsPreset();
    static CalculationPreset createMechanicsPreset();

private:
    QString m_id;
    QString m_name;
    QString m_description;
    QVector<int> m_calculationIds;
    bool m_isBuiltin = false;
    bool m_isFavorite = false;
    QDateTime m_createdAt;
    QDateTime m_modifiedAt;
};
