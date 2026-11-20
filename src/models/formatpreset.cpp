#include "formatpreset.h"
#include <QUuid>
#include <QJsonArray>

FormatPreset::FormatPreset()
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_createdAt(QDateTime::currentDateTime())
    , m_modifiedAt(QDateTime::currentDateTime())
{
}

QJsonObject FormatPreset::toJson() const
{
    QJsonObject obj;
    obj["id"] = m_id;
    obj["name"] = m_name;
    obj["favorite"] = m_favorite;
    obj["is_default"] = m_isDefault;
    obj["created_at"] = m_createdAt.toString(Qt::ISODate);
    obj["modified_at"] = m_modifiedAt.toString(Qt::ISODate);

    // Typography
    obj["font_family"] = m_fontFamily;
    obj["font_size_body"] = m_fontSizeBody;
    obj["font_size_title"] = m_fontSizeTitle;
    obj["font_size_subtitle"] = m_fontSizeSubtitle;

    // Colors
    obj["color_title"] = m_colorTitle.name();
    obj["color_body"] = m_colorBody.name();
    obj["color_accent"] = m_colorAccent.name();
    obj["color_table_header"] = m_colorTableHeader.name();

    // Margins
    QJsonObject margins;
    margins["top"] = m_marginTop;
    margins["bottom"] = m_marginBottom;
    margins["left"] = m_marginLeft;
    margins["right"] = m_marginRight;
    obj["margins"] = margins;

    // Content
    obj["header_text"] = m_headerText;
    obj["subtitle"] = m_subtitle;
    obj["art_number"] = m_artNumber;
    obj["rigging_number"] = m_riggingNumber;
    obj["group_title"] = m_groupTitle;
    obj["footer_text"] = m_footerText;
    obj["show_logo"] = m_showLogo;
    obj["show_diagram"] = m_showDiagram;
    obj["logo_path"] = m_logoPath;

    // Document Layout
    obj["document_layout"] = static_cast<int>(m_documentLayout);
    obj["plan_data"] = m_planData.toJson();

    // Table
    obj["table_style"] = static_cast<int>(m_tableStyle);
    obj["table_borders"] = m_tableBorders;
    obj["table_alternating_rows"] = m_tableAlternatingRows;

    return obj;
}

FormatPreset FormatPreset::fromJson(const QJsonObject& json)
{
    FormatPreset p;
    p.m_id = json["id"].toString(p.m_id);
    p.m_name = json["name"].toString("Sem Nome");
    p.m_favorite = json["favorite"].toBool(false);
    p.m_isDefault = json["is_default"].toBool(false);
    p.m_createdAt = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
    p.m_modifiedAt = QDateTime::fromString(json["modified_at"].toString(), Qt::ISODate);

    p.m_fontFamily = json["font_family"].toString("Arial");
    p.m_fontSizeBody = json["font_size_body"].toInt(11);
    p.m_fontSizeTitle = json["font_size_title"].toInt(20);
    p.m_fontSizeSubtitle = json["font_size_subtitle"].toInt(14);

    p.m_colorTitle = QColor(json["color_title"].toString("#1A237E"));
    p.m_colorBody = QColor(json["color_body"].toString("#212121"));
    p.m_colorAccent = QColor(json["color_accent"].toString("#1565C0"));
    p.m_colorTableHeader = QColor(json["color_table_header"].toString("#1565C0"));

    if (json.contains("margins")) {
        QJsonObject margins = json["margins"].toObject();
        p.m_marginTop = margins["top"].toInt(25);
        p.m_marginBottom = margins["bottom"].toInt(20);
        p.m_marginLeft = margins["left"].toInt(30);
        p.m_marginRight = margins["right"].toInt(20);
    }

    p.m_headerText = json["header_text"].toString("Memorial");
    p.m_subtitle = json["subtitle"].toString("Içamento Chiller");
    p.m_artNumber = json["art_number"].toString("1720264496217");
    p.m_riggingNumber = json["rigging_number"].toString("2607036");
    p.m_groupTitle = json["group_title"].toString("Lingada MD 45007");
    p.m_footerText = json["footer_text"].toString("");
    p.m_showLogo = json["show_logo"].toBool(true);
    p.m_showDiagram = json["show_diagram"].toBool(true);
    p.m_logoPath = json["logo_path"].toString();

    if (json.contains("document_layout")) {
        p.m_documentLayout = static_cast<DocumentLayout>(json["document_layout"].toInt(0));
    } else {
        if (p.m_name.contains("Plano de Rigging", Qt::CaseInsensitive)) {
            p.m_documentLayout = PlanoRiggingLayout;
        } else if (p.m_name.contains("Memorial", Qt::CaseInsensitive) || p.m_isDefault) {
            p.m_documentLayout = MemorialLayout;
        } else {
            p.m_documentLayout = StandardLayout;
        }
    }

    if (json.contains("plan_data")) {
        p.m_planData = RiggingPlanData::fromJson(json["plan_data"].toObject());
    }

    p.m_tableStyle = static_cast<TableStyle>(json["table_style"].toInt(0));
    p.m_tableBorders = json["table_borders"].toBool(true);
    p.m_tableAlternatingRows = json["table_alternating_rows"].toBool(true);

    return p;
}

FormatPreset FormatPreset::createPlanoRigging()
{
    FormatPreset p;
    p.m_name = "Plano de Rigging Completo";
    p.m_isDefault = false;
    p.m_favorite = true;
    p.m_documentLayout = PlanoRiggingLayout;
    p.m_fontFamily = "Arial";
    p.m_fontSizeBody = 9;
    p.m_fontSizeTitle = 14;
    p.m_fontSizeSubtitle = 10;
    p.m_colorTitle = QColor("#000000");
    p.m_colorBody = QColor("#000000");
    p.m_colorAccent = QColor("#000000");
    p.m_colorTableHeader = QColor("#000000");
    p.m_marginTop = 5;
    p.m_marginBottom = 5;
    p.m_marginLeft = 5;
    p.m_marginRight = 5;
    p.m_headerText = "PLANO DE RIGGING nº 2607036";
    p.m_subtitle = "";
    p.m_artNumber = "1720264496217";
    p.m_riggingNumber = "2607036";
    p.m_planData = RiggingPlanData::createDefault();
    return p;
}

FormatPreset FormatPreset::createPlanoRiggingResultados()
{
    FormatPreset p = createPlanoRigging();
    p.m_name = "Plano de Rigging — Resultados";
    p.m_favorite = true;
    return p;
}

FormatPreset FormatPreset::createDefault()
{
    FormatPreset p;
    p.m_name = "Memorial Guindastes Ribas";
    p.m_isDefault = true;
    p.m_favorite = true;
    p.m_documentLayout = MemorialLayout;
    p.m_fontFamily = "Arial";
    p.m_fontSizeBody = 11;
    p.m_fontSizeTitle = 16;
    p.m_fontSizeSubtitle = 12;
    p.m_colorTitle = QColor("#111827");
    p.m_colorBody = QColor("#1F2937");
    p.m_colorAccent = QColor("#0D47A1");
    p.m_colorTableHeader = QColor("#1565C0");
    p.m_marginTop = 20;
    p.m_marginBottom = 15;
    p.m_marginLeft = 20;
    p.m_marginRight = 20;
    p.m_headerText = "Memorial";
    p.m_subtitle = "Içamento Chiller";
    p.m_artNumber = "1720264496217";
    p.m_riggingNumber = "2607036";
    p.m_groupTitle = "Lingada MD 45007";
    p.m_footerText = "";
    p.m_showLogo = true;
    p.m_showDiagram = true;
    p.m_tableStyle = Modern;
    p.m_tableBorders = true;
    p.m_tableAlternatingRows = true;
    return p;
}

FormatPreset FormatPreset::createModern()
{
    FormatPreset p;
    p.m_name = "Moderno";
    p.m_documentLayout = StandardLayout;
    p.m_fontFamily = "Segoe UI";
    p.m_fontSizeBody = 11;
    p.m_fontSizeTitle = 24;
    p.m_fontSizeSubtitle = 15;
    p.m_colorTitle = QColor("#0D47A1");
    p.m_colorBody = QColor("#37474F");
    p.m_colorAccent = QColor("#00897B");
    p.m_colorTableHeader = QColor("#00897B");
    p.m_marginTop = 20;
    p.m_marginBottom = 15;
    p.m_marginLeft = 25;
    p.m_marginRight = 15;
    p.m_headerText = "Relatório de Rigging";
    p.m_footerText = "RigPlan Integrator — Documento Confidencial";
    p.m_tableStyle = Modern;
    p.m_tableBorders = false;
    p.m_tableAlternatingRows = true;
    return p;
}

FormatPreset FormatPreset::createMinimal()
{
    FormatPreset p;
    p.m_name = "Minimalista";
    p.m_documentLayout = StandardLayout;
    p.m_fontFamily = "Calibri";
    p.m_fontSizeBody = 10;
    p.m_fontSizeTitle = 16;
    p.m_fontSizeSubtitle = 12;
    p.m_colorTitle = QColor("#333333");
    p.m_colorBody = QColor("#555555");
    p.m_colorAccent = QColor("#666666");
    p.m_colorTableHeader = QColor("#888888");
    p.m_marginTop = 15;
    p.m_marginBottom = 15;
    p.m_marginLeft = 20;
    p.m_marginRight = 15;
    p.m_headerText = "Plano de Rigging";
    p.m_footerText = "";
    p.m_tableStyle = Minimal;
    p.m_tableBorders = true;
    p.m_tableAlternatingRows = false;
    return p;
}
