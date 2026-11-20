#include "documentgenerator.h"
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QImage>
#include <cmath>

DocumentGenerator::DocumentGenerator(QObject* parent)
    : QObject(parent)
{
}

QString DocumentGenerator::getBase64Image(const QString& qrcPath, const QString& localPath)
{
    // Try QRC first
    QFile f(qrcPath);
    if (f.open(QIODevice::ReadOnly)) {
        return "data:image/png;base64," + QString::fromLatin1(f.readAll().toBase64());
    }

    // Try relative/local path
    QFile f2(localPath);
    if (f2.open(QIODevice::ReadOnly)) {
        return "data:image/png;base64," + QString::fromLatin1(f2.readAll().toBase64());
    }

    // Try current application dir path
    QString appPath = QDir::current().filePath(localPath);
    QFile f3(appPath);
    if (f3.open(QIODevice::ReadOnly)) {
        return "data:image/png;base64," + QString::fromLatin1(f3.readAll().toBase64());
    }

    return QString();
}

static QString s_currentTextColor = "#000000";
static QString s_currentLineColor = "#000000";

QString DocumentGenerator::makeFraction(const QString& num, const QString& den,
                                        const QString& lineColor,
                                        const QString& textColor)
{
    QString lColor = (lineColor == "#000000" && !s_currentLineColor.isEmpty()) ? s_currentLineColor : lineColor;
    QString tColor = (textColor == "#000000" && !s_currentTextColor.isEmpty()) ? s_currentTextColor : textColor;
    return QString(
        "<td valign='middle'>"
        "<table border='0' cellspacing='0' cellpadding='0'>"
        "<tr><td align='center' style='border-bottom: 1.2px solid %1; padding: 0 4px; font-size: 9.5pt; color: %2; font-family: Cambria, Georgia, \"Times New Roman\", serif;'>%3</td></tr>"
        "<tr><td align='center' style='padding: 0 4px; font-size: 9.5pt; color: %2; font-family: Cambria, Georgia, \"Times New Roman\", serif;'>%4</td></tr>"
        "</table>"
        "</td>"
    ).arg(lColor, tColor, num, den);
}

static QString makeTextCell(const QString& text, const QString& textColor = "") {
    QString col = textColor.isEmpty() ? s_currentTextColor : textColor;
    return QString("<td valign='middle' style='color: %1; font-size: 10.5pt; font-family: Cambria, Georgia, \"Times New Roman\", serif;'>%2</td>")
        .arg(col, text);
}

static QString makeParenCell(const QString& paren, const QString& parenColor = "") {
    QString col = parenColor.isEmpty() ? s_currentTextColor : parenColor;
    return QString("<td valign='middle' style='color: %1; font-size: 19pt; font-family: Cambria, Georgia, serif;'>%2</td>")
        .arg(col, paren);
}

static QString makeCenteredFormula(const QString& cellsHtml) {
    return QString("<table align='center' border='0' cellspacing='0' cellpadding='0' style='margin: 3px auto;'><tr>%1</tr></table>").arg(cellsHtml);
}

QTextDocument* DocumentGenerator::generate(
    const QVector<RiggingCalculation*>& calculations,
    const FormatPreset& preset,
    const QString& fileName)
{
    auto* doc = new QTextDocument(this);
    QString html = generateHtml(calculations, preset, fileName);
    doc->setHtml(html);
    doc->setDefaultFont(QFont(preset.fontFamily(), preset.fontSizeBody()));
    return doc;
}

QString DocumentGenerator::generateHtml(
    const QVector<RiggingCalculation*>& calculations,
    const FormatPreset& preset,
    const QString& fileName)
{
    s_currentTextColor = preset.colorBody().name();
    s_currentLineColor = preset.colorAccent().name();

    QString html;
    html += "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<style>";

    // Styles for viewing and printing
    bool isMemorial = preset.isMemorialLayout() || preset.name().contains("Memorial", Qt::CaseInsensitive);
    if (isMemorial) {
        html += QString(
            "body { font-family: '%1', Arial, Helvetica, sans-serif; font-size: %2pt; "
            "background-color: #FFFFFF; color: %3; margin: 0; padding: 6mm 10mm 6mm 10mm; line-height: 1.20; } "
        ).arg(preset.fontFamily())
         .arg(preset.fontSizeBody())
         .arg(preset.colorBody().name());
    } else {
        html += QString(
            "body { font-family: '%1', Arial, Helvetica, sans-serif; font-size: %2pt; "
            "background-color: #FFFFFF; color: %3; margin: 0; padding: %4mm %5mm %6mm %7mm; line-height: 1.45; } "
        ).arg(preset.fontFamily())
         .arg(preset.fontSizeBody())
         .arg(preset.colorBody().name())
         .arg(preset.marginTop())
         .arg(preset.marginRight())
         .arg(preset.marginBottom())
         .arg(preset.marginLeft());
    }

    html += ".page-container { "
            "background-color: #FFFFFF; "
            "width: 100%; "
            "margin: 0; "
            "padding: 0; } ";

    html += QString(".header-table { width: 100%; border-bottom: 2px solid %1; padding-bottom: 2px; margin-bottom: 6px; } ")
            .arg(preset.colorAccent().name());
    html += QString(".header-title { font-size: %1pt; font-weight: bold; color: %2; text-align: center; } ")
            .arg(preset.fontSizeTitle())
            .arg(preset.colorTitle().name());
    html += QString(".header-subtitle { font-size: %1pt; font-weight: bold; color: %2; text-align: center; margin-top: 2px; } ")
            .arg(preset.fontSizeSubtitle())
            .arg(preset.colorBody().name());

    html += QString(".section-title { font-size: %1pt; font-weight: bold; text-decoration: underline; color: %2; margin-top: 8px; margin-bottom: 2px; text-align: left; } ")
            .arg(preset.fontSizeSubtitle() > 0 ? preset.fontSizeSubtitle() : 11)
            .arg(preset.colorTitle().name());
    html += QString(".calc-desc { font-size: %1pt; color: %2; margin: 2px 0; text-align: center; line-height: 1.25; } ")
            .arg(preset.fontSizeBody())
            .arg(preset.colorBody().name());
    html += QString(".formula-line-center { font-size: %1pt; color: %2; margin: 2px auto; text-align: center; font-family: Cambria, Georgia, '%3', serif; } ")
            .arg(preset.fontSizeBody() > 0 ? (preset.fontSizeBody() + 0.5) : 10.5)
            .arg(preset.colorBody().name())
            .arg(preset.fontFamily());

    html += QString(".page-footer { text-align: center; font-size: 9.5pt; color: %1; margin-top: 16px; padding-top: 4px; } ")
            .arg(preset.colorBody().name());

    // Table styles
    html += tableStyleCss(preset);

    html += "@media print { "
            "body { background-color: #FFFFFF; padding: 0; } "
            ".page-container { border: none; box-shadow: none; margin: 0; padding: 5mm; page-break-after: always; } "
            "} ";

    html += "</style></head><body>";

    if (preset.isPlanoRiggingLayout() || preset.name().contains("Plano de Rigging", Qt::CaseInsensitive)) {
        html += buildPlanoRiggingPage(calculations, preset);
    } else if (preset.isMemorialLayout() || preset.name().contains("Memorial", Qt::CaseInsensitive)) {
        html += buildMemorialPages(calculations, preset);
    } else {
        html += buildStandardDocument(calculations, preset, fileName);
    }

    html += "</body></html>";
    return html;
}

QTextDocument* DocumentGenerator::generateContinuous(
    const QVector<DocumentSection>& sections,
    const QVector<RiggingCalculation*>& calculations,
    const QString& fileName)
{
    auto* doc = new QTextDocument(this);
    QString html = generateContinuousHtml(sections, calculations, fileName);
    doc->setHtml(html);

    // Default font from first preset section or default
    FormatPreset basePreset = FormatPreset::createDefault();
    for (const auto& s : sections) {
        if (s.isEnabled() && s.isPreset()) {
            basePreset = s.preset();
            break;
        }
    }
    doc->setDefaultFont(QFont(basePreset.fontFamily(), basePreset.fontSizeBody()));
    return doc;
}

QString DocumentGenerator::generateContinuousHtml(
    const QVector<DocumentSection>& sections,
    const QVector<RiggingCalculation*>& calculations,
    const QString& fileName)
{
    QVector<DocumentSection> activeSections;
    for (const auto& s : sections) {
        if (s.isEnabled()) activeSections.append(s);
    }

    if (activeSections.isEmpty()) {
        FormatPreset def = FormatPreset::createDefault();
        return generateHtml(calculations, def, fileName);
    }

    // Determine baseline styling from first preset
    FormatPreset basePreset = FormatPreset::createDefault();
    for (const auto& s : activeSections) {
        if (s.isPreset()) {
            basePreset = s.preset();
            break;
        }
    }

    s_currentTextColor = basePreset.colorBody().name();
    s_currentLineColor = basePreset.colorAccent().name();

    QString html;
    html += "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<style>";

    // Detect if we have a Memorial section for compact styles
    bool hasMemorial = false;
    for (const auto& s : activeSections) {
        if (s.isPreset() && (s.preset().isMemorialLayout() || s.preset().name().contains("Memorial", Qt::CaseInsensitive))) {
            hasMemorial = true;
            break;
        }
    }

    // Styles for viewing and printing — use compact padding/line-height for Memorial
    if (hasMemorial) {
        html += QString(
            "body { font-family: '%1', Arial, Helvetica, sans-serif; font-size: %2pt; "
            "background-color: #FFFFFF; color: %3; margin: 0; padding: 6mm 10mm 6mm 10mm; line-height: 1.20; } "
        ).arg(basePreset.fontFamily())
         .arg(basePreset.fontSizeBody())
         .arg(basePreset.colorBody().name());
    } else {
        html += QString(
            "body { font-family: '%1', Arial, Helvetica, sans-serif; font-size: %2pt; "
            "background-color: #FFFFFF; color: %3; margin: 0; padding: %4mm %5mm %6mm %7mm; line-height: 1.45; } "
        ).arg(basePreset.fontFamily())
         .arg(basePreset.fontSizeBody())
         .arg(basePreset.colorBody().name())
         .arg(basePreset.marginTop())
         .arg(basePreset.marginRight())
         .arg(basePreset.marginBottom())
         .arg(basePreset.marginLeft());
    }

    html += ".page-container { "
            "background-color: #FFFFFF; "
            "width: 100%; "
            "margin: 0; "
            "padding: 0; } ";

    html += QString(".header-table { width: 100%; border-bottom: 2px solid %1; padding-bottom: 2px; margin-bottom: 6px; } ")
            .arg(basePreset.colorAccent().name());
    html += QString(".header-title { font-size: %1pt; font-weight: bold; color: %2; text-align: center; } ")
            .arg(basePreset.fontSizeTitle())
            .arg(basePreset.colorTitle().name());
    html += QString(".header-subtitle { font-size: %1pt; font-weight: bold; color: %2; text-align: center; margin-top: 2px; } ")
            .arg(basePreset.fontSizeSubtitle())
            .arg(basePreset.colorBody().name());

    html += QString(".section-title { font-size: %1pt; font-weight: bold; text-decoration: underline; color: %2; margin-top: 8px; margin-bottom: 2px; text-align: left; } ")
            .arg(basePreset.fontSizeSubtitle() > 0 ? basePreset.fontSizeSubtitle() : 11)
            .arg(basePreset.colorTitle().name());
    html += QString(".calc-desc { font-size: %1pt; color: %2; margin: 2px 0; text-align: center; line-height: 1.25; } ")
            .arg(basePreset.fontSizeBody())
            .arg(basePreset.colorBody().name());
    html += QString(".formula-line-center { font-size: %1pt; color: %2; margin: 2px auto; text-align: center; font-family: Cambria, Georgia, '%3', serif; } ")
            .arg(basePreset.fontSizeBody() > 0 ? (basePreset.fontSizeBody() + 0.5) : 10.5)
            .arg(basePreset.colorBody().name())
            .arg(basePreset.fontFamily());

    html += QString(".page-footer { text-align: center; font-size: 9.5pt; color: %1; margin-top: 16px; padding-top: 4px; } ")
            .arg(basePreset.colorBody().name());

    html += tableStyleCss(basePreset);

    // Continuous page break & visual divider
    html += ".continuous-page-break { "
            "   page-break-after: always; "
            "   height: 1px; "
            "   margin: 40px 0 25px 0; "
            "   border-bottom: 2px dashed #94A3B8; "
            "} ";

    html += ".continuous-section-banner { "
            "   background-color: #1E293B; "
            "   color: #38BDF8; "
            "   font-size: 10.5pt; "
            "   font-weight: bold; "
            "   padding: 5px 12px; "
            "   border-radius: 4px; "
            "   margin-bottom: 12px; "
            "   text-align: left; "
            "} ";

    html += "@media print { "
            "body { background-color: #FFFFFF; padding: 0; } "
            ".page-container { border: none; box-shadow: none; margin: 0; padding: 5mm; page-break-after: always; } "
            ".continuous-page-break { border: none; margin: 0; height: 0; } "
            ".continuous-section-banner { display: none; } "
            "} ";

    html += "</style></head><body>";

    const int totalActive = activeSections.size();
    for (int i = 0; i < totalActive; ++i) {
        const auto& sec = activeSections[i];

        // Banner removed from export — only used for UI preview via CSS
        // (The continuous-section-banner class is kept in CSS for potential UI-only usage)

        if (sec.isPreset()) {
            FormatPreset p = sec.preset();
            if (p.isPlanoRiggingLayout() || p.name().contains("Plano de Rigging", Qt::CaseInsensitive)) {
                html += buildPlanoRiggingPage(calculations, p);
            } else if (p.isMemorialLayout() || p.name().contains("Memorial", Qt::CaseInsensitive)) {
                html += buildMemorialPages(calculations, p);
            } else {
                html += buildStandardDocument(calculations, p, fileName);
            }
        } else {
            // CREA, SOLIDWORKS, or Custom Image
            html += buildImagePage(sec, i + 1, totalActive, basePreset);
        }

        if (i < totalActive - 1) {
            html += "<div class='continuous-page-break'></div>";
        }
    }

    html += "</body></html>";
    return html;
}

QString DocumentGenerator::generateSectionHtml(
    const DocumentSection& section,
    const QVector<RiggingCalculation*>& calculations,
    const QString& fileName,
    const FormatPreset& basePreset,
    int pageNum, int totalPages)
{
    s_currentTextColor = basePreset.colorBody().name();
    s_currentLineColor = basePreset.colorAccent().name();

    QString html;
    html += "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<style>";

    bool isRigging = section.isPreset() &&
        (section.preset().isPlanoRiggingLayout() || section.preset().name().contains("Plano de Rigging", Qt::CaseInsensitive));
    bool isSectionMemorial = section.isPreset() &&
        (section.preset().isMemorialLayout() || section.preset().name().contains("Memorial", Qt::CaseInsensitive));

    if (isRigging) {
        html += QString(
            "body { font-family: '%1', Arial, Helvetica, sans-serif; font-size: 7.5pt; "
            "background-color: #FFFFFF; color: %2; margin: 0; padding: 0; line-height: 1.05; } "
            "table { border-collapse: collapse; } "
            "td { padding: 0px 2px; } "
        ).arg(basePreset.fontFamily())
         .arg(basePreset.colorBody().name());
    } else if (isSectionMemorial) {
        html += QString(
            "body { font-family: '%1', Arial, Helvetica, sans-serif; font-size: %2pt; "
            "background-color: #FFFFFF; color: %3; margin: 0; padding: 6mm 10mm 6mm 10mm; line-height: 1.20; } "
        ).arg(basePreset.fontFamily())
         .arg(basePreset.fontSizeBody())
         .arg(basePreset.colorBody().name());
    } else if (!section.isPreset()) {
        // Image section: CREA, SolidWorks, CustomImage
        // Use compact padding so the full A4 page can host header, image, and footer on a single page
        html += QString(
            "body { font-family: '%1', Arial, Helvetica, sans-serif; font-size: 8pt; "
            "background-color: #FFFFFF; color: %2; margin: 0; padding: 3mm 6mm 3mm 6mm; line-height: 1.15; } "
        ).arg(basePreset.fontFamily())
         .arg(basePreset.colorBody().name());
    } else {
        html += QString(
            "body { font-family: '%1', Arial, Helvetica, sans-serif; font-size: %2pt; "
            "background-color: #FFFFFF; color: %3; margin: 0; padding: %4mm %5mm %6mm %7mm; line-height: 1.45; } "
        ).arg(basePreset.fontFamily())
         .arg(basePreset.fontSizeBody())
         .arg(basePreset.colorBody().name())
         .arg(basePreset.marginTop()).arg(basePreset.marginRight()).arg(basePreset.marginBottom()).arg(basePreset.marginLeft());
    }

    html += ".page-container { background-color: #FFFFFF; width: 100%; margin: 0; padding: 0; } ";

    html += QString(".header-table { width: 100%; border-bottom: 2px solid %1; padding-bottom: 2px; margin-bottom: 6px; } ")
            .arg(basePreset.colorAccent().name());
    html += QString(".header-title { font-size: %1pt; font-weight: bold; color: %2; text-align: center; } ")
            .arg(basePreset.fontSizeTitle())
            .arg(basePreset.colorTitle().name());
    html += QString(".header-subtitle { font-size: %1pt; font-weight: bold; color: %2; text-align: center; margin-top: 2px; } ")
            .arg(basePreset.fontSizeSubtitle())
            .arg(basePreset.colorBody().name());

    html += QString(".section-title { font-size: %1pt; font-weight: bold; text-decoration: underline; color: %2; margin-top: 8px; margin-bottom: 2px; text-align: left; } ")
            .arg(basePreset.fontSizeSubtitle() > 0 ? basePreset.fontSizeSubtitle() : 11)
            .arg(basePreset.colorTitle().name());
    html += QString(".calc-desc { font-size: %1pt; color: %2; margin: 2px 0; text-align: center; line-height: 1.25; } ")
            .arg(basePreset.fontSizeBody())
            .arg(basePreset.colorBody().name());
    html += QString(".formula-line-center { font-size: %1pt; color: %2; margin: 2px auto; text-align: center; font-family: Cambria, Georgia, '%3', serif; } ")
            .arg(basePreset.fontSizeBody() > 0 ? (basePreset.fontSizeBody() + 0.5) : 10.5)
            .arg(basePreset.colorBody().name())
            .arg(basePreset.fontFamily());

    html += QString(".page-footer { text-align: center; font-size: 9.5pt; color: %1; margin-top: 16px; padding-top: 4px; } ")
            .arg(basePreset.colorBody().name());

    html += tableStyleCss(basePreset);

    html += "</style></head><body>";

    if (section.isPreset()) {
        FormatPreset p = section.preset();
        if (p.isPlanoRiggingLayout() || p.name().contains("Plano de Rigging", Qt::CaseInsensitive)) {
            html += buildPlanoRiggingPage(calculations, p);
        } else if (p.isMemorialLayout() || p.name().contains("Memorial", Qt::CaseInsensitive)) {
            html += buildMemorialPages(calculations, p);
        } else {
            html += buildStandardDocument(calculations, p, fileName);
        }
    } else {
        // CREA, SOLIDWORKS, or Custom Image
        html += buildImagePage(section, pageNum, totalPages, basePreset);
    }

    html += "</body></html>";
    return html;
}

QTextDocument* DocumentGenerator::generateSectionDocument(
    const DocumentSection& section,
    const QVector<RiggingCalculation*>& calculations,
    const QString& fileName,
    const FormatPreset& basePreset,
    int pageNum, int totalPages)
{
    auto* doc = new QTextDocument(this);
    doc->setDocumentMargin(0);
    QString html = generateSectionHtml(section, calculations, fileName, basePreset, pageNum, totalPages);
    doc->setHtml(html);

    FormatPreset effectivePreset = section.isPreset() ? section.preset() : basePreset;
    doc->setDefaultFont(QFont(effectivePreset.fontFamily(), effectivePreset.fontSizeBody()));
    return doc;
}

QString DocumentGenerator::buildImagePage(
    const DocumentSection& section, int pageNum, int totalPages,
    const FormatPreset& fallbackPreset)
{
    QString logoB64 = getBase64Image(":/images/image3.png", "resources/images/image3.png");
    QString imgB64 = section.effectiveImageBase64();

    QString sectionTitle = section.title();
    if (sectionTitle.isEmpty()) {
        if (section.type() == DocumentSection::CreaImage) sectionTitle = "ANOTAÇÃO DE RESPONSABILIDADE TÉCNICA - CREA";
        else if (section.type() == DocumentSection::SolidworksImage) sectionTitle = "DESENHO TÉCNICO DE RIGGING - SOLIDWORKS";
        else sectionTitle = "ANEXO TÉCNICO";
    }

    QString subTitle = (section.type() == DocumentSection::CreaImage)
        ? "Documento de Fiscalização e Responsabilidade Técnica de Engenharia"
        : ((section.type() == DocumentSection::SolidworksImage)
            ? "Arranjo Geral e Estudo Tridimensional de Içamento"
            : "Anexo Técnico Complementar do Plano de Rigging");

    QString art = fallbackPreset.artNumber().trimmed().isEmpty() ? "1720264496217" : fallbackPreset.artNumber();
    QString rig = fallbackPreset.riggingNumber().trimmed().isEmpty() ? "2607036" : fallbackPreset.riggingNumber();

    QString html;
    html += "<div class='page-container'>";

    // Header Table (Compact to fit on a single page)
    html += "<table width='100%' class='header-table' border='0' cellspacing='0' cellpadding='0' style='width: 100%; border-bottom: 2px solid #1565C0; padding-bottom: 3px; margin-bottom: 6px;'>";
    html += "<tr>";
    html += "<td width='25%' align='left' valign='middle' style='width: 25%;'>";
    if (!logoB64.isEmpty()) {
        html += QString("<img src='%1' height='36' style='vertical-align: middle;' alt='GUINDASTES RIBAS' />").arg(logoB64);
    } else {
        html += "<div style='font-size: 11pt; font-weight: bold; color: #1565C0;'>GUINDASTES RIBAS</div>";
    }
    html += "</td>";
    html += "<td width='50%' align='center' valign='middle' style='width: 50%; text-align: center;'>";
    html += QString("<div class='header-title' style='font-size: 12pt; font-weight: bold; color: #1565C0; text-align: center;'>%1</div>").arg(sectionTitle.toHtmlEscaped());
    html += QString("<div class='header-subtitle' style='font-size: 9pt; font-weight: bold; color: #333333; text-align: center; margin-top: 2px;'>%1</div>").arg(subTitle.toHtmlEscaped());
    html += "</td>";
    html += "<td width='25%' align='right' valign='middle' style='width: 25%; text-align: right;'>";
    html += "<table border='0' cellspacing='0' cellpadding='0' align='right' style='border-left: 2px solid #1565C0; padding-left: 6px; text-align: center;'>";
    html += QString("<tr><td style='font-size: 8pt; font-weight: bold; color: #1565C0; padding: 0 4px;'>ART Nº: %1</td></tr>").arg(art.toHtmlEscaped());
    html += QString("<tr><td style='font-size: 8pt; font-weight: bold; color: #333333; padding: 0 4px;'>RIGGING Nº: %1</td></tr>").arg(rig.toHtmlEscaped());
    html += "</table>";
    html += "</td>";
    html += "</tr></table>";

    // Image Body
    html += "<div style='text-align: center; margin: 4px auto;'>";
    if (!imgB64.isEmpty()) {
        // Compute target dimensions so that the image fits perfectly within a single A4 page
        // without overflowing into a second page or causing a page break.
        QSize imgSize;
        if (!section.imagePath().isEmpty() && QFile::exists(section.imagePath())) {
            QImageReader reader(section.imagePath());
            imgSize = reader.size();
        }
        if (!imgSize.isValid() || imgSize.width() <= 0 || imgSize.height() <= 0) {
            int commaIdx = imgB64.indexOf(',');
            if (commaIdx != -1) {
                QByteArray raw = QByteArray::fromBase64(imgB64.mid(commaIdx + 1).toLatin1());
                QImage tmpImg = QImage::fromData(raw);
                imgSize = tmpImg.size();
            }
        }

        const double maxImgWidth = 530.0;
        const double maxImgHeight = 620.0;
        int targetW = 530;
        int targetH = 600;

        if (imgSize.isValid() && imgSize.width() > 0 && imgSize.height() > 0) {
            double scale = qMin(maxImgWidth / imgSize.width(), maxImgHeight / imgSize.height());
            targetW = qRound(imgSize.width() * scale);
            targetH = qRound(imgSize.height() * scale);
        }

        html += QString("<img src='%1' width='%2' height='%3' style='display: block; margin: 0 auto; border: 1px solid #94A3B8; border-radius: 4px; box-shadow: 0 4px 10px rgba(0,0,0,0.12);' />")
                    .arg(imgB64).arg(targetW).arg(targetH);
    } else {
        html += "<div style='border: 2px dashed #94A3B8; border-radius: 8px; background-color: #F8FAFC; padding: 60px 20px; text-align: center; color: #64748B;'>";
        html += "<div style='font-size: 32px; margin-bottom: 6px;'>📷</div>";
        html += QString("<div style='font-size: 12pt; font-weight: bold; color: #334155;'>Nenhuma Imagem Importada para %1</div>").arg(sectionTitle.toHtmlEscaped());
        html += "<div style='font-size: 9.5pt; color: #64748B; margin-top: 4px;'>Utilize o botão de importação na barra de pré-visualização para carregar o arquivo (PNG, JPG, BMP).</div>";
        html += "</div>";
    }
    html += "</div>";

    // Footer
    html += QString("<div style='text-align: center; font-size: 8pt; color: #64748B; border-top: 1px solid #CBD5E1; padding-top: 4px; margin-top: 6px;'>")
         + QString("Anexo Integrante do Plano de Rigging nº %1 — %2 — Página %3 de %4")
             .arg(rig.toHtmlEscaped(), sectionTitle.toHtmlEscaped()).arg(pageNum).arg(totalPages)
         + "</div>";

    html += "</div>";
    return html;
}

QString DocumentGenerator::buildPageHeader(const FormatPreset& preset)
{
    QString logoB64 = getBase64Image(":/images/image3.png", "resources/images/image3.png");

    QString title = preset.headerText().trimmed().isEmpty() ? "Memorial" : preset.headerText().trimmed();
    QString subtitle = preset.subtitle().trimmed();
    QString art = preset.artNumber().trimmed().isEmpty() ? "1720264496217" : preset.artNumber().trimmed();
    QString rigging = preset.riggingNumber().trimmed().isEmpty() ? "2607036" : preset.riggingNumber().trimmed();

    QString html;
    html += QString("<table width='100%' class='header-table' border='0' cellspacing='0' cellpadding='0' style='width: 100%; border-bottom: 2px solid %1; padding-bottom: 4px; margin-bottom: 12px;'>")
            .arg(preset.colorAccent().name());
    html += "<tr>";

    // Column 1: Logo (Left)
    html += "<td width='25%' align='left' valign='middle' style='width: 25%; vertical-align: middle; text-align: left;'>";
    if (preset.showLogo() && !logoB64.isEmpty()) {
        html += QString("<img src='%1' style='max-height: 48px; max-width: 160px;' alt='GUINDASTES RIBAS' />").arg(logoB64);
    } else if (preset.showLogo()) {
        html += QString("<div style='font-size: 12pt; font-weight: bold; color: %1;'>GUINDASTES RIBAS</div>")
                .arg(preset.colorTitle().name());
    }
    html += "</td>";

    // Column 2: Center Titles (EXACT MEIO / CENTRALIZADO)
    html += "<td width='50%' align='center' valign='middle' style='width: 50%; vertical-align: middle; text-align: center;'>";
    html += QString("<div class='header-title' style='font-size: %1pt; font-weight: bold; color: %2; text-align: center;'>%3</div>")
            .arg(preset.fontSizeTitle())
            .arg(preset.colorTitle().name())
            .arg(title.toHtmlEscaped());
    if (!subtitle.isEmpty()) {
        html += QString("<div class='header-subtitle' style='font-size: %1pt; font-weight: bold; color: %2; text-align: center; margin-top: 2px;'>%3</div>")
                .arg(preset.fontSizeSubtitle())
                .arg(preset.colorBody().name())
                .arg(subtitle.toHtmlEscaped());
    }
    html += "</td>";

    // Column 3: ART & RIGGING Box (EXACT RIGHT / DIREITA)
    html += "<td width='25%' align='right' valign='middle' style='width: 25%; vertical-align: middle; text-align: right;'>";
    html += QString("<table border='0' cellspacing='0' cellpadding='0' align='right' style='border-left: 2px solid %1; padding-left: 10px; text-align: center;'>")
            .arg(preset.colorAccent().name());
    html += QString("<tr><td align='center' style='font-size: 8pt; font-weight: bold; color: %1; text-align: center;'>ART</td></tr>")
            .arg(preset.colorTitle().name());
    html += QString("<tr><td align='center' style='font-size: 9pt; font-weight: bold; color: %1; text-align: center;'>%2</td></tr>")
            .arg(preset.colorBody().name(), art.toHtmlEscaped());
    html += QString("<tr><td style='border-top: 1px solid %1; padding: 1px 0;'></td></tr>")
            .arg(preset.colorAccent().name());
    html += QString("<tr><td align='center' style='font-size: 8pt; font-weight: bold; color: %1; text-align: center;'>RIGGING</td></tr>")
            .arg(preset.colorTitle().name());
    html += QString("<tr><td align='center' style='font-size: 9pt; font-weight: bold; color: %1; text-align: center;'>%2</td></tr>")
            .arg(preset.colorBody().name(), rigging.toHtmlEscaped());
    html += "</table>";
    html += "</td>";

    html += "</tr>";
    html += "</table>";

    return html;
}

QString DocumentGenerator::buildPageFooter(int pageNum)
{
    return QString("<div class='page-footer'>%1</div>").arg(pageNum);
}

QString DocumentGenerator::buildMemorialPages(
    const QVector<RiggingCalculation*>& calculations,
    const FormatPreset& preset)
{
    QString html;

    // Map calculations by ID
    QMap<int, const RiggingCalculation*> calcMap;
    for (const auto* c : calculations) {
        calcMap[c->id()] = c;
    }

    auto getCalc = [&](int id) -> const RiggingCalculation* {
        return calcMap.value(id, nullptr);
    };

    auto formatDesc = [](const QString& rawDesc, const QString& defaultDesc) -> QString {
        QString d = rawDesc.trimmed();
        if (d.isEmpty()) return defaultDesc;
        if (d.contains("<") && d.contains(">")) {
            return d;
        }
        return d.toHtmlEscaped().replace("\n", "<br>");
    };

    // ==========================================
    // PAGE 1: Header + Calcs 1, 2, 3
    // ==========================================
    html += "<div class='page-container'>";
    html += buildPageHeader(preset);

    // 1. Cinta tubular 3m x 6t
    const auto* c1 = getCalc(1);
    double c1_alpha = c1 ? c1->parameterValue(0) : 28.0;
    double c1_pa = c1 ? c1->parameterValue(1) : 3.45;
    double c1_te = c1 && c1->isValid() ? c1->result() : 1.95;
    double c1_cap = c1 ? c1->parameterValue(2) : 4.2;
    if (c1_cap <= 0) c1_cap = 4.2;
    double c1_tu = (c1_cap > 0) ? (c1_te / c1_cap) : 0.46;
    int c1_tu_pct = std::round(c1_tu * 100.0);
    QString c1_desc = formatDesc(c1 ? c1->description() : "", "Considerando, no máximo, <b>85% de taxa de utilização</b> (TU).");

    html += QString("<div class='section-title'>1. %1</div>").arg(c1 ? c1->name() : "Cinta tubular 3m x 6t");

    QString c1_f1_cells;
    c1_f1_cells += makeTextCell("<i>Te</i> =&nbsp;");
    c1_f1_cells += makeFraction("1", "cos &alpha;");
    c1_f1_cells += makeTextCell("&nbsp;&middot; ( <i>Pa</i> ) &rarr; <i>Te</i> =&nbsp;");
    c1_f1_cells += makeFraction("1", QString("cos %1&deg;").arg(c1_alpha, 0, 'f', 0));
    c1_f1_cells += makeTextCell("&nbsp;&middot;&nbsp;");
    c1_f1_cells += makeParenCell("(");
    c1_f1_cells += makeFraction(QString::number(c1_pa, 'f', 2).replace('.', ','), "2");
    c1_f1_cells += makeParenCell(")");
    c1_f1_cells += makeTextCell(QString("&nbsp;t &rarr; <i>Te</i> = <b>%1 t</b>").arg(QString::number(c1_te, 'f', 2).replace('.', ',')));
    html += makeCenteredFormula(c1_f1_cells);

    html += QString("<div class='calc-desc'>%1</div>").arg(c1_desc);

    QString c1_f2_cells;
    c1_f2_cells += makeTextCell("<i>TU</i><sub>m&aacute;x</sub> =&nbsp;");
    c1_f2_cells += makeFraction(QString::number(c1_te, 'f', 2).replace('.', ','),
                                QString::number(c1_cap, 'f', 1).replace('.', ','));
    c1_f2_cells += makeTextCell(QString("&nbsp;= %1 &cong; <b>%2%</b>")
                                .arg(QString::number(c1_tu, 'f', 2).replace('.', ','))
                                .arg(c1_tu_pct));
    html += makeCenteredFormula(c1_f2_cells);

    // 2. Cinta tubular 4m x 6t
    const auto* c2 = getCalc(2);
    double c2_alpha = c2 ? c2->parameterValue(0) : 41.0;
    double c2_pa = c2 ? c2->parameterValue(1) : 3.70;
    double c2_te = c2 && c2->isValid() ? c2->result() : 2.50;
    double c2_cap = c2 ? c2->parameterValue(2) : 4.2;
    if (c2_cap <= 0) c2_cap = 4.2;
    double c2_tu = (c2_cap > 0) ? (c2_te / c2_cap) : 0.60;
    int c2_tu_pct = std::round(c2_tu * 100.0);
    QString c2_desc = formatDesc(c2 ? c2->description() : "", "Considerando, no máximo, <b>85% de taxa de utilização</b> (TU).");

    QString c2_pa_expr = (std::abs(c2_pa - 3.70) < 0.01)
                             ? QString("3,45 + 0,25")
                             : QString::number(c2_pa, 'f', 2).replace('.', ',');

    html += QString("<div class='section-title'>2. %1</div>").arg(c2 ? c2->name() : "Cinta tubular 4m x 6t");

    QString c2_f1_cells;
    c2_f1_cells += makeTextCell("<i>Te</i> =&nbsp;");
    c2_f1_cells += makeFraction("1", "cos &alpha;");
    c2_f1_cells += makeTextCell("&nbsp;&middot; ( <i>Pa</i> ) &rarr; <i>Te</i> =&nbsp;");
    c2_f1_cells += makeFraction("1", QString("cos %1&deg;").arg(c2_alpha, 0, 'f', 0));
    c2_f1_cells += makeTextCell("&nbsp;&middot;&nbsp;");
    c2_f1_cells += makeParenCell("(");
    c2_f1_cells += makeFraction(c2_pa_expr, "2");
    c2_f1_cells += makeParenCell(")");
    c2_f1_cells += makeTextCell(QString("&nbsp;t &rarr; <i>Te</i> = <b>%1 t</b>").arg(QString::number(c2_te, 'f', 1).replace('.', ',')));
    html += makeCenteredFormula(c2_f1_cells);

    html += QString("<div class='calc-desc'>%1</div>").arg(c2_desc);

    QString c2_f2_cells;
    c2_f2_cells += makeTextCell("<i>TU</i><sub>m&aacute;x</sub> =&nbsp;");
    c2_f2_cells += makeFraction(QString::number(c2_te, 'f', 1).replace('.', ','),
                                QString::number(c2_cap, 'f', 1).replace('.', ','));
    c2_f2_cells += makeTextCell(QString("&nbsp;= %1 &cong; <b>%2%</b>")
                                .arg(QString::number(c2_tu, 'f', 2).replace('.', ','))
                                .arg(c2_tu_pct));
    html += makeCenteredFormula(c2_f2_cells);

    // 3. Manilhas (carga x balancim)
    const auto* c3 = getCalc(3);
    double c3_te = c3 ? c3->parameterValue(0) : 0.98;
    double c3_cm = c3 ? c3->parameterValue(1) : 5.5;
    double c3_tu = (c3_cm > 0) ? (c3_te / c3_cm) : 0.18;
    int c3_tu_pct = std::round(c3_tu * 100.0);
    QString c3_desc = formatDesc(c3 ? c3->description() : "",
                                 "Considerando, no máximo, <b>85% de taxa de utilização</b> (TU)<br>"
                                 "Da tabela de manilhas curvas selecionada <b>Ø 7/8” Grau 6 = 5,5 tf</b>");

    html += QString("<div class='section-title'>3. %1</div>").arg(c3 ? c3->name() : "Manilhas (carga x balancim)");
    html += QString("<div class='calc-desc'>%1</div>").arg(c3_desc);

    QString c3_f_cells;
    c3_f_cells += makeTextCell("<i>TU</i> =&nbsp;");
    c3_f_cells += makeFraction(QString::number(c3_te, 'f', 2).replace('.', ','),
                               QString::number(c3_cm, 'f', 1).replace('.', ','));
    c3_f_cells += makeTextCell(QString("&nbsp;= %1 &cong; <b>%2%</b>")
                               .arg(QString::number(c3_tu, 'f', 2).replace('.', ','))
                               .arg(c3_tu_pct));
    html += makeCenteredFormula(c3_f_cells);

    // 4. Manilhas (balancim x gancho)
    const auto* c4 = getCalc(4);
    double c4_te = c4 ? c4->parameterValue(0) : 5.0;
    double c4_cm = c4 ? c4->parameterValue(1) : 7.2;
    double c4_tu = (c4_cm > 0) ? (c4_te / c4_cm) : 0.70;
    int c4_tu_pct = std::round(c4_tu * 100.0);
    QString c4_desc = formatDesc(c4 ? c4->description() : "",
                                 "Considerando, no máximo, <b>85% de taxa de utilização</b> (TU)<br>"
                                 "Da tabela de manilhas curvas selecionada <b>Ø 1” Grau 6 = 7,2 tf</b>");

    html += QString("<div class='section-title'>4. %1</div>").arg(c4 ? c4->name() : "Manilhas (balancim x gancho)");
    html += QString("<div class='calc-desc'>%1</div>").arg(c4_desc);

    QString c4_f_cells;
    c4_f_cells += makeTextCell("<i>TU</i> =&nbsp;");
    c4_f_cells += makeFraction(QString::number(c4_te, 'f', (c4_te == std::floor(c4_te) ? 0 : 1)).replace('.', ','),
                               QString::number(c4_cm, 'f', 1).replace('.', ','));
    c4_f_cells += makeTextCell(QString("&nbsp;= %1 &cong; <b>%2%</b>")
                               .arg(QString::number(c4_tu, 'f', 2).replace('.', ','))
                               .arg(c4_tu_pct));
    html += makeCenteredFormula(c4_f_cells);

    html += "</div>"; // End Page 1 container

    // ==========================================
    // PAGE 2: Header + Calcs 5, 6, 7, 8
    // ==========================================
    html += "<div class='page-container' style='page-break-before: always;'>";
    html += buildPageHeader(preset);

    // 5. Esforço nas sapatas
    const auto* c5 = getCalc(5);
    double c5_a = c5 ? c5->parameterValue(0) : 2.95;
    double c5_b = c5 ? c5->parameterValue(1) : 5.70;
    double c5_ro = c5 ? c5->parameterValue(2) : 5.0;
    double c5_pc = c5 ? c5->parameterValue(3) : 3700.0;
    double c5_pg = c5 ? c5->parameterValue(4) : 11550.0;
    double c5_m = c5_pc * c5_ro;
    double c5_f = (c5_a + c5_b > 0) ? (c5_m / (c5_a + c5_b)) : 2139.0;
    double c5_es = (c5_pg / 2.0) + c5_f;
    double c5_es_tf = c5_es / 1000.0;

    QString c5_pg_formatted = (c5_pg >= 1000)
        ? QString("%1.%2").arg((int)c5_pg / 1000).arg((int)c5_pg % 1000, 3, 10, QChar('0'))
        : QString::number(c5_pg, 'f', 0);

    QString c5_defaultDesc = QString("Distância eixo da coluna → sapata esquerda: a=%1 m<br>"
                                     "Abertura entre sapatas: b=%2 m<br>"
                                     "Raio da operação: Ro=%3 m<br>"
                                     "Carga içada: Pc=%4 kgf<br>"
                                     "Peso do guindaste + caminhão: Pg=%5 kgf")
                                 .arg(QString::number(c5_a, 'f', 2).replace('.', ','))
                                 .arg(QString::number(c5_b, 'f', 1).replace('.', ','))
                                 .arg(QString::number(c5_ro, 'f', 1).replace('.', ','))
                                 .arg(QString::number(c5_pc, 'f', 0))
                                 .arg(c5_pg_formatted);
    QString c5_desc = formatDesc(c5 ? c5->description() : "", c5_defaultDesc);

    html += QString("<div class='section-title'>5. %1</div>").arg(c5 ? c5->name() : "Esforço nas sapatas");
    html += QString("<div class='calc-desc' style='line-height: 1.5;'>%1</div>").arg(c5_desc);

    html += QString("<div class='formula-line-center'><i>M</i> = <i>Pc</i> &middot; <i>Ro</i> &rarr; <i>M</i> = %1 &middot; %2 = <b>%3 kgf</b></div>")
                .arg(QString::number(c5_pc, 'f', 0))
                .arg(QString::number(c5_ro, 'f', 0))
                .arg(QString::number(c5_m, 'f', 0));

    QString c5_f2_cells;
    c5_f2_cells += makeTextCell("<i>F</i> =&nbsp;");
    c5_f2_cells += makeFraction("<i>M</i>", "a + b");
    c5_f2_cells += makeTextCell("&nbsp;&rarr; <i>F</i> =&nbsp;");
    c5_f2_cells += makeFraction(QString::number(c5_m, 'f', 0), QString("%1 + %2").arg(QString::number(c5_a, 'f', 2).replace('.', ',')).arg(QString::number(c5_b, 'f', 1).replace('.', ',')));
    c5_f2_cells += makeTextCell(QString("&nbsp;= <b>%1 kgf</b>").arg(QString::number(c5_f, 'f', 0)));
    html += makeCenteredFormula(c5_f2_cells);

    QString c5_f3_cells;
    c5_f3_cells += makeTextCell("<i>ES</i> =&nbsp;(&nbsp;");
    c5_f3_cells += makeFraction("<i>Pg</i>", "2");
    c5_f3_cells += makeTextCell("&nbsp;) + <i>F</i> &rarr; <i>ES</i> =&nbsp;(&nbsp;");
    c5_f3_cells += makeFraction(QString::number(c5_pg, 'f', 0), "2");
    c5_f3_cells += makeTextCell(QString("&nbsp;) + %1 = <b>%2 kgf &cong; %3 tf</b>")
                                .arg(QString::number(c5_f, 'f', 0))
                                .arg(QString::number(c5_es, 'f', 0))
                                .arg(QString::number(c5_es_tf, 'f', 1).replace('.', ',')));
    html += makeCenteredFormula(c5_f3_cells);

    // 6. Resistência mínima exigida do terreno (RMET)
    const auto* c6 = getCalc(6);
    double c6_ld = c6 ? c6->parameterValue(0) : 50.0;
    double c6_cd = c6 ? c6->parameterValue(1) : 50.0;
    double c6_nd = c6 ? c6->parameterValue(2) : 1.0;
    double c6_es = c6 ? c6->parameterValue(3) : c5_es;
    double c6_ap = c6_ld * c6_cd * c6_nd;
    double c6_rmet = (c6_ap > 0) ? (c6_es / c6_ap) : 3.17;
    QString c6_desc = formatDesc(c6 ? c6->description() : "",
                                 "LD = Largura do dormente ׀ CD = Comprimento do dormente ׀ ND = Quantidade de dormentes ES = Esfor&ccedil;o nas sapatas (kgf) e AP = &Aacute;rea de patolamento (cm2)");

    html += QString("<div class='section-title'>6. %1</div>").arg(c6 ? c6->name() : "Resistência mínima exigida do terreno (RMET)");

    html += QString("<div class='formula-line-center'><i>AP</i> = <i>LD</i> &times; <i>CD</i> &times; <i>ND</i> &rarr; <i>AP</i> = %1 &times; %2 &times; %3 &rarr; <b><i>AP</i> = %4 cm<sup>2</sup></b></div>")
                .arg(c6_ld, 0, 'f', 0).arg(c6_cd, 0, 'f', 0).arg(c6_nd, 0, 'f', 0).arg(c6_ap, 0, 'f', 0);

    QString c6_f2_cells;
    c6_f2_cells += makeTextCell("<i>RMET</i> =&nbsp;");
    c6_f2_cells += makeFraction("<i>ES</i>", "<i>AP</i>");
    c6_f2_cells += makeTextCell("&nbsp;&rarr; <i>RMET</i> =&nbsp;");
    c6_f2_cells += makeFraction(QString::number(c6_es, 'f', 0), QString::number(c6_ap, 'f', 0));
    c6_f2_cells += makeTextCell(QString("&nbsp;&rarr; <b><i>RMET</i> = %1 kgf / cm<sup>2</sup></b>").arg(QString::number(c6_rmet, 'f', 2).replace('.', ',')));
    html += makeCenteredFormula(c6_f2_cells);

    html += QString("<div class='calc-desc' style='font-size: 8.5pt; font-style: italic; text-align: center; margin-top: 8px;'>%1</div>").arg(c6_desc);

    // 7. Capacidade de carga
    const auto* c7 = getCalc(7);
    double c7_raio = c7 ? c7->parameterValue(0) : 5.0;
    double c7_cap = c7 ? c7->parameterValue(1) : 7948.0;
    QString c7_desc = c7 ? c7->description().trimmed() : "";

    html += QString("<div class='section-title'>7. %1</div>").arg(c7 ? c7->name() : "Capacidade de carga");
    if (!c7_desc.isEmpty()) {
        html += QString("<div class='calc-desc'>%1</div>").arg(c7_desc.toHtmlEscaped().replace("\n", "<br>"));
    }
    html += QString("<div class='formula-line-center'>Raio de opera&ccedil;&atilde;o %1 m &rarr; ~ <b>%2 kgf (interpolado)</b></div>")
                .arg(QString::number(c7_raio, 'f', 1).replace('.', ',')).arg(QString::number(c7_cap, 'f', 0));

    // 8. Gráfico de carga MD 45007
    const auto* c8 = getCalc(8);
    QString c8_desc = c8 ? c8->description().trimmed() : "";

    html += QString("<div class='section-title'>8. %1</div>").arg(c8 ? c8->name() : "Gráfico de carga MD 45007");
    if (!c8_desc.isEmpty()) {
        html += QString("<div class='calc-desc'>%1</div>").arg(c8_desc.toHtmlEscaped().replace("\n", "<br>"));
    }

    if (c8 && !c8->formulaText().isEmpty()) {
        html += QString("<div class='formula-line-center'>%1</div>")
                    .arg(c8->formulaText().toHtmlEscaped());
    }

    // Additional custom calculations (if any)
    QVector<const RiggingCalculation*> extraCalcs;
    for (const auto* c : calculations) {
        if (c->id() > 8) {
            extraCalcs.append(c);
        }
    }

    if (!extraCalcs.isEmpty()) {
        html += "<div style='margin: 20px 0 10px 0; border-top: 2px solid #000000; padding-top: 10px;'></div>";
        html += "<div style='font-size: 12pt; font-weight: bold; margin-bottom: 10px; color: #000000;'>Cálculos Adicionais & Personalizados</div>";

        for (const auto* calc : extraCalcs) {
            html += buildCalculationSection(calc, preset);
        }
    }

    html += "</div>"; // End unified page-container

    return html;
}

QString DocumentGenerator::buildStandardDocument(
    const QVector<RiggingCalculation*>& calculations,
    const FormatPreset& preset,
    const QString& fileName)
{
    Q_UNUSED(fileName)
    QString html;
    html += "<div class='page-container'>";

    // Header
    html += buildPageHeader(preset);

    // Summary Table
    html += buildSummaryTable(calculations, preset);

    // Calculations
    html += "<h2 style='color: " + preset.colorTitle().name() + ";'>Detalhamento dos Cálculos</h2>";
    for (const auto* calc : calculations) {
        html += buildCalculationSection(calc, preset);
    }

    // Footer
    html += buildFooter(preset);
    html += "</div>";
    return html;
}

QString DocumentGenerator::buildCalculationSection(
    const RiggingCalculation* calc, const FormatPreset& preset)
{
    Q_UNUSED(preset)
    QString html;
    html += "<div style='margin-bottom: 14px; padding: 10px 12px; border: 1px solid #E2E8F0; border-radius: 6px; background-color: #FAFAFA;'>";

    // Title
    html += QString("<div class='section-title' style='margin-top: 0;'>%1. %2</div>")
                .arg(calc->id()).arg(calc->name());

    // Description (user editable)
    if (!calc->description().isEmpty()) {
        html += QString("<div class='calc-desc' style='margin-left: 0;'>%1</div>")
                    .arg(calc->description().toHtmlEscaped().replace("\n", "<br>"));
    }

    // Formula
    if (!calc->formulaText().isEmpty()) {
        html += QString("<div style='font-family: Consolas, monospace; background-color: #F1F5F9; padding: 5px 8px; border-radius: 4px; color: #0F172A; font-size: 10pt; margin: 4px 0;'>%1</div>")
                    .arg(calc->formulaText());
    }

    // Parameters
    if (!calc->parameters().isEmpty()) {
        html += "<table style='width: 100%; border-collapse: collapse; margin: 4px 0;'>";
        for (const auto& p : calc->parameters()) {
            html += "<tr>";
            html += QString("<td style='padding: 2px 4px; color: #475569; font-size: 9pt;'>%1</td>").arg(p.name);
            html += QString("<td style='padding: 2px 4px; text-align: right; font-weight: bold; color: #0F172A; font-size: 9pt;'>%1</td>")
                        .arg(p.effectiveValue(), 0, 'g', 6);
            html += QString("<td style='padding: 2px 4px; color: #64748B; font-size: 9pt; width: 40px;'>%1</td>").arg(p.unit);
            html += "</tr>";
        }
        html += "</table>";
    }

    // Result
    if (calc->isValid()) {
        html += QString("<div style='font-weight: bold; color: #166534; font-size: 10.5pt; margin-top: 4px;'>Resultado: %1 %2</div>")
                    .arg(calc->result(), 0, 'g', 8)
                    .arg(calc->resultUnit());
    } else if (calc->status() == RiggingCalculation::Error) {
        html += "<div style='font-weight: bold; color: #991B1B; font-size: 9.5pt; margin-top: 4px;'>Erro no cálculo</div>";
    } else {
        html += "<div style='color: #64748B; font-size: 9.5pt; font-style: italic; margin-top: 4px;'>Aguardando dados...</div>";
    }

    html += "</div>";
    return html;
}

QString DocumentGenerator::buildSummaryTable(
    const QVector<RiggingCalculation*>& calculations,
    const FormatPreset& preset)
{
    QString html;
    html += "<h2 style='color: " + preset.colorTitle().name() + "; font-size: 13pt;'>Resumo dos Cálculos</h2>";
    html += "<table class='data-table'>";
    html += "<thead><tr>"
            "<th style='width: 5%;'>#</th>"
            "<th style='width: 35%;'>Cálculo</th>"
            "<th style='width: 30%;'>Fórmula</th>"
            "<th style='width: 20%;'>Resultado</th>"
            "<th style='width: 10%;'>Status</th>"
            "</tr></thead><tbody>";

    for (const auto* calc : calculations) {
        html += "<tr>";
        html += QString("<td>%1</td>").arg(calc->id());
        html += QString("<td><strong>%1</strong></td>").arg(calc->name());
        html += QString("<td style='font-family: Consolas, monospace; color: %1;'>%2</td>")
                    .arg(preset.colorAccent().name(), calc->formulaText());

        if (calc->status() == RiggingCalculation::Computed) {
            html += QString("<td class='param-value'>%1 %2</td>")
                        .arg(calc->result(), 0, 'g', 6)
                        .arg(calc->resultUnit());
            html += "<td class='status-ok'>✓</td>";
        } else if (calc->status() == RiggingCalculation::Error) {
            html += "<td class='status-err'>Erro</td>";
            html += "<td class='status-err'>✗</td>";
        } else {
            html += "<td style='color: #9E9E9E;'>—</td>";
            html += "<td class='status-warn'>⏳</td>";
        }
        html += "</tr>";
    }

    html += "</tbody></table>";
    return html;
}

QString DocumentGenerator::buildFooter(const FormatPreset& preset)
{
    QString html;
    html += "<div class='page-footer' style='border-top: 1px solid #E2E8F0; margin-top: 20px;'>";
    if (!preset.footerText().isEmpty()) {
        html += preset.footerText() + "<br>";
    }
    html += QString("Gerado em %1 — RigPlan Integrator")
                .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    html += "</div>";
    return html;
}

QString DocumentGenerator::tableStyleCss(const FormatPreset& preset)
{
    QString css;
    QString headerColor = preset.colorTableHeader().name();
    bool borders = preset.tableBorders();
    bool alternating = preset.tableAlternatingRows();

    css += ".data-table { width: 100%; border-collapse: collapse; margin: 10px 0; } ";
    css += QString(".data-table thead tr { background-color: %1; color: #FFFFFF; } ").arg(headerColor);
    css += ".data-table th { padding: 5px 8px; text-align: left; font-size: 9pt; } ";
    css += ".data-table td { padding: 5px 8px; font-size: 9pt; } ";

    if (borders) {
        css += ".data-table th, .data-table td { border: 1px solid #CBD5E1; } ";
    } else {
        css += ".data-table th { border-bottom: 2px solid " + headerColor + "; } ";
        css += ".data-table td { border-bottom: 1px solid #E2E8F0; } ";
    }

    if (alternating) {
        css += ".data-table tbody tr:nth-child(even) { background-color: #F8FAFC; } ";
        css += ".data-table tbody tr:nth-child(odd) { background-color: #FFFFFF; } ";
    }

    return css;
}

QString DocumentGenerator::buildPlanoRiggingPage(
    const QVector<RiggingCalculation*>& calculations,
    const FormatPreset& preset)
{
    const RiggingPlanData& data = preset.planData();
    QString logoB64 = getBase64Image(":/images/image3.png", "resources/images/image3.png");

    // Map calculations by ID
    QMap<int, const RiggingCalculation*> calcMap;
    for (const auto* c : calculations) {
        calcMap[c->id()] = c;
    }
    auto getCalc = [&](int id) -> const RiggingCalculation* {
        return calcMap.value(id, nullptr);
    };

    // Calculation results (Pontos rosas vinculados aos cálculos do outro estilo)
    const auto* c2 = getCalc(2);
    const auto* c4 = getCalc(4);
    const auto* c5 = getCalc(5);
    const auto* c6 = getCalc(6);
    const auto* c7 = getCalc(7);
    const RiggingCalculation* windCalc = nullptr;
    for (const auto* calc : calculations) {
        if (calc->name().contains("vento", Qt::CaseInsensitive)) {
            windCalc = calc;
            break;
        }
    }

    // c2: result = Te (tensão), param[2] = Cap → TU = Te/Cap * 100
    bool hasC2Utilization = false;
    double c2_tu = 0.0;
    if (c2 && c2->isValid()) {
        double te = c2->result();
        double cap = c2->parameterValue(2);
        if (cap > 0) {
            c2_tu = (te / cap) * 100.0;
            hasC2Utilization = true;
        }
    }

    // Os campos rosa exibem somente resultados efetivamente calculados.
    const QString c4TuText = c4 && c4->isValid()
        ? QString::number(c4->result(), 'f', 1).replace('.', ',') : "-";
    const QString c5EsText = c5 && c5->isValid()
        ? QString::number(c5->result(), 'f', 0) : "-";
    const QString c6RmetText = c6 && c6->isValid()
        ? QString::number(c6->result(), 'f', 2).replace('.', ',') : "-";
    const QString c7CapacityText = c7 && c7->isValid()
        ? QString::number(c7->result() / 1000.0, 'f', 2).replace('.', ',') : "-";
    const QString windText = windCalc && windCalc->isValid()
        ? QString::number(windCalc->result(), 'f', 1).replace('.', ',') : "-";

    // Totais e cálculos automáticos (Linhas azuis, pontos verdes, pontos amarelos)
    double pesoLingada = data.pesoTotalLingada();
    double pesoBalancim = data.pesoTotalBalancim();
    double cargaBrutaKg = data.cargaBruta();
    double cargaBrutaTon = data.cargaBrutaTon();
    double tuGuindaste = 0.0;
    if (c7 && c7->isValid() && c7->result() > 0.0) {
        tuGuindaste = (cargaBrutaKg / c7->result()) * 100.0;
    }

    QString html;
    html += "<div style='width: 100%; border: 1.5px solid #000000; padding: 2px; background: #FFFFFF; font-family: Arial, Helvetica, sans-serif; font-size: 7.5pt; color: #000000; box-sizing: border-box;'>";

    // 1. HEADER
    html += "<table width='100%' border='0' cellspacing='0' cellpadding='0' style='border-collapse: collapse; border: 1.5px solid #000000; margin-bottom: 2px;'>";
    html += "<tr>";
    // Col 1: Logo
    html += "<td width='35%' align='center' valign='middle' style='border-right: 1.5px solid #000000; padding: 2px;'>";
    if (preset.showLogo() && !logoB64.isEmpty()) {
        html += QString("<img src='%1' style='max-height: 25px; max-width: 120px;' alt='GUINDASTES RIBAS' />").arg(logoB64);
    } else {
        html += "<div style='font-size: 10.5pt; font-weight: 900; color: #000000;'>GUINDASTES RIBAS</div>";
    }
    html += "</td>";
    // Col 2: Info Box + Title
    html += "<td width='65%' valign='top' style='padding: 0;'>";
    html += "<table width='100%' border='0' cellspacing='0' cellpadding='0' style='border-collapse: collapse;'>";
    html += QString("<tr><td width='18%' style='font-size: 7pt; font-weight: bold; border-bottom: 1px solid #000; border-right: 1px solid #000; text-align: right; padding-right: 3px;'>CLIENTE:</td>"
                    "<td colspan='3' style='font-size: 7.5pt; border-bottom: 1px solid #000; padding-left: 4px;'>%1</td></tr>")
                .arg(data.cliente.toHtmlEscaped());
    html += QString("<tr><td style='font-size: 7pt; font-weight: bold; border-bottom: 1px solid #000; border-right: 1px solid #000; text-align: right; padding-right: 3px;'>LOCAL:</td>"
                    "<td width='42%' style='font-size: 7.5pt; border-bottom: 1px solid #000; border-right: 1px solid #000; padding-left: 4px;'>%1</td>"
                    "<td width='14%' style='font-size: 7pt; font-weight: bold; border-bottom: 1px solid #000; border-right: 1px solid #000; text-align: right; padding-right: 3px;'>DATA:</td>"
                    "<td width='26%' style='font-size: 7.5pt; border-bottom: 1px solid #000; text-align: center;'>%2</td></tr>")
                .arg(data.local.toHtmlEscaped(), data.data.toHtmlEscaped());
    html += QString("<tr><td colspan='4' align='center' style='padding: 1px 0; font-size: 10pt; font-weight: bold; letter-spacing: 0.5px; text-align: center;'>"
                    "PLANO DE RIGGING nº %1</td></tr>")
                .arg(data.planoNumero.toHtmlEscaped());
    html += "</table>";
    html += "</td>";
    html += "</tr>";
    html += "</table>";

    // 2. DADOS DA CARGA:
    html += "<div style='font-size: 8pt; font-weight: bold; margin: 1px 0;'>DADOS DA CARGA:</div>";
    html += "<table width='100%' border='1' cellspacing='0' cellpadding='0' style='border-collapse: collapse; border: 1px solid #000; text-align: center; margin-bottom: 2px;'>";
    html += "<tr style='font-size: 7.5pt; font-weight: bold;'>"
            "<td width='50%'>Descrição da carga</td>"
            "<td width='12%'>Comp. (m)</td>"
            "<td width='12%'>Largura (m)</td>"
            "<td width='12%'>Altura (m)</td>"
            "<td width='14%'>Peso (kg)</td></tr>";
    html += QString("<tr style='font-size: 7.5pt;'>"
                    "<td align='left' style='padding-left: 4px;'>%1</td>"
                    "<td>%2</td>"
                    "<td>%3</td>"
                    "<td>%4</td>"
                    "<td>%5</td></tr>")
                .arg(data.cargaDescricao.toHtmlEscaped())
                .arg(QString::number(data.cargaComp, 'f', 1).replace('.', ','))
                .arg(QString::number(data.cargaLargura, 'f', 2).replace('.', ','))
                .arg(QString::number(data.cargaAltura, 'f', 1).replace('.', ','))
                .arg(QString::number(data.cargaPeso, 'f', 0));
    html += "</table>";

    // 3. LINGADA / AMARRAS
    html += "<table width='100%' border='1' cellspacing='0' cellpadding='0' style='border-collapse: collapse; border: 1px solid #000; text-align: center; margin-bottom: 2px;'>";
    html += "<tr style='font-size: 8pt; font-weight: bold;'>"
            "<td colspan='3' align='left' style='padding-left: 4px;'>LINGADA / AMARRAS</td>"
            "<td colspan='2'>Pesos</td></tr>";
    html += "<tr style='font-size: 7.5pt; font-weight: bold;'>"
            "<td width='6%'>Item</td>"
            "<td width='64%'>Descrição</td>"
            "<td width='10%'>Qtd. (pç)</td>"
            "<td width='10%'>Unitário (kg)</td>"
            "<td width='10%'>Total (kg)</td></tr>";

    for (const auto& it : data.lingadaItems) {
        html += QString("<tr style='font-size: 7pt;'>"
                        "<td>%1</td>"
                        "<td align='left' style='padding-left: 4px;'>%2</td>"
                        "<td>%3</td>"
                        "<td>%4</td>"
                        "<td>%5</td></tr>")
                    .arg(it.item)
                    .arg(it.descricao.toHtmlEscaped())
                    .arg(it.qtd, 0, 'g', 4)
                    .arg(QString::number(it.unitario, 'f', 2).replace('.', ','))
                    .arg(QString::number(it.total(), 'f', 2).replace('.', ','));
    }

    html += QString("<tr style='font-size: 7.5pt; font-weight: bold;'>"
                    "<td colspan='4' align='right' style='padding-right: 6px;'>PESO TOTAL DA LINGADA / AMARRAÇÃO:</td>"
                    "<td>%1</td></tr>")
                .arg(QString::number(pesoLingada, 'f', 2).replace('.', ','));
    html += "</table>";

    // 4. DISPOSITIVOS / BALANCIM
    html += "<table width='100%' border='1' cellspacing='0' cellpadding='0' style='border-collapse: collapse; border: 1px solid #000; text-align: center; margin-bottom: 2px;'>";
    html += "<tr style='font-size: 8pt; font-weight: bold;'>"
            "<td colspan='4' align='left' style='padding-left: 4px;'>DISPOSITIVOS / BALANCIM</td>"
            "<td colspan='2'>Pesos</td></tr>";
    html += "<tr style='font-size: 7.5pt; font-weight: bold;'>"
            "<td width='6%'>Item</td>"
            "<td width='50%'>Descrição</td>"
            "<td width='14%'>Ref. /Desenho</td>"
            "<td width='10%'>Qtd. (pç)</td>"
            "<td width='10%'>Unitário (kg)</td>"
            "<td width='10%'>Total (kg)</td></tr>";

    for (const auto& it : data.balancimItems) {
        html += QString("<tr style='font-size: 7pt;'>"
                        "<td>%1</td>"
                        "<td align='left' style='padding-left: 4px;'>%2</td>"
                        "<td>%3</td>"
                        "<td>%4</td>"
                        "<td>%5</td>"
                        "<td>%6</td></tr>")
                    .arg(it.item)
                    .arg(it.descricao.toHtmlEscaped())
                    .arg(it.refDesenho.toHtmlEscaped())
                    .arg(it.qtd, 0, 'g', 4)
                    .arg(QString::number(it.unitario, 'f', 0))
                    .arg(QString::number(it.total(), 'f', 0));
    }

    html += QString("<tr style='font-size: 7.5pt; font-weight: bold;'>"
                    "<td colspan='5' align='right' style='padding-right: 6px;'>PESO TOTAL DISPOSITIVOS / BALANCIM:</td>"
                    "<td>%1</td></tr>")
                .arg(QString::number(pesoBalancim, 'f', 0));
    html += "</table>";

    // 5. INFORMAÇÕES DO GUINDASTE
    html += "<div style='border: 1px solid #000; padding: 1px 2px; margin-bottom: 2px;'>";
    html += "<div style='font-size: 8pt; font-weight: bold; margin: 1px 0;'>INFORMAÇÕES DO GUINDASTE</div>";
    html += "<table width='100%' border='1' cellspacing='0' cellpadding='0' style='border-collapse: collapse; border: 1px solid #000; font-size: 7.5pt;'>";
    html += QString("<tr><td width='15%' style='font-weight: bold; text-align: right; padding-right: 6px;'>Modelo</td>"
                    "<td colspan='3' style='font-weight: bold;'>%1</td></tr>")
                .arg(data.guindasteModelo.toHtmlEscaped());
    html += QString("<tr><td style='font-weight: bold; text-align: right; padding-right: 6px;'>Caminhão</td>"
                    "<td width='45%'><b>%1</b></td>"
                    "<td width='15%' style='font-weight: bold; text-align: right; padding-right: 6px;'>Placas</td>"
                    "<td width='25%'><b>%2</b></td></tr>")
                .arg(data.guindasteCaminhao.toHtmlEscaped(), data.guindastePlacas.toHtmlEscaped());
    html += QString("<tr><td style='font-weight: bold; text-align: right; padding-right: 6px;'>Chassi</td>"
                    "<td><b>%1</b></td>"
                    "<td style='font-weight: bold; text-align: right; padding-right: 6px;'>Ano de Fabricação</td>"
                    "<td><b>%2</b></td></tr>")
                .arg(data.guindasteChassi.toHtmlEscaped(), data.guindasteAno.toHtmlEscaped());
    html += "</table>";
    html += "</div>";

    // 6. DOIS BLOCOS: CONFIGURAÇÃO DO GUINDASTE & COMPOSIÇÃO DA CARGA BRUTA
    html += "<table width='100%' border='0' cellspacing='0' cellpadding='0' style='border-collapse: collapse; margin-bottom: 2px;'>";
    html += "<tr>";

    // Coluna Esquerda: Configuração + Quadrante
    html += "<td width='49%' valign='top' style='border: 1px solid #000; padding: 2px;'>";
    html += "<div style='font-size: 8pt; font-weight: bold; text-align: center; border-bottom: 1px solid #000; padding-bottom: 1px;'>CONFIGURAÇÃO DO GUINDASTE</div>";
    html += "<table width='100%' border='1' cellspacing='0' cellpadding='0' style='border-collapse: collapse; border: 1px solid #000; font-size: 7pt; margin-top: 1px;'>";
    html += QString("<tr style='border-bottom: 1px solid #000;'><td width='65%'>Capacidade nominal</td><td width='25%' align='right'><b>%1</b></td><td width='10%'><b>t</b></td></tr>")
                .arg(QString::number(data.capNominal, 'f', 1).replace('.', ','));
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Comprimento da lança</td><td align='right'><b>%1</b></td><td><b>m</b></td></tr>")
                .arg(QString::number(data.compLanca, 'f', 1).replace('.', ','));
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Raio de operação</td><td align='right'><b>%1</b></td><td><b>m</b></td></tr>")
                .arg(QString::number(data.raioOperacao, 'f', 1).replace('.', ','));
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Capacidade de carga</td><td align='right'><b>%1</b></td><td><b>t</b></td></tr>")
                .arg(QString::number(data.capCargaNoRaio, 'f', 1).replace('.', ','));
    html += "</table>";

    html += "<div style='font-size: 8pt; font-weight: bold; text-align: center; border-bottom: 1px solid #000; padding: 2px 0 1px 0;'>QUADRANTE OPERACIONAL</div>";
    html += "<table width='100%' border='1' cellspacing='0' cellpadding='0' style='border-collapse: collapse; border: 1px solid #000; font-size: 7pt; text-align: center; margin-top: 1px;'>";
    html += "<tr style='border-bottom: 1px solid #000;'><td width='50%'>Inicial</td><td width='50%'>Final</td></tr>";
    html += QString("<tr style='border-bottom: 1px solid #000;'><td><b>%1</b></td><td><b>%2</b></td></tr>")
                .arg(data.quadranteInicial.toHtmlEscaped(), data.quadranteFinal.toHtmlEscaped());
    html += "</table>";
    html += "</td>";

    html += "<td width='2%'></td>"; // Gap

    // Coluna Direita: Composição da Carga Bruta
    html += "<td width='49%' valign='top' style='border: 1px solid #000; padding: 2px;'>";
    html += "<div style='font-size: 8pt; font-weight: bold; text-align: center; border-bottom: 1px solid #000; padding-bottom: 1px;'>COMPOSIÇÃO DA CARGA BRUTA</div>";
    html += "<table width='100%' border='1' cellspacing='0' cellpadding='0' style='border-collapse: collapse; border: 1px solid #000; font-size: 7pt; margin-top: 1px;'>";
    html += QString("<tr style='border-bottom: 1px solid #000;'><td width='65%'>Carga líquida</td><td width='25%' align='right'><b>%1</b></td><td width='10%'><b>kg</b></td></tr>")
                .arg(QString::number(data.cargaPeso, 'f', 0));
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Lingada / Amarração</td><td align='right'><b>%1</b></td><td><b>kg</b></td></tr>")
                .arg(QString::number(pesoLingada, 'f', 2).replace('.', ','));
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Dispositivos / Balancim</td><td align='right'><b>%1</b></td><td><b>kg</b></td></tr>")
                .arg(QString::number(pesoBalancim, 'f', 0));
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Cargas extras</td><td align='right'><b>%1</b></td><td><b>kg</b></td></tr>")
                .arg(data.cargasExtras > 0 ? QString::number(data.cargasExtras, 'f', 0) : "-");
    html += QString("<tr style='border-bottom: 1px solid #000; font-size: 7.5pt;'><td><b>Carga bruta</b></td><td align='right'><b>%1</b></td><td><b>kg</b></td></tr>")
                .arg(QString::number(std::round(cargaBrutaKg), 'f', 0));
    html += "</table>";
    html += "</td>";

    html += "</tr>";
    html += "</table>";

    // 7. DOIS BLOCOS INFERIORES: RESUMO DOS CÁLCULOS & OBSERVAÇÕES/ART
    html += "<table width='100%' border='0' cellspacing='0' cellpadding='0' style='border-collapse: collapse; margin-top: 2px; margin-bottom: 2px;'>";
    html += "<tr>";

    // Coluna Esquerda: Resumo dos Cálculos (Pontos rosas)
    html += "<td width='49%' valign='top' style='border: 1px solid #000; padding: 2px;'>";
    html += "<div style='font-size: 8pt; font-weight: bold; text-align: center; border-bottom: 1px solid #000; padding-bottom: 1px;'>RESUMO DOS CÁLCULOS</div>";
    html += "<table width='100%' border='1' cellspacing='0' cellpadding='0' style='border-collapse: collapse; border: 1px solid #000; font-size: 7pt; margin-top: 1px;'>";
    html += QString("<tr style='border-bottom: 1px solid #000;'><td width='70%'>Total da carga bruta</td><td width='30%' align='right'><b>%1 t</b></td></tr>")
                .arg(QString::number(cargaBrutaTon, 'f', 2).replace('.', ','));
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Capacidade de carga do guindaste</td><td align='right'><b>%1 t</b></td></tr>")
                .arg(c7CapacityText);
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Taxa de utilização do guindaste</td><td align='right'><b>%1 %</b></td></tr>")
                .arg(QString::number(tuGuindaste, 'f', 1).replace('.', ','));
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Taxa de utilização da cinta plana</td><td align='right'><b>%1 %</b></td></tr>")
                .arg(hasC2Utilization ? QString::number(c2_tu, 'f', 1).replace('.', ',') : "-");
    html += "<tr style='border-bottom: 1px solid #000;'><td>Taxa de utilização da cinta tubular</td><td align='right'><b>- %</b></td></tr>";
    html += "<tr style='border-bottom: 1px solid #000;'><td>Taxa de utiliz. laço de cabo de aço</td><td align='right'><b>- %</b></td></tr>";
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Taxa de utilização da manilha</td><td align='right'><b>%1 %</b></td></tr>")
                .arg(c4TuText);
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Esforço nas sapatas</td><td align='right'><b>%1 kgf</b></td></tr>")
                .arg(c5EsText);
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Resistência mínima do terreno</td><td align='right'><b>%1 kgf/cm<sup>2</sup></b></td></tr>")
                .arg(c6RmetText);
    html += QString("<tr style='border-bottom: 1px solid #000;'><td>Velocidade máxima do vento</td><td align='right'><b>%1 m/s</b></td></tr>")
                .arg(windText);
    html += "</table>";
    html += "</td>";

    html += "<td width='2%'></td>"; // Gap

    // Coluna Direita: Observações / Anexos + ART
    html += "<td width='49%' valign='top' style='border: 1px solid #000; padding: 2px;'>";
    html += "<div style='border: 1px solid #000; padding: 2px; box-sizing: border-box;'>";
    html += "<div style='font-size: 7.5pt; font-weight: bold;'>Observações / Anexos:</div>";
    if (!data.observacoes.isEmpty()) {
        html += QString("<div style='font-size: 7pt; margin-top: 1px;'>%1</div>").arg(data.observacoes.toHtmlEscaped());
    }
    // Large ART
    html += QString("<div style='text-align: center; margin-top: 3px; font-size: 9pt; font-weight: bold;'>ART %1</div>")
                .arg(data.artNumero.toHtmlEscaped());
    html += "</div>";
    html += "</td>";

    html += "</tr>";
    html += "</table>";

    // 8. RODAPÉ / ASSINATURAS
    html += "<table width='100%' border='0' cellspacing='0' cellpadding='0' style='border-collapse: collapse; margin-top: 2px; font-size: 7.5pt;'>";
    html += "<tr>";
    html += QString("<td width='49%' align='center' style='border-top: 1px solid #000; padding-top: 2px;'>%1</td>").arg(data.riggerNome.toHtmlEscaped());
    html += "<td width='2%'></td>";
    html += QString("<td width='49%' align='center' style='border-top: 1px solid #000; padding-top: 2px;'>%1</td>").arg(data.supervisorNome.toHtmlEscaped());
    html += "</tr>";
    html += "</table>";

    html += "</div>"; // End container
    return html;
}
