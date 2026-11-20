#pragma once

#include <QString>
#include <QJsonObject>
#include "formatpreset.h"

/**
 * @brief Represents a single page or multi-page section in the continuous document.
 *
 * Can be a preset style (e.g. Plano de Rigging, Memorial, Standard),
 * an imported CREA / ART image, a SOLIDWORKS CAD drawing image, or a custom image.
 */
class DocumentSection {
public:
    enum Type {
        PresetStyle = 0,
        CreaImage = 1,
        SolidworksImage = 2,
        CustomImage = 3
    };

    DocumentSection();
    DocumentSection(const QString& id, Type type, const QString& title);

    QString id() const { return m_id; }
    void setId(const QString& id) { m_id = id; }

    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }

    QString title() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; }

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

    int order() const { return m_order; }
    void setOrder(int order) { m_order = order; }

    // Preset Style section
    FormatPreset preset() const { return m_preset; }
    void setPreset(const FormatPreset& preset) { m_preset = preset; }
    QString presetId() const { return m_preset.id(); }

    // Image section
    QString imagePath() const { return m_imagePath; }
    void setImagePath(const QString& path) { m_imagePath = path; }

    QString imageBase64() const { return m_imageBase64; }
    void setImageBase64(const QString& b64) { m_imageBase64 = b64; }

    bool hasImage() const;
    QString effectiveImageBase64() const;

    // Helpers
    bool isPreset() const { return m_type == PresetStyle; }
    bool isCrea() const { return m_type == CreaImage; }
    bool isSolidworks() const { return m_type == SolidworksImage; }
    bool isImage() const { return m_type != PresetStyle; }

    QString typeName() const;
    QString typeIcon() const;

    // Serialization
    QJsonObject toJson() const;
    static DocumentSection fromJson(const QJsonObject& json);

private:
    QString m_id;
    Type m_type = PresetStyle;
    QString m_title;
    bool m_enabled = true;
    int m_order = 0;

    FormatPreset m_preset;
    QString m_imagePath;
    QString m_imageBase64;
};
