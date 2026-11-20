#pragma once

#include <QObject>
#include <QTextDocument>
#include "models/riggingcalculation.h"
#include "models/formatpreset.h"
#include "models/documentsection.h"

/**
 * @brief Generates a formatted QTextDocument from calculation data and presets.
 *
 * The document is used both for live continuous preview and as the source for export.
 */
class DocumentGenerator : public QObject {
    Q_OBJECT

public:
    explicit DocumentGenerator(QObject* parent = nullptr);

    /**
     * @brief Generate a complete document from calculations and preset.
     */
    QTextDocument* generate(const QVector<RiggingCalculation*>& calculations,
                            const FormatPreset& preset,
                            const QString& fileName);

    /**
     * @brief Generate the document as an HTML string (for preview).
     */
    QString generateHtml(const QVector<RiggingCalculation*>& calculations,
                         const FormatPreset& preset,
                         const QString& fileName);

    /**
     * @brief Generate a unified continuous document from multiple sections (preset styles & images).
     */
    QTextDocument* generateContinuous(const QVector<DocumentSection>& sections,
                                      const QVector<RiggingCalculation*>& calculations,
                                      const QString& fileName);

    /**
     * @brief Generate unified continuous HTML string for preview.
     */
    QString generateContinuousHtml(const QVector<DocumentSection>& sections,
                                   const QVector<RiggingCalculation*>& calculations,
                                   const QString& fileName);

    /**
     * @brief Build an engineering image attachment page (CREA, SolidWorks, or custom).
     */
    QString buildImagePage(const DocumentSection& section, int pageNum, int totalPages,
                           const FormatPreset& fallbackPreset);

    /**
     * @brief Generate a standalone HTML string for a single section (for section-by-section export).
     */
    QString generateSectionHtml(const DocumentSection& section,
                                const QVector<RiggingCalculation*>& calculations,
                                const QString& fileName,
                                const FormatPreset& basePreset,
                                int pageNum, int totalPages);

    /**
     * @brief Generate a standalone QTextDocument for a single section.
     */
    QTextDocument* generateSectionDocument(const DocumentSection& section,
                                           const QVector<RiggingCalculation*>& calculations,
                                           const QString& fileName,
                                           const FormatPreset& basePreset,
                                           int pageNum, int totalPages);

private:
    QString buildPageHeader(const FormatPreset& preset);
    QString buildPageFooter(int pageNum);
    QString buildMemorialPages(const QVector<RiggingCalculation*>& calculations,
                               const FormatPreset& preset);
    QString buildPlanoRiggingPage(const QVector<RiggingCalculation*>& calculations,
                                  const FormatPreset& preset);
    QString buildStandardDocument(const QVector<RiggingCalculation*>& calculations,
                                  const FormatPreset& preset,
                                  const QString& fileName);
    QString buildCalculationSection(const RiggingCalculation* calc, const FormatPreset& preset);
    QString buildSummaryTable(const QVector<RiggingCalculation*>& calculations,
                              const FormatPreset& preset);
    QString buildFooter(const FormatPreset& preset);
    QString tableStyleCss(const FormatPreset& preset);
    static QString makeFraction(const QString& num, const QString& den,
                                const QString& lineColor = "#000000",
                                const QString& textColor = "#000000");
    static QString getBase64Image(const QString& qrcPath, const QString& localPath);
};
