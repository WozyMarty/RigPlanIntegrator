#pragma once

#include <QObject>
#include <QString>
#include <QTextDocument>
#include <QVector>

class FormatPreset;

/**
 * @brief Exports a QTextDocument to PDF using QPrinter.
 */
class PdfExporter : public QObject {
    Q_OBJECT

public:
    explicit PdfExporter(QObject* parent = nullptr);

    /**
     * @brief Export document to PDF file.
     * @param doc The text document to export
     * @param filePath Output file path (should end with .pdf)
     * @param preset Format preset for margins
     * @return true if export succeeded
     */
    bool exportToPdf(QTextDocument* doc, const QString& filePath,
                     const FormatPreset& preset);

    /**
     * @brief Export multiple section documents to a single PDF file, one section per page.
     * @param sectionDocs List of QTextDocuments (one per section). Caller owns memory.
     * @param filePath Output file path (should end with .pdf)
     * @param preset Format preset for margins
     * @return true if export succeeded
     */
    bool exportSectionsToPdf(const QVector<QTextDocument*>& sectionDocs,
                             const QString& filePath,
                             const FormatPreset& preset);

signals:
    void exportStarted();
    void exportFinished(bool success, const QString& filePath);
    void exportError(const QString& message);
};
