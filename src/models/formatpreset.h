#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QDateTime>
#include <QColor>
#include "riggingplandata.h"

/**
 * @brief Format preset defining document appearance.
 *
 * Similar to a "skin" or appearance profile — users can create,
 * save, favorite, and switch between multiple formatting presets.
 */
class FormatPreset {
public:
    FormatPreset();

    // --- Identity ---
    QString id() const { return m_id; }
    void setId(const QString& id) { m_id = id; }

    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    bool isFavorite() const { return m_favorite; }
    void setFavorite(bool fav) { m_favorite = fav; }

    bool isDefault() const { return m_isDefault; }
    void setDefault(bool def) { m_isDefault = def; }

    QDateTime createdAt() const { return m_createdAt; }
    QDateTime modifiedAt() const { return m_modifiedAt; }
    void touch() { m_modifiedAt = QDateTime::currentDateTime(); }

    // --- Typography ---
    QString fontFamily() const { return m_fontFamily; }
    void setFontFamily(const QString& f) { m_fontFamily = f; }

    int fontSizeBody() const { return m_fontSizeBody; }
    void setFontSizeBody(int s) { m_fontSizeBody = s; }

    int fontSizeTitle() const { return m_fontSizeTitle; }
    void setFontSizeTitle(int s) { m_fontSizeTitle = s; }

    int fontSizeSubtitle() const { return m_fontSizeSubtitle; }
    void setFontSizeSubtitle(int s) { m_fontSizeSubtitle = s; }

    // --- Colors ---
    QColor colorTitle() const { return m_colorTitle; }
    void setColorTitle(const QColor& c) { m_colorTitle = c; }

    QColor colorBody() const { return m_colorBody; }
    void setColorBody(const QColor& c) { m_colorBody = c; }

    QColor colorAccent() const { return m_colorAccent; }
    void setColorAccent(const QColor& c) { m_colorAccent = c; }

    QColor colorTableHeader() const { return m_colorTableHeader; }
    void setColorTableHeader(const QColor& c) { m_colorTableHeader = c; }

    // --- Margins (mm) ---
    int marginTop() const { return m_marginTop; }
    void setMarginTop(int m) { m_marginTop = m; }

    int marginBottom() const { return m_marginBottom; }
    void setMarginBottom(int m) { m_marginBottom = m; }

    int marginLeft() const { return m_marginLeft; }
    void setMarginLeft(int m) { m_marginLeft = m; }

    int marginRight() const { return m_marginRight; }
    void setMarginRight(int m) { m_marginRight = m; }

    // --- Content ---
    QString headerText() const { return m_headerText; }
    void setHeaderText(const QString& t) { m_headerText = t; }

    QString footerText() const { return m_footerText; }
    void setFooterText(const QString& t) { m_footerText = t; }

    bool showLogo() const { return m_showLogo; }
    void setShowLogo(bool s) { m_showLogo = s; }

    QString logoPath() const { return m_logoPath; }
    void setLogoPath(const QString& p) { m_logoPath = p; }

    QString subtitle() const { return m_subtitle; }
    void setSubtitle(const QString& s) { m_subtitle = s; }

    QString artNumber() const { return m_artNumber; }
    void setArtNumber(const QString& a) { m_artNumber = a; }

    QString riggingNumber() const { return m_riggingNumber; }
    void setRiggingNumber(const QString& r) { m_riggingNumber = r; }

    QString groupTitle() const { return m_groupTitle; }
    void setGroupTitle(const QString& g) { m_groupTitle = g; }

    // --- Document Layout Type ---
    enum DocumentLayout {
        MemorialLayout = 0,
        PlanoRiggingLayout = 1,
        StandardLayout = 2
    };

    DocumentLayout documentLayout() const { return m_documentLayout; }
    void setDocumentLayout(DocumentLayout l) { m_documentLayout = l; }
    bool isMemorialLayout() const { return m_documentLayout == MemorialLayout; }
    bool isPlanoRiggingLayout() const { return m_documentLayout == PlanoRiggingLayout; }

    const RiggingPlanData& planData() const { return m_planData; }
    RiggingPlanData& planData() { return m_planData; }
    void setPlanData(const RiggingPlanData& d) { m_planData = d; }

    // --- Table Style ---
    enum TableStyle { Modern, Classic, Minimal, Bordered };

    TableStyle tableStyle() const { return m_tableStyle; }
    void setTableStyle(TableStyle s) { m_tableStyle = s; }

    bool tableBorders() const { return m_tableBorders; }
    void setTableBorders(bool b) { m_tableBorders = b; }

    bool tableAlternatingRows() const { return m_tableAlternatingRows; }
    void setTableAlternatingRows(bool a) { m_tableAlternatingRows = a; }

    // --- Serialization ---
    QJsonObject toJson() const;
    static FormatPreset fromJson(const QJsonObject& json);

    // --- Factory ---
    static FormatPreset createDefault();
    static FormatPreset createPlanoRigging();
    static FormatPreset createPlanoRiggingResultados();
    static FormatPreset createModern();
    static FormatPreset createMinimal();

private:
    // Identity
    QString m_id;
    QString m_name = "Sem Nome";
    bool m_favorite = false;
    bool m_isDefault = false;
    QDateTime m_createdAt;
    QDateTime m_modifiedAt;

    // Typography
    QString m_fontFamily = "Arial";
    int m_fontSizeBody = 11;
    int m_fontSizeTitle = 20;
    int m_fontSizeSubtitle = 14;

    // Colors
    QColor m_colorTitle = QColor("#1A237E");
    QColor m_colorBody = QColor("#212121");
    QColor m_colorAccent = QColor("#1565C0");
    QColor m_colorTableHeader = QColor("#1565C0");

    // Margins (mm)
    int m_marginTop = 25;
    int m_marginBottom = 20;
    int m_marginLeft = 30;
    int m_marginRight = 20;

    // Content
    QString m_headerText = "Memorial";
    QString m_subtitle = "Içamento Chiller";
    QString m_artNumber = "1720264496217";
    QString m_riggingNumber = "2607036";
    QString m_groupTitle = "Lingada MD 45007";
    QString m_footerText = "Gerado por RigPlan Integrator";
    bool m_showLogo = true;
    bool m_showDiagram = true;
    QString m_logoPath;

    // Document Layout
    DocumentLayout m_documentLayout = MemorialLayout;
    RiggingPlanData m_planData = RiggingPlanData::createDefault();

    // Table
    TableStyle m_tableStyle = Modern;
    bool m_tableBorders = true;
    bool m_tableAlternatingRows = true;
};
