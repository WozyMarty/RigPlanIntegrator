#include "documentsection.h"
#include <QFile>
#include <QFileInfo>
#include <QUuid>

DocumentSection::DocumentSection()
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_type(PresetStyle)
    , m_title("Nova Página")
    , m_enabled(true)
    , m_order(0)
{
}

DocumentSection::DocumentSection(const QString& id, Type type, const QString& title)
    : m_id(id.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : id)
    , m_type(type)
    , m_title(title)
    , m_enabled(true)
    , m_order(0)
{
}

bool DocumentSection::hasImage() const
{
    if (!m_imageBase64.isEmpty()) return true;
    if (!m_imagePath.isEmpty() && QFile::exists(m_imagePath)) return true;
    if (m_type == SolidworksImage) {
        // Can fallback to resource sample if available
        return QFile::exists(":/images/image1.png") || QFile::exists("resources/images/image1.png");
    }
    return false;
}

QString DocumentSection::effectiveImageBase64() const
{
    if (!m_imageBase64.isEmpty()) {
        return m_imageBase64;
    }

    if (!m_imagePath.isEmpty()) {
        QFile file(m_imagePath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            QString format = "png";
            if (m_imagePath.endsWith(".jpg", Qt::CaseInsensitive) || m_imagePath.endsWith(".jpeg", Qt::CaseInsensitive)) {
                format = "jpeg";
            }
            return QString("data:image/%1;base64,%2").arg(format, QString::fromLatin1(data.toBase64()));
        }
    }

    // Default sample fallback for SolidWorks
    if (m_type == SolidworksImage) {
        QStringList paths = {":/images/image1.png", "resources/images/image1.png"};
        for (const auto& path : paths) {
            QFile file(path);
            if (file.open(QIODevice::ReadOnly)) {
                return "data:image/png;base64," + QString::fromLatin1(file.readAll().toBase64());
            }
        }
    }

    return QString();
}

QString DocumentSection::typeName() const
{
    switch (m_type) {
    case PresetStyle:
        return QString("Estilo: %1").arg(m_preset.name());
    case CreaImage:
        return "Anexo CREA (ART)";
    case SolidworksImage:
        return "Desenho Técnico SOLIDWORKS";
    case CustomImage:
        return "Imagem Personalizada";
    }
    return "Seção de Documento";
}

QString DocumentSection::typeIcon() const
{
    switch (m_type) {
    case PresetStyle:
        return "📋";
    case CreaImage:
        return "📜";
    case SolidworksImage:
        return "📐";
    case CustomImage:
        return "🖼️";
    }
    return "📄";
}

QJsonObject DocumentSection::toJson() const
{
    QJsonObject obj;
    obj["id"] = m_id;
    obj["type"] = static_cast<int>(m_type);
    obj["title"] = m_title;
    obj["enabled"] = m_enabled;
    obj["order"] = m_order;
    obj["image_path"] = m_imagePath;
    obj["image_base64"] = m_imageBase64;
    if (m_type == PresetStyle) {
        obj["preset"] = m_preset.toJson();
    }
    return obj;
}

DocumentSection DocumentSection::fromJson(const QJsonObject& json)
{
    DocumentSection sec;
    sec.m_id = json["id"].toString();
    sec.m_type = static_cast<Type>(json["type"].toInt(0));
    sec.m_title = json["title"].toString("Página");
    sec.m_enabled = json["enabled"].toBool(true);
    sec.m_order = json["order"].toInt(0);
    sec.m_imagePath = json["image_path"].toString();
    sec.m_imageBase64 = json["image_base64"].toString();
    if (sec.m_type == PresetStyle && json.contains("preset")) {
        sec.m_preset = FormatPreset::fromJson(json["preset"].toObject());
    }
    return sec;
}
