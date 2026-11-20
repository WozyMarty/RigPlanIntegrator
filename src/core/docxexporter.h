#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QTextDocument>
#include <QImage>

class FormatPreset;
class RiggingCalculation;

/**
 * @brief Exports documents to DOCX format (Office Open XML).
 *
 * Creates a valid DOCX file by generating the XML structure
 * and packaging it as a ZIP file (without external dependencies).
 */
class DocxExporter : public QObject {
    Q_OBJECT

public:
    explicit DocxExporter(QObject* parent = nullptr);

    /**
     * @brief Export calculations to a DOCX file.
     * @param calculations List of calculations
     * @param preset Format preset for styling
     * @param fileName Display name of the document
     * @param filePath Output file path (.docx)
     * @return true if export succeeded
     */
    bool exportToDocx(QTextDocument* document,
                      const FormatPreset& preset,
                      const QString& filePath);

    /**
     * @brief Export multiple section documents to a single DOCX file.
     * Each section gets its own page(s), guaranteeing clean page breaks.
     * @param sectionDocs List of QTextDocuments (one per section). Caller owns memory.
     * @param preset Format preset for styling
     * @param filePath Output file path (.docx)
     * @return true if export succeeded
     */
    bool exportSectionsToDocx(const QVector<QTextDocument*>& sectionDocs,
                              const FormatPreset& preset,
                              const QString& filePath);

signals:
    void exportStarted();
    void exportFinished(bool success, const QString& filePath);
    void exportError(const QString& message);

private:
    // DOCX XML generators
    QByteArray generateContentTypes();
    QByteArray generateRels();
    QByteArray generateDocumentRels(int imageCount);
    QByteArray generateStyles(const FormatPreset& preset);
    QByteArray generateDocument(const QVector<RiggingCalculation*>& calculations,
                                const FormatPreset& preset,
                                const QString& fileName);
    QByteArray generateEditableDocument(QTextDocument* document, const FormatPreset& preset);
    QByteArray generatePreviewDocument(int pageCount, const QSize& pagePixels,
                                       const QSizeF& pagePoints, const FormatPreset& preset);
    QVector<QByteArray> renderPages(QTextDocument* document, const FormatPreset& preset,
                                   QSize* pagePixels, QSizeF* pagePoints) const;

    // XML helpers
    QString escapeXml(const QString& text) const;
    QString colorToDocx(const QColor& color) const;

    // --- Minimal ZIP writer (no external dependencies) ---
    struct ZipEntry {
        QString name;
        QByteArray data;
        uint32_t crc32;
        uint32_t offset;
    };

    static uint32_t computeCrc32(const QByteArray& data);
    QByteArray createZip(const QVector<ZipEntry>& entries);

    static void writeLittleEndian16(QByteArray& buf, uint16_t val);
    static void writeLittleEndian32(QByteArray& buf, uint32_t val);

    QVector<QByteArray> m_embeddedImages;
};
