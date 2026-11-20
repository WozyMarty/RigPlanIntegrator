#include "pdfexporter.h"
#include "models/formatpreset.h"
#include "utils/constants.h"
#include <QPrinter>
#include <QPainter>
#include <QPageLayout>
#include <QMarginsF>
#include <QFileInfo>
#include <QPageSize>
#include <QAbstractTextDocumentLayout>

PdfExporter::PdfExporter(QObject* parent)
    : QObject(parent)
{
}

bool PdfExporter::exportToPdf(QTextDocument* doc, const QString& filePath,
                               const FormatPreset& preset)
{
    emit exportStarted();

    if (!doc) {
        emit exportError("Documento inválido.");
        emit exportFinished(false, filePath);
        return false;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageSize(QPageSize(QPageSize::A4));

    QMarginsF margins(
        preset.marginLeft(),
        preset.marginTop(),
        preset.marginRight(),
        preset.marginBottom()
    );
    printer.setPageMargins(margins, QPageLayout::Millimeter);

    printer.setResolution(Constants::PDF_DPI);
    printer.setColorMode(QPrinter::Color);

    // Set the document page size to match printer
    doc->setPageSize(printer.pageRect(QPrinter::Point).size());

    doc->print(&printer);

    QFileInfo fi(filePath);
    if (fi.exists() && fi.size() > 0) {
        emit exportFinished(true, filePath);
        return true;
    } else {
        emit exportError("Falha ao salvar o arquivo PDF.");
        emit exportFinished(false, filePath);
        return false;
    }
}

bool PdfExporter::exportSectionsToPdf(const QVector<QTextDocument*>& sectionDocs,
                                       const QString& filePath,
                                       const FormatPreset& preset)
{
    emit exportStarted();

    if (sectionDocs.isEmpty()) {
        emit exportError("Nenhuma seção para exportar.");
        emit exportFinished(false, filePath);
        return false;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageSize(QPageSize(QPageSize::A4));

    QMarginsF margins(
        qMin(preset.marginLeft(), 4),
        qMin(preset.marginTop(), 4),
        qMin(preset.marginRight(), 4),
        qMin(preset.marginBottom(), 4)
    );
    printer.setPageMargins(margins, QPageLayout::Millimeter);
    printer.setResolution(Constants::PDF_DPI);
    printer.setColorMode(QPrinter::Color);

    QSizeF pageSize = printer.pageRect(QPrinter::Point).size();

    // We need a QPainter to manually render each section as separate page(s)
    QPainter painter;
    if (!painter.begin(&printer)) {
        emit exportError("Não foi possível iniciar a renderização PDF.");
        emit exportFinished(false, filePath);
        return false;
    }

    for (int s = 0; s < sectionDocs.size(); ++s) {
        QTextDocument* doc = sectionDocs[s];
        if (!doc) continue;

        doc->setPageSize(pageSize);
        const int sectionPageCount = doc->pageCount();

        for (int p = 0; p < sectionPageCount; ++p) {
            if (s > 0 || p > 0) {
                printer.newPage();
            }

            // Calculate the clip rect for this page of the section document
            QRectF sourceRect(0, p * pageSize.height(), pageSize.width(), pageSize.height());

            // Scale from point coordinates to device coordinates
            const qreal scaleX = printer.pageRect(QPrinter::DevicePixel).width() / pageSize.width();
            const qreal scaleY = printer.pageRect(QPrinter::DevicePixel).height() / pageSize.height();

            painter.save();
            painter.scale(scaleX, scaleY);
            painter.translate(0, -p * pageSize.height());

            // Use QAbstractTextDocumentLayout for precise rendering
            QAbstractTextDocumentLayout::PaintContext ctx;
            ctx.clip = sourceRect;
            doc->documentLayout()->draw(&painter, ctx);

            painter.restore();
        }
    }

    painter.end();

    QFileInfo fi(filePath);
    if (fi.exists() && fi.size() > 0) {
        emit exportFinished(true, filePath);
        return true;
    } else {
        emit exportError("Falha ao salvar o arquivo PDF.");
        emit exportFinished(false, filePath);
        return false;
    }
}
