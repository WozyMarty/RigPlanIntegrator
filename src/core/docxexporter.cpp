#include "docxexporter.h"
#include "models/riggingcalculation.h"
#include "models/formatpreset.h"
#include <QFile>
#include <QDateTime>
#include <QColor>
#include <QImage>
#include <QPainter>
#include <QPageSize>
#include <QBuffer>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextFrame>
#include <QTextImageFormat>
#include <QTextTable>
#include <QTextTableCell>
#include <QTextTableCellFormat>
#include <QTextTableFormat>
#include <QTextLength>
#include <QUrl>
#include <QXmlStreamReader>
#include <QRegularExpression>
#include <functional>
#include <cstdint>

DocxExporter::DocxExporter(QObject* parent)
    : QObject(parent)
{
}

bool DocxExporter::exportToDocx(
    QTextDocument* document,
    const FormatPreset& preset,
    const QString& filePath)
{
    emit exportStarted();

    if (!document) {
        emit exportError("Documento de pré-visualização inválido.");
        emit exportFinished(false, filePath);
        return false;
    }

    m_embeddedImages.clear();

    QSize pagePixels;
    QSizeF pagePoints;
    const QVector<QByteArray> renderedPages = renderPages(document, preset, &pagePixels, &pagePoints);
    if (renderedPages.isEmpty()) {
        emit exportError("Não foi possível renderizar o documento para o formato DOCX.");
        emit exportFinished(false, filePath);
        return false;
    }

    m_embeddedImages = renderedPages;

    QByteArray documentXml = generatePreviewDocument(
        renderedPages.size(),
        pagePixels,
        pagePoints,
        preset
    );

    if (documentXml.isEmpty()) {
        emit exportError("Não foi possível gerar o conteúdo do Word.");
        emit exportFinished(false, filePath);
        return false;
    }

    QVector<ZipEntry> entries;
    auto addEntry = [&](const QString& name, const QByteArray& data) {
        ZipEntry e;
        e.name = name;
        e.data = data;
        e.crc32 = computeCrc32(data);
        e.offset = 0;
        entries.append(e);
    };

    addEntry("[Content_Types].xml", generateContentTypes());
    addEntry("_rels/.rels", generateRels());
    addEntry("word/_rels/document.xml.rels", generateDocumentRels(m_embeddedImages.size()));
    addEntry("word/styles.xml", generateStyles(preset));
    addEntry("word/document.xml", documentXml);
    for (int i = 0; i < m_embeddedImages.size(); ++i) {
        addEntry(QString("word/media/image_%1.png").arg(i + 1), m_embeddedImages[i]);
    }

    QByteArray zipData = createZip(entries);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit exportError(QString("Não foi possível criar o arquivo: %1").arg(filePath));
        emit exportFinished(false, filePath);
        return false;
    }

    file.write(zipData);
    file.close();

    emit exportFinished(true, filePath);
    return true;
}

bool DocxExporter::exportSectionsToDocx(
    const QVector<QTextDocument*>& sectionDocs,
    const FormatPreset& preset,
    const QString& filePath)
{
    emit exportStarted();

    if (sectionDocs.isEmpty()) {
        emit exportError("Nenhuma seção para exportar.");
        emit exportFinished(false, filePath);
        return false;
    }

    m_embeddedImages.clear();

    const QSizeF paperPoints = QPageSize(QPageSize::A4).size(QPageSize::Point);
    constexpr qreal rasterScale = 2.0; // 144 dpi
    QSize pagePixels(qRound(paperPoints.width() * rasterScale),
                     qRound(paperPoints.height() * rasterScale));

    // Render each section document as separate page images
    for (QTextDocument* doc : sectionDocs) {
        if (!doc) continue;

        doc->setPageSize(paperPoints);
        const int sectionPageCount = qMax(1, doc->pageCount());

        for (int page = 0; page < sectionPageCount; ++page) {
            QImage image(pagePixels, QImage::Format_RGB32);
            image.fill(Qt::white);

            QPainter painter(&image);
            painter.scale(rasterScale, rasterScale);
            const qreal top = page * paperPoints.height();
            painter.translate(0, -top);
            doc->drawContents(&painter, QRectF(0, top, paperPoints.width(), paperPoints.height()));
            painter.end();

            QByteArray png;
            QBuffer buffer(&png);
            if (!buffer.open(QIODevice::WriteOnly) || !image.save(&buffer, "PNG")) {
                emit exportError("Falha ao renderizar página da seção.");
                emit exportFinished(false, filePath);
                return false;
            }
            m_embeddedImages.append(png);
        }
    }

    if (m_embeddedImages.isEmpty()) {
        emit exportError("Não foi possível renderizar nenhuma página.");
        emit exportFinished(false, filePath);
        return false;
    }

    QByteArray documentXml = generatePreviewDocument(
        m_embeddedImages.size(),
        pagePixels,
        paperPoints,
        preset
    );

    if (documentXml.isEmpty()) {
        emit exportError("Não foi possível gerar o conteúdo do Word.");
        emit exportFinished(false, filePath);
        return false;
    }

    QVector<ZipEntry> entries;
    auto addEntry = [&](const QString& name, const QByteArray& data) {
        ZipEntry e;
        e.name = name;
        e.data = data;
        e.crc32 = computeCrc32(data);
        e.offset = 0;
        entries.append(e);
    };

    addEntry("[Content_Types].xml", generateContentTypes());
    addEntry("_rels/.rels", generateRels());
    addEntry("word/_rels/document.xml.rels", generateDocumentRels(m_embeddedImages.size()));
    addEntry("word/styles.xml", generateStyles(preset));
    addEntry("word/document.xml", documentXml);
    for (int i = 0; i < m_embeddedImages.size(); ++i) {
        addEntry(QString("word/media/image_%1.png").arg(i + 1), m_embeddedImages[i]);
    }

    QByteArray zipData = createZip(entries);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit exportError(QString("Não foi possível criar o arquivo: %1").arg(filePath));
        emit exportFinished(false, filePath);
        return false;
    }

    file.write(zipData);
    file.close();

    emit exportFinished(true, filePath);
    return true;
}

QByteArray DocxExporter::generateContentTypes()
{
    return QByteArray(
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
        "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
        "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
        "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
        "<Default Extension=\"png\" ContentType=\"image/png\"/>"
        "<Override PartName=\"/word/document.xml\" "
        "ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>"
        "<Override PartName=\"/word/styles.xml\" "
        "ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml\"/>"
        "</Types>");
}

QByteArray DocxExporter::generateRels()
{
    return QByteArray(
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
        "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
        "<Relationship Id=\"rId1\" "
        "Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" "
        "Target=\"word/document.xml\"/>"
        "</Relationships>");
}

QByteArray DocxExporter::generateDocumentRels(int imageCount)
{
    QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
                  "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
                  "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/>";
    for (int i = 0; i < imageCount; ++i) {
        xml += QString("<Relationship Id=\"rId%1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/image\" Target=\"media/image_%2.png\"/>")
                   .arg(i + 2).arg(i + 1);
    }
    xml += "</Relationships>";
    return xml.toUtf8();
}

QByteArray DocxExporter::generateStyles(const FormatPreset& preset)
{
    QString xml;
    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n";
    xml += "<w:styles xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">";

    // Default style
    xml += "<w:style w:type=\"paragraph\" w:default=\"1\" w:styleId=\"Normal\">";
    xml += "<w:name w:val=\"Normal\"/>";
    xml += "<w:rPr>";
    xml += QString("<w:rFonts w:ascii=\"%1\" w:hAnsi=\"%1\"/>").arg(escapeXml(preset.fontFamily()));
    xml += QString("<w:sz w:val=\"%1\"/>").arg(preset.fontSizeBody() * 2);
    xml += QString("<w:color w:val=\"%1\"/>").arg(colorToDocx(preset.colorBody()));
    xml += "</w:rPr></w:style>";

    // Heading 1
    xml += "<w:style w:type=\"paragraph\" w:styleId=\"Heading1\">";
    xml += "<w:name w:val=\"heading 1\"/>";
    xml += "<w:pPr><w:pBdr><w:bottom w:val=\"single\" w:sz=\"12\" w:space=\"4\" "
           "w:color=\"" + colorToDocx(preset.colorTitle()) + "\"/></w:pBdr></w:pPr>";
    xml += "<w:rPr>";
    xml += QString("<w:rFonts w:ascii=\"%1\" w:hAnsi=\"%1\"/>").arg(escapeXml(preset.fontFamily()));
    xml += QString("<w:sz w:val=\"%1\"/>").arg(preset.fontSizeTitle() * 2);
    xml += "<w:b/>";
    xml += QString("<w:color w:val=\"%1\"/>").arg(colorToDocx(preset.colorTitle()));
    xml += "</w:rPr></w:style>";

    // Heading 2
    xml += "<w:style w:type=\"paragraph\" w:styleId=\"Heading2\">";
    xml += "<w:name w:val=\"heading 2\"/>";
    xml += "<w:rPr>";
    xml += QString("<w:rFonts w:ascii=\"%1\" w:hAnsi=\"%1\"/>").arg(escapeXml(preset.fontFamily()));
    xml += QString("<w:sz w:val=\"%1\"/>").arg(preset.fontSizeSubtitle() * 2);
    xml += "<w:b/>";
    xml += QString("<w:color w:val=\"%1\"/>").arg(colorToDocx(preset.colorAccent()));
    xml += "</w:rPr></w:style>";

    xml += "</w:styles>";
    return xml.toUtf8();
}

QByteArray DocxExporter::generateEditableDocument(QTextDocument* document,
                                                  const FormatPreset& preset)
{
    if (!document) return {};

    QString html = document->toHtml();
    html.replace("&nbsp;", "&#160;");
    html.replace("&mdash;", "&#8212;");
    html.replace("&ndash;", "&#8211;");
    html.replace("&bull;", "&#8226;");

    QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n";
    xml += "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" "
           "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" "
           "xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" "
           "xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" "
           "xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\"><w:body>";

    struct ElementState {
        QString tag;
        QTextCharFormat charFormat;
    };
    QVector<ElementState> stack;
    QVector<bool> cellHasParagraph;
    QVector<bool> tableHasBorders;
    bool inBody = false;
    bool inParagraph = false;
    int paragraphCount = 0;
    int imageIndex = 0;
    int tableIndex = 0;
    const qreal emuPerPoint = 12700.0;

    auto escapeText = [](QString value) {
        value.replace("&", "&amp;");
        value.replace("<", "&lt;");
        value.replace(">", "&gt;");
        return value;
    };
    auto currentFormat = [&]() {
        return stack.isEmpty() ? QTextCharFormat() : stack.last().charFormat;
    };
    auto openParagraph = [&]() {
        if (!inParagraph) {
            xml += "<w:p>";
            inParagraph = true;
            ++paragraphCount;
        }
    };
    auto closeParagraph = [&]() {
        if (inParagraph) {
            xml += "</w:p>";
            inParagraph = false;
            if (!cellHasParagraph.isEmpty()) cellHasParagraph.last() = true;
        }
    };
    auto runProperties = [&](const QTextCharFormat& format) {
        QString props;
        if (format.fontWeight() >= QFont::Bold) props += "<w:b/>";
        if (format.fontItalic()) props += "<w:i/>";
        if (format.fontUnderline()) props += "<w:u w:val=\"single\"/>";
        if (format.foreground().style() != Qt::NoBrush) {
            const QColor color = format.foreground().color();
            if (color.isValid()) props += QString("<w:color w:val=\"%1\"/>").arg(colorToDocx(color));
        }
        if (format.background().style() != Qt::NoBrush) {
            const QColor color = format.background().color();
            if (color.isValid()) props += QString("<w:shd w:val=\"clear\" w:color=\"auto\" w:fill=\"%1\"/>").arg(colorToDocx(color));
        }
        if (format.fontPointSize() > 0) {
            props += QString("<w:sz w:val=\"%1\"/>").arg(qRound(format.fontPointSize() * 2.0));
        }
        if (!format.fontFamily().isEmpty()) {
            const QString family = escapeXml(format.fontFamily());
            props += QString("<w:rFonts w:ascii=\"%1\" w:hAnsi=\"%1\"/>").arg(family);
        }
        return props.isEmpty() ? QString() : "<w:rPr>" + props + "</w:rPr>";
    };
    auto startCellParagraph = [&](const QXmlStreamAttributes& attrs, const QString& tag) {
        closeParagraph();
        xml += "<w:p>";
        inParagraph = true;
        ++paragraphCount;
        QString align = attrs.value("align").toString().toLower();
        const QString style = attrs.value("style").toString();
        const QRegularExpression alignRegex("text-align\\s*:\\s*(left|center|right|justify)", QRegularExpression::CaseInsensitiveOption);
        const auto alignMatch = alignRegex.match(style);
        if (alignMatch.hasMatch()) align = alignMatch.captured(1).toLower();
        if (tag == "h1" || tag == "h2" || tag == "h3") align = "left";
        QString pPr;
        if (align == "center") pPr = "<w:jc w:val=\"center\"/>";
        else if (align == "right") pPr = "<w:jc w:val=\"right\"/>";
        else if (align == "justify") pPr = "<w:jc w:val=\"both\"/>";
        if (tag == "h1" || tag == "h2" || tag == "h3") {
            pPr += QString("<w:spacing w:before=\"%1\" w:after=\"80\"/>").arg(tag == "h1" ? 180 : 120);
        }
        if (!pPr.isEmpty()) xml += "<w:pPr>" + pPr + "</w:pPr>";
    };
    auto parseStyle = [&](QTextCharFormat& format, const QString& style) {
        const QStringList declarations = style.split(';', Qt::SkipEmptyParts);
        for (const QString& declaration : declarations) {
            const int colon = declaration.indexOf(':');
            if (colon < 0) continue;
            const QString key = declaration.left(colon).trimmed().toLower();
            const QString value = declaration.mid(colon + 1).trimmed();
            if (key == "color") {
                QColor c(value);
                if (c.isValid()) format.setForeground(c);
            } else if (key == "background-color") {
                QColor c(value);
                if (c.isValid()) format.setBackground(c);
            } else if (key == "font-weight" && (value == "bold" || value.toInt() >= 600)) {
                format.setFontWeight(QFont::Bold);
            } else if (key == "font-style" && value.contains("italic")) {
                format.setFontItalic(true);
            } else if (key == "text-decoration" && value.contains("underline")) {
                format.setFontUnderline(true);
            } else if (key == "font-family") {
                format.setFontFamily(value.section(',', 0, 0).remove('"').remove('\'').trimmed());
            } else if (key == "font-size") {
                const QString number = value;
                bool ok = false;
                qreal size = number.left(number.indexOf(QRegularExpression("[^0-9.]"))).toDouble(&ok);
                if (ok) {
                    if (number.contains("px")) size *= 0.75;
                    format.setFontPointSize(size);
                }
            }
        }
    };

    QXmlStreamReader reader(html);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            const QString tag = reader.name().toString().toLower();
            if (tag == "body") {
                inBody = true;
                stack.append({tag, currentFormat()});
                continue;
            }
            if (!inBody) continue;

            QTextCharFormat format = currentFormat();
            const auto attrs = reader.attributes();
            parseStyle(format, attrs.value("style").toString());
            if (tag == "b" || tag == "strong") format.setFontWeight(QFont::Bold);
            if (tag == "i" || tag == "em") format.setFontItalic(true);
            if (tag == "u") format.setFontUnderline(true);
            stack.append({tag, format});

            if (tag == "table") {
                closeParagraph();
                ++tableIndex;
                const QString tableStyle = attrs.value("style").toString();
                const bool hasBorders = attrs.value("border").toInt() > 0
                    || QRegularExpression("border(?:-top|-right|-bottom|-left)?\\s*:\\s*[1-9]")
                           .match(tableStyle, QRegularExpression::CaseInsensitiveOption).hasMatch();
                tableHasBorders.append(hasBorders);
                xml += "<w:tbl><w:tblPr><w:tblW w:w=\"5000\" w:type=\"pct\"/><w:tblBorders>";
                const QString borderValue = hasBorders
                    ? "w:val=\"single\" w:sz=\"6\" w:color=\"000000\""
                    : "w:val=\"nil\"";
                xml += QString("<w:top %1/><w:left %1/><w:bottom %1/><w:right %1/><w:insideH %1/><w:insideV %1/>")
                           .arg(borderValue);
                xml += "</w:tblBorders></w:tblPr>";
            } else if (tag == "tr") {
                closeParagraph();
                xml += "<w:tr>";
            } else if (tag == "td" || tag == "th") {
                closeParagraph();
                xml += "<w:tc><w:tcPr>";
                bool spanOk = false;
                const int span = attrs.value("colspan").toInt(&spanOk);
                if (spanOk && span > 1) xml += QString("<w:gridSpan w:val=\"%1\"/>").arg(span);
                const QString cellStyle = attrs.value("style").toString();
                  if (!tableHasBorders.isEmpty() && tableHasBorders.last()) {
                      xml += "<w:tcBorders><w:top w:val=\"single\" w:sz=\"6\" w:color=\"000000\"/>"
                          "<w:left w:val=\"single\" w:sz=\"6\" w:color=\"000000\"/>"
                          "<w:bottom w:val=\"single\" w:sz=\"6\" w:color=\"000000\"/>"
                          "<w:right w:val=\"single\" w:sz=\"6\" w:color=\"000000\"/></w:tcBorders>";
                  } else if (QRegularExpression("border(?:-top|-right|-bottom|-left)?\\s*:\\s*[1-9]")
                           .match(cellStyle, QRegularExpression::CaseInsensitiveOption).hasMatch()) {
                      xml += "<w:tcBorders><w:top w:val=\"single\" w:sz=\"4\" w:color=\"000000\"/>"
                          "<w:left w:val=\"single\" w:sz=\"4\" w:color=\"000000\"/>"
                          "<w:bottom w:val=\"single\" w:sz=\"4\" w:color=\"000000\"/>"
                          "<w:right w:val=\"single\" w:sz=\"4\" w:color=\"000000\"/></w:tcBorders>";
                  }
                if (cellStyle.contains("background", Qt::CaseInsensitive)) {
                    QTextCharFormat cellFormat;
                    parseStyle(cellFormat, cellStyle);
                    if (cellFormat.background().style() != Qt::NoBrush) {
                        xml += QString("<w:shd w:val=\"clear\" w:color=\"auto\" w:fill=\"%1\"/>").arg(colorToDocx(cellFormat.background().color()));
                    }
                }
                xml += "</w:tcPr>";
                cellHasParagraph.append(false);
            } else if (tag == "p" || tag == "div" || tag == "h1" || tag == "h2" || tag == "h3") {
                startCellParagraph(attrs, tag);
            } else if (tag == "br") {
                openParagraph();
                xml += "<w:r><w:br/></w:r>";
            } else if (tag == "img") {
                openParagraph();
                const QString source = attrs.value("src").toString();
                QImage image;
                if (source.startsWith("data:image", Qt::CaseInsensitive)) {
                    const QByteArray encoded = source.section(',', 1).toLatin1();
                    image.loadFromData(QByteArray::fromBase64(encoded));
                } else {
                    const QVariant resource = document->resource(QTextDocument::ImageResource, QUrl(source));
                    if (resource.canConvert<QImage>()) image = resource.value<QImage>();
                    else if (resource.canConvert<QPixmap>()) image = resource.value<QPixmap>().toImage();
                }
                if (!image.isNull()) {
                    QByteArray png;
                    QBuffer buffer(&png);
                    buffer.open(QIODevice::WriteOnly);
                    if (image.save(&buffer, "PNG")) {
                        m_embeddedImages.append(png);
                        const int relationshipId = imageIndex + 2;
                        ++imageIndex;
                        const qreal widthPt = image.width() * 0.75;
                        const qreal heightPt = image.height() * 0.75;
                        xml += QString("<w:r><w:drawing><wp:inline><wp:extent cx=\"%1\" cy=\"%2\"/><wp:docPr id=\"%3\" name=\"Image %3\"/><a:graphic><a:graphicData uri=\"http://schemas.openxmlformats.org/drawingml/2006/picture\"><pic:pic><pic:nvPicPr><pic:cNvPr id=\"%3\" name=\"image_%3.png\"/><pic:cNvPicPr/></pic:nvPicPr><pic:blipFill><a:blip r:embed=\"rId%4\"/><a:stretch><a:fillRect/></a:stretch></pic:blipFill><pic:spPr><a:xfrm><a:off x=\"0\" y=\"0\"/><a:ext cx=\"%1\" cy=\"%2\"/></a:xfrm><a:prstGeom prst=\"rect\"><a:avLst/></a:prstGeom></pic:spPr></pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing></w:r>")
                                   .arg(qRound64(widthPt * emuPerPoint)).arg(qRound64(heightPt * emuPerPoint))
                                   .arg(imageIndex).arg(relationshipId);
                    }
                }
            }
        } else if (reader.isCharacters() && !reader.isWhitespace() && inBody) {
            const QString text = reader.text().toString();
            if (!text.isEmpty()) {
                openParagraph();
                xml += "<w:r>" + runProperties(currentFormat()) + "<w:t xml:space=\"preserve\">"
                    + escapeText(text) + "</w:t></w:r>";
            }
        } else if (reader.isEndElement()) {
            const QString tag = reader.name().toString().toLower();
            if (tag == "body") {
                closeParagraph();
                inBody = false;
            } else if (inBody) {
                if (tag == "p" || tag == "h1" || tag == "h2" || tag == "h3") {
                    closeParagraph();
                } else if (tag == "td" || tag == "th") {
                    closeParagraph();
                    if (!cellHasParagraph.isEmpty()) {
                        if (!cellHasParagraph.last()) xml += "<w:p/>";
                        cellHasParagraph.removeLast();
                    }
                    xml += "</w:tc>";
                } else if (tag == "tr") {
                    closeParagraph();
                    xml += "</w:tr>";
                } else if (tag == "table") {
                    closeParagraph();
                    xml += "</w:tbl>";
                    if (!tableHasBorders.isEmpty()) tableHasBorders.removeLast();
                } else if (tag == "div") {
                    closeParagraph();
                }
            }
            if (!stack.isEmpty()) stack.removeLast();
        }
    }

    if (reader.hasError()) return {};

    closeParagraph();
    const qreal twipsPerMm = 1440.0 / 25.4;
    const QSizeF a4 = QPageSize(QPageSize::A4).size(QPageSize::Point);
    const int widthTwips = qRound(a4.width() * 20.0);
    const int heightTwips = qRound(a4.height() * 20.0);
    xml += QString("<w:sectPr><w:pgSz w:w=\"%1\" w:h=\"%2\"/><w:pgMar w:top=\"%3\" w:right=\"%4\" w:bottom=\"%5\" w:left=\"%6\"/></w:sectPr>")
               .arg(widthTwips).arg(heightTwips)
               .arg(qRound(preset.marginTop() * twipsPerMm))
               .arg(qRound(preset.marginRight() * twipsPerMm))
               .arg(qRound(preset.marginBottom() * twipsPerMm))
               .arg(qRound(preset.marginLeft() * twipsPerMm));
    xml += "</w:body></w:document>";

    Q_UNUSED(paragraphCount)
    Q_UNUSED(tableIndex)
    return xml.toUtf8();
}

QVector<QByteArray> DocxExporter::renderPages(QTextDocument* document,
                                              const FormatPreset& preset,
                                              QSize* pagePixels,
                                              QSizeF* pagePoints) const
{
    if (!document || !pagePixels || !pagePoints) return {};

    const QSizeF paperPoints = QPageSize(QPageSize::A4).size(QPageSize::Point);
    Q_UNUSED(preset)
    *pagePoints = paperPoints;
    constexpr qreal rasterScale = 2.0; // 144 dpi
    *pagePixels = QSize(qRound(paperPoints.width() * rasterScale),
                        qRound(paperPoints.height() * rasterScale));

    document->setPageSize(*pagePoints);
    const int pageCount = qMax(1, document->pageCount());
    QVector<QByteArray> images;
    images.reserve(pageCount);

    for (int page = 0; page < pageCount; ++page) {
        QImage image(*pagePixels, QImage::Format_RGB32);
        image.fill(Qt::white);

        QPainter painter(&image);
        painter.scale(rasterScale, rasterScale);
        const qreal top = page * paperPoints.height();
        painter.translate(0, -top);
        document->drawContents(&painter, QRectF(0, top, paperPoints.width(), paperPoints.height()));
        painter.end();

        QByteArray png;
        QBuffer buffer(&png);
        if (!buffer.open(QIODevice::WriteOnly) || !image.save(&buffer, "PNG")) return {};
        images.append(png);
    }

    return images;
}

QByteArray DocxExporter::generatePreviewDocument(int pageCount,
                                                  const QSize& pagePixels,
                                                  const QSizeF& pagePoints,
                                                  const FormatPreset& preset)
{
    Q_UNUSED(pagePixels)
    Q_UNUSED(preset)
    const qreal emuPerPoint = 12700.0;
    const int pageWidthTwips = qRound(QPageSize(QPageSize::A4).size(QPageSize::Point).width() * 20.0);
    const int pageHeightTwips = qRound(QPageSize(QPageSize::A4).size(QPageSize::Point).height() * 20.0);
    const qint64 imageWidthEmu = qRound64(pagePoints.width() * emuPerPoint);
    const qint64 imageHeightEmu = qRound64(pagePoints.height() * emuPerPoint);

    QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n";
    xml += "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" "
           "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" "
           "xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" "
           "xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" "
           "xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\"><w:body>";

    for (int page = 0; page < pageCount; ++page) {
        const int relationId = page + 2; // rId1 is the styles relationship
        xml += "<w:p><w:pPr><w:spacing w:before=\"0\" w:after=\"0\"/>";
        if (page > 0) xml += "<w:pageBreakBefore/>";
        xml += "</w:pPr><w:r><w:drawing><wp:inline distT=\"0\" distB=\"0\" distL=\"0\" distR=\"0\">";
        xml += QString("<wp:extent cx=\"%1\" cy=\"%2\"/><wp:docPr id=\"%3\" name=\"Preview page %3\"/>")
                   .arg(imageWidthEmu).arg(imageHeightEmu).arg(page + 1);
        xml += "<wp:cNvGraphicFramePr><a:graphicFrameLocks noChangeAspect=\"1\"/></wp:cNvGraphicFramePr>";
        xml += "<a:graphic><a:graphicData uri=\"http://schemas.openxmlformats.org/drawingml/2006/picture\"><pic:pic>";
        xml += "<pic:nvPicPr><pic:cNvPr id=\"0\" name=\"preview.png\"/><pic:cNvPicPr/></pic:nvPicPr>";
        xml += QString("<pic:blipFill><a:blip r:embed=\"rId%1\"/><a:stretch><a:fillRect/></a:stretch></pic:blipFill>").arg(relationId);
        xml += QString("<pic:spPr><a:xfrm><a:off x=\"0\" y=\"0\"/><a:ext cx=\"%1\" cy=\"%2\"/></a:xfrm><a:prstGeom prst=\"rect\"><a:avLst/></a:prstGeom></pic:spPr>")
                   .arg(imageWidthEmu).arg(imageHeightEmu);
        xml += "</pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing></w:r></w:p>";
    }

    xml += QString("<w:sectPr><w:pgSz w:w=\"%1\" w:h=\"%2\"/><w:pgMar w:top=\"0\" w:right=\"0\" w:bottom=\"0\" w:left=\"0\" w:header=\"0\" w:footer=\"0\" w:gutter=\"0\"/></w:sectPr>")
               .arg(pageWidthTwips).arg(pageHeightTwips);
    xml += "</w:body></w:document>";
    return xml.toUtf8();
}

QByteArray DocxExporter::generateDocument(
    const QVector<RiggingCalculation*>& calculations,
    const FormatPreset& preset,
    const QString& fileName)
{
    QString xml;
    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n";
        xml += "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" "
            "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" "
            "xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" "
            "xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" "
            "xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\">";
    xml += "<w:body>";

    // Title
    xml += "<w:p><w:pPr><w:pStyle w:val=\"Heading1\"/></w:pPr>";
    xml += "<w:r><w:t>" + escapeXml(preset.headerText().isEmpty() ? "Plano de Rigging" : preset.headerText()) + "</w:t></w:r>";
    xml += "</w:p>";

    // File info
    xml += "<w:p><w:r><w:rPr><w:color w:val=\"757575\"/><w:sz w:val=\"20\"/></w:rPr>";
    xml += "<w:t xml:space=\"preserve\">Arquivo: " + escapeXml(fileName) + "    |    Data: " +
           QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm") + "    |    Formatação: " +
           escapeXml(preset.name()) + "</w:t></w:r></w:p>";

    // Empty line
    xml += "<w:p/>";

    // Summary heading
    xml += "<w:p><w:pPr><w:pStyle w:val=\"Heading2\"/></w:pPr>";
    xml += "<w:r><w:t>Resumo dos Resultados</w:t></w:r></w:p>";

    // Summary table
    xml += "<w:tbl><w:tblPr>";
    xml += "<w:tblStyle w:val=\"TableGrid\"/>";
    xml += "<w:tblW w:w=\"5000\" w:type=\"pct\"/>";
    xml += "<w:tblBorders>";
    xml += "<w:top w:val=\"single\" w:sz=\"4\" w:color=\"BDBDBD\"/>";
    xml += "<w:left w:val=\"single\" w:sz=\"4\" w:color=\"BDBDBD\"/>";
    xml += "<w:bottom w:val=\"single\" w:sz=\"4\" w:color=\"BDBDBD\"/>";
    xml += "<w:right w:val=\"single\" w:sz=\"4\" w:color=\"BDBDBD\"/>";
    xml += "<w:insideH w:val=\"single\" w:sz=\"4\" w:color=\"BDBDBD\"/>";
    xml += "<w:insideV w:val=\"single\" w:sz=\"4\" w:color=\"BDBDBD\"/>";
    xml += "</w:tblBorders></w:tblPr>";

    // Table header row
    xml += "<w:tr>";
    for (const auto& header : {"#", "Cálculo", "Fórmula", "Resultado"}) {
        xml += "<w:tc><w:tcPr><w:shd w:val=\"clear\" w:color=\"auto\" w:fill=\"" +
               colorToDocx(preset.colorTableHeader()) + "\"/></w:tcPr>";
        xml += "<w:p><w:r><w:rPr><w:b/><w:color w:val=\"FFFFFF\"/><w:sz w:val=\"20\"/></w:rPr>";
        xml += "<w:t>" + QString(header) + "</w:t></w:r></w:p></w:tc>";
    }
    xml += "</w:tr>";

    // Table rows
    for (const auto* calc : calculations) {
        xml += "<w:tr>";

        // ID
        xml += "<w:tc><w:p><w:r><w:rPr><w:sz w:val=\"20\"/></w:rPr>";
        xml += "<w:t>" + QString::number(calc->id()) + "</w:t></w:r></w:p></w:tc>";

        // Name
        xml += "<w:tc><w:p><w:r><w:rPr><w:b/><w:sz w:val=\"20\"/></w:rPr>";
        xml += "<w:t>" + escapeXml(calc->name()) + "</w:t></w:r></w:p></w:tc>";

        // Formula
        xml += "<w:tc><w:p><w:r><w:rPr><w:rFonts w:ascii=\"Consolas\" w:hAnsi=\"Consolas\"/>";
        xml += "<w:color w:val=\"" + colorToDocx(preset.colorAccent()) + "\"/><w:sz w:val=\"20\"/></w:rPr>";
        xml += "<w:t>" + escapeXml(calc->formulaText()) + "</w:t></w:r></w:p></w:tc>";

        // Result
        xml += "<w:tc><w:p><w:r><w:rPr><w:b/><w:sz w:val=\"20\"/>";
        if (calc->status() == RiggingCalculation::Computed) {
            xml += "<w:color w:val=\"2E7D32\"/>";
            xml += "</w:rPr><w:t>" + QString::number(calc->result(), 'g', 6) + " " +
                   escapeXml(calc->resultUnit()) + "</w:t>";
        } else {
            xml += "<w:color w:val=\"9E9E9E\"/>";
            xml += "</w:rPr><w:t>—</w:t>";
        }
        xml += "</w:r></w:p></w:tc>";

        xml += "</w:tr>";
    }
    xml += "</w:tbl>";

    // Spacer
    xml += "<w:p/>";

    // Detailed sections
    xml += "<w:p><w:pPr><w:pStyle w:val=\"Heading2\"/></w:pPr>";
    xml += "<w:r><w:t>Detalhamento dos Cálculos</w:t></w:r></w:p>";

    for (const auto* calc : calculations) {
        // Calc name
        xml += "<w:p><w:pPr><w:pBdr><w:bottom w:val=\"single\" w:sz=\"4\" w:color=\"E0E0E0\"/>"
               "</w:pBdr></w:pPr>";
        xml += "<w:r><w:rPr><w:b/><w:sz w:val=\"24\"/><w:color w:val=\"" +
               colorToDocx(preset.colorAccent()) + "\"/></w:rPr>";
        xml += "<w:t>" + QString::number(calc->id()) + ". " + escapeXml(calc->name()) + "</w:t></w:r></w:p>";

        // Description
        if (!calc->description().isEmpty()) {
            const QStringList lines = calc->description().split('\n');
            for (const auto& line : lines) {
                if (!line.trimmed().isEmpty()) {
                    xml += "<w:p><w:r><w:rPr><w:i/><w:color w:val=\"555555\"/><w:sz w:val=\"20\"/></w:rPr>";
                    xml += "<w:t>" + escapeXml(line.trimmed()) + "</w:t></w:r></w:p>";
                }
            }
        }

        // Formula
        xml += "<w:p><w:r><w:rPr><w:rFonts w:ascii=\"Consolas\" w:hAnsi=\"Consolas\"/>";
        xml += "<w:color w:val=\"1565C0\"/><w:sz w:val=\"22\"/></w:rPr>";
        xml += "<w:t>" + escapeXml(calc->formulaText()) + "</w:t></w:r></w:p>";

        // Parameters
        const auto& params = calc->parameters();
        for (const auto& p : params) {
            xml += "<w:p><w:r><w:rPr><w:sz w:val=\"20\"/></w:rPr>";
            xml += "<w:t xml:space=\"preserve\">   " + escapeXml(p.name) + " = " +
                   QString::number(p.effectiveValue(), 'g', 6);
            if (!p.unit.isEmpty()) {
                xml += " " + escapeXml(p.unit);
            }
            xml += "</w:t></w:r></w:p>";
        }

        // Result
        if (calc->status() == RiggingCalculation::Computed) {
            xml += "<w:p><w:r><w:rPr><w:b/><w:sz w:val=\"24\"/><w:color w:val=\"2E7D32\"/></w:rPr>";
            xml += "<w:t xml:space=\"preserve\">   Resultado: " +
                   QString::number(calc->result(), 'g', 10) + " " +
                   escapeXml(calc->resultUnit()) + "</w:t></w:r></w:p>";
        }

        // Spacer
        xml += "<w:p/>";
    }

    // Footer
    xml += "<w:p><w:pPr><w:pBdr><w:top w:val=\"single\" w:sz=\"4\" w:color=\"BDBDBD\"/>"
           "</w:pBdr><w:jc w:val=\"center\"/></w:pPr>";
    xml += "<w:r><w:rPr><w:sz w:val=\"18\"/><w:color w:val=\"9E9E9E\"/></w:rPr>";
    QString footerText = preset.footerText().isEmpty() ? "RigPlan Integrator" : preset.footerText();
    xml += "<w:t>" + escapeXml(footerText) + " — " +
           QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss") +
           " — Dados validados por SHA-256</w:t></w:r></w:p>";

    xml += "</w:body></w:document>";
    return xml.toUtf8();
}

QString DocxExporter::escapeXml(const QString& text) const
{
    QString escaped = text;
    escaped.replace("&", "&amp;");
    escaped.replace("<", "&lt;");
    escaped.replace(">", "&gt;");
    escaped.replace("\"", "&quot;");
    escaped.replace("'", "&apos;");
    return escaped;
}

QString DocxExporter::colorToDocx(const QColor& color) const
{
    return color.name().mid(1).toUpper(); // Remove '#' prefix
}

// ============================================================
// Minimal ZIP writer — creates STORED (uncompressed) ZIP files
// ============================================================

// CRC-32 lookup table
static uint32_t s_crc32Table[256];
static bool s_crc32Initialized = false;

static void initCrc32Table()
{
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320u : 0u);
        }
        s_crc32Table[i] = crc;
    }
    s_crc32Initialized = true;
}

uint32_t DocxExporter::computeCrc32(const QByteArray& data)
{
    if (!s_crc32Initialized) initCrc32Table();

    uint32_t crc = 0xFFFFFFFFu;
    for (int i = 0; i < data.size(); ++i) {
        uint8_t byte = static_cast<uint8_t>(data[i]);
        crc = (crc >> 8) ^ s_crc32Table[(crc ^ byte) & 0xFF];
    }
    return crc ^ 0xFFFFFFFFu;
}

void DocxExporter::writeLittleEndian16(QByteArray& buf, uint16_t val)
{
    buf.append(static_cast<char>(val & 0xFF));
    buf.append(static_cast<char>((val >> 8) & 0xFF));
}

void DocxExporter::writeLittleEndian32(QByteArray& buf, uint32_t val)
{
    buf.append(static_cast<char>(val & 0xFF));
    buf.append(static_cast<char>((val >> 8) & 0xFF));
    buf.append(static_cast<char>((val >> 16) & 0xFF));
    buf.append(static_cast<char>((val >> 24) & 0xFF));
}

QByteArray DocxExporter::createZip(const QVector<ZipEntry>& entriesIn)
{
    QByteArray zip;
    QVector<ZipEntry> entries = entriesIn;

    // Write local file headers + data
    for (int i = 0; i < entries.size(); ++i) {
        entries[i].offset = static_cast<uint32_t>(zip.size());

        QByteArray nameBytes = entries[i].name.toUtf8();
        uint32_t dataSize = static_cast<uint32_t>(entries[i].data.size());

        // Local file header (signature: 0x04034b50)
        writeLittleEndian32(zip, 0x04034B50);
        writeLittleEndian16(zip, 20);           // Version needed (2.0)
        writeLittleEndian16(zip, 0);            // Flags
        writeLittleEndian16(zip, 0);            // Compression: STORED
        writeLittleEndian16(zip, 0);            // Mod time
        writeLittleEndian16(zip, 0x5421);       // Mod date (some valid date)
        writeLittleEndian32(zip, entries[i].crc32);
        writeLittleEndian32(zip, dataSize);     // Compressed size
        writeLittleEndian32(zip, dataSize);     // Uncompressed size
        writeLittleEndian16(zip, static_cast<uint16_t>(nameBytes.size()));
        writeLittleEndian16(zip, 0);            // Extra field length

        zip.append(nameBytes);
        zip.append(entries[i].data);
    }

    // Central directory
    uint32_t cdStart = static_cast<uint32_t>(zip.size());

    for (const auto& entry : entries) {
        QByteArray nameBytes = entry.name.toUtf8();
        uint32_t dataSize = static_cast<uint32_t>(entry.data.size());

        // Central directory header (signature: 0x02014b50)
        writeLittleEndian32(zip, 0x02014B50);
        writeLittleEndian16(zip, 20);           // Version made by
        writeLittleEndian16(zip, 20);           // Version needed
        writeLittleEndian16(zip, 0);            // Flags
        writeLittleEndian16(zip, 0);            // Compression: STORED
        writeLittleEndian16(zip, 0);            // Mod time
        writeLittleEndian16(zip, 0x5421);       // Mod date
        writeLittleEndian32(zip, entry.crc32);
        writeLittleEndian32(zip, dataSize);     // Compressed size
        writeLittleEndian32(zip, dataSize);     // Uncompressed size
        writeLittleEndian16(zip, static_cast<uint16_t>(nameBytes.size()));
        writeLittleEndian16(zip, 0);            // Extra field length
        writeLittleEndian16(zip, 0);            // Comment length
        writeLittleEndian16(zip, 0);            // Disk number
        writeLittleEndian16(zip, 0);            // Internal attrs
        writeLittleEndian32(zip, 0);            // External attrs
        writeLittleEndian32(zip, entry.offset); // Local header offset

        zip.append(nameBytes);
    }

    uint32_t cdEnd = static_cast<uint32_t>(zip.size());
    uint32_t cdSize = cdEnd - cdStart;

    // End of central directory (signature: 0x06054b50)
    writeLittleEndian32(zip, 0x06054B50);
    writeLittleEndian16(zip, 0);                // Disk number
    writeLittleEndian16(zip, 0);                // Disk with CD
    writeLittleEndian16(zip, static_cast<uint16_t>(entries.size()));
    writeLittleEndian16(zip, static_cast<uint16_t>(entries.size()));
    writeLittleEndian32(zip, cdSize);
    writeLittleEndian32(zip, cdStart);
    writeLittleEndian16(zip, 0);                // Comment length

    return zip;
}
