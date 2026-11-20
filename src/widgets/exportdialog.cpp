#include "exportdialog.h"
#include "core/documentgenerator.h"
#include "core/docxexporter.h"
#include "core/pdfexporter.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

ExportDialog::ExportDialog(const QVector<DocumentSection>& previewSections,
                           PresetManager* presetManager,
                           CalculationEngine* engine,
                           const QString& defaultDocName,
                           const QString& defaultFormat,
                           QWidget* parent)
    : QDialog(parent)
    , m_previewSections(previewSections)
    , m_presetManager(presetManager)
    , m_engine(engine)
    , m_documentName(defaultDocName.isEmpty() ? "Plano_de_Rigging" : defaultDocName)
    , m_format(defaultFormat.isEmpty() ? "DOCX" : defaultFormat.toUpper())
{
    setWindowTitle("💾 Exportação Contínua Integrada (PDF / DOCX)");
    resize(760, 560);
    populateInitialSections();
    setupUi();
    updateTable();
}

void ExportDialog::populateInitialSections()
{
    m_exportItems.clear();

    // 1. All sections currently in preview are included AND checked by default
    QSet<QString> includedPresetIds;
    for (const auto& sec : m_previewSections) {
        DocumentSection item = sec;
        item.setEnabled(true); // Pre-selecionado por padrão!
        m_exportItems.append(item);
        if (item.isPreset()) {
            includedPresetIds.insert(item.preset().id());
        }
    }

    // 2. Add other available styles from PresetManager as unchecked options
    if (m_presetManager) {
        for (const auto& preset : m_presetManager->allPresets()) {
            if (!includedPresetIds.contains(preset.id())) {
                DocumentSection item(QString(), DocumentSection::PresetStyle, preset.name());
                item.setPreset(preset);
                item.setEnabled(false); // Unchecked, available for selection!
                m_exportItems.append(item);
            }
        }
    }
}

void ExportDialog::setupUi()
{
    setStyleSheet(
        "QDialog {"
        "   background-color: #0E131F;"
        "   color: #FFFFFF;"
        "}"
        "QLabel {"
        "   color: #C9D1D9;"
        "   font-size: 12px;"
        "}"
        "QLineEdit, QComboBox {"
        "   background-color: #161B26;"
        "   color: #FFFFFF;"
        "   border: 1px solid #2B384E;"
        "   border-radius: 4px;"
        "   padding: 5px 8px;"
        "   font-size: 12px;"
        "}"
        "QLineEdit:focus, QComboBox:focus {"
        "   border: 1px solid #1F6FEB;"
        "}"
        "QTableWidget {"
        "   background-color: #161B26;"
        "   color: #FFFFFF;"
        "   border: 1px solid #2B384E;"
        "   border-radius: 6px;"
        "   gridline-color: #21262D;"
        "   selection-background-color: #1F6FEB;"
        "}"
        "QHeaderView::section {"
        "   background-color: #1F2430;"
        "   color: #90A4AE;"
        "   padding: 6px;"
        "   font-weight: bold;"
        "   border: none;"
        "   border-bottom: 1px solid #2B384E;"
        "}"
        "QPushButton {"
        "   background-color: #21262D;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 5px;"
        "   padding: 6px 14px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #30363D;"
        "}"
        "QPushButton#exportBtn {"
        "   background-color: #238636;"
        "   color: #FFFFFF;"
        "   border: 1px solid #2EA043;"
        "   font-size: 12px;"
        "   padding: 8px 20px;"
        "}"
        "QPushButton#exportBtn:hover {"
        "   background-color: #2EA043;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(14);

    QLabel* titleLabel = new QLabel("🚀 Exportar Documento Integrado Contínuo", this);
    titleLabel->setStyleSheet("font-size: 15px; font-weight: bold; color: #FFFFFF;");
    mainLayout->addWidget(titleLabel);

    QLabel* descLabel = new QLabel(
        "Os estilos e imagens presentes na Pré-Visualização já foram pré-selecionados abaixo. "
        "Você pode marcar outros estilos disponíveis ou importar novas imagens para anexar ao documento final.", this);
    descLabel->setStyleSheet("color: #8B949E; font-size: 11.5px;");
    descLabel->setWordWrap(true);
    mainLayout->addWidget(descLabel);

    // Form settings: Name & Format
    QHBoxLayout* topFormLayout = new QHBoxLayout();
    topFormLayout->setSpacing(10);

    QLabel* nameLbl = new QLabel("Nome do Documento:", this);
    nameLbl->setStyleSheet("font-weight: bold;");
    m_fileNameEdit = new QLineEdit(m_documentName, this);

    QLabel* formatLbl = new QLabel("Formato:", this);
    formatLbl->setStyleSheet("font-weight: bold;");
    m_formatCombo = new QComboBox(this);
    m_formatCombo->addItem("DOCX (.docx) - Microsoft Word", "DOCX");
    m_formatCombo->addItem("PDF (.pdf) - Documento Portátil", "PDF");
    if (m_format == "PDF") m_formatCombo->setCurrentIndex(1);

    topFormLayout->addWidget(nameLbl);
    topFormLayout->addWidget(m_fileNameEdit, 1);
    topFormLayout->addWidget(formatLbl);
    topFormLayout->addWidget(m_formatCombo);

    mainLayout->addLayout(topFormLayout);

    // Path selector
    QHBoxLayout* pathLayout = new QHBoxLayout();
    pathLayout->setSpacing(10);

    QLabel* pathLbl = new QLabel("Destino:", this);
    pathLbl->setStyleSheet("font-weight: bold;");
    m_pathEdit = new QLineEdit(this);
    QString ext = (m_format == "PDF") ? "pdf" : "docx";
    m_pathEdit->setText(QDir::current().filePath(QString("%1.%2").arg(m_documentName, ext)));

    m_browseBtn = new QPushButton("Procurar...", this);

    pathLayout->addWidget(pathLbl);
    pathLayout->addWidget(m_pathEdit, 1);
    pathLayout->addWidget(m_browseBtn);

    mainLayout->addLayout(pathLayout);

    // Table of sections to export
    QLabel* listLbl = new QLabel("📋 Seleção de Estilos, Páginas e Anexos:", this);
    listLbl->setStyleSheet("font-weight: bold; color: #58A6FF; font-size: 12px; margin-top: 6px;");
    mainLayout->addWidget(listLbl);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"Exportar", "Tipo", "Título / Estilo", "Origem / Status"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->verticalHeader()->setVisible(false);
    mainLayout->addWidget(m_table, 1);

    // Buttons below table
    QHBoxLayout* tableBtnsLayout = new QHBoxLayout();
    tableBtnsLayout->setSpacing(8);

    QPushButton* upBtn = new QPushButton("▲ Subir", this);
    QPushButton* downBtn = new QPushButton("▼ Descer", this);
    QPushButton* creaBtn = new QPushButton("📷 Importar CREA", this);
    QPushButton* swBtn = new QPushButton("📐 Importar SOLIDWORKS", this);
    QPushButton* customImgBtn = new QPushButton("➕ Importar Outra Imagem", this);

    tableBtnsLayout->addWidget(upBtn);
    tableBtnsLayout->addWidget(downBtn);
    tableBtnsLayout->addSpacing(10);
    tableBtnsLayout->addWidget(creaBtn);
    tableBtnsLayout->addWidget(swBtn);
    tableBtnsLayout->addWidget(customImgBtn);
    tableBtnsLayout->addStretch(1);

    mainLayout->addLayout(tableBtnsLayout);

    // Bottom Action Row
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setStyleSheet("color: #58A6FF; font-weight: bold; font-size: 12px;");

    QPushButton* cancelBtn = new QPushButton("Cancelar", this);
    m_exportBtn = new QPushButton("🚀 EXPORTAR AGORA", this);
    m_exportBtn->setObjectName("exportBtn");

    bottomLayout->addWidget(m_summaryLabel);
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(cancelBtn);
    bottomLayout->addWidget(m_exportBtn);

    mainLayout->addLayout(bottomLayout);

    // Connections
    connect(m_browseBtn, &QPushButton::clicked, this, &ExportDialog::onBrowsePath);
    connect(m_formatCombo, &QComboBox::currentTextChanged, this, [this](const QString&) {
        onFormatChanged(m_formatCombo->currentData().toString());
    });
    connect(m_fileNameEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        m_documentName = text.trimmed();
        QString ext = (m_formatCombo->currentData().toString() == "PDF") ? "pdf" : "docx";
        m_pathEdit->setText(QDir::current().filePath(QString("%1.%2").arg(m_documentName, ext)));
    });

    connect(upBtn, &QPushButton::clicked, this, &ExportDialog::moveUp);
    connect(downBtn, &QPushButton::clicked, this, &ExportDialog::moveDown);
    connect(creaBtn, &QPushButton::clicked, this, &ExportDialog::onImportCrea);
    connect(swBtn, &QPushButton::clicked, this, &ExportDialog::onImportSolidworks);
    connect(customImgBtn, &QPushButton::clicked, this, &ExportDialog::onImportCustomImage);

    connect(m_exportBtn, &QPushButton::clicked, this, &ExportDialog::onExecuteExport);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void ExportDialog::updateTable()
{
    m_table->setRowCount(m_exportItems.size());
    int selectedCount = 0;

    for (int i = 0; i < m_exportItems.size(); ++i) {
        const auto& item = m_exportItems[i];
        if (item.isEnabled()) selectedCount++;

        // 0. Checkbox
        QCheckBox* cb = new QCheckBox();
        cb->setChecked(item.isEnabled());
        cb->setStyleSheet("margin-left: 12px;");
        connect(cb, &QCheckBox::toggled, this, [this, i](bool checked) {
            m_exportItems[i].setEnabled(checked);
            updateTable();
        });
        m_table->setCellWidget(i, 0, cb);

        // 1. Tipo
        auto* typeItem = new QTableWidgetItem(QString("%1 %2").arg(item.typeIcon(), item.typeName()));
        typeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_table->setItem(i, 1, typeItem);

        // 2. Título
        auto* titleItem = new QTableWidgetItem(item.title());
        titleItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_table->setItem(i, 2, titleItem);

        // 3. Status
        QString statusText;
        bool inPreview = false;
        for (const auto& p : m_previewSections) {
            if (p.id() == item.id() || (p.isPreset() && item.isPreset() && p.presetId() == item.presetId())) {
                inPreview = true;
                break;
            }
        }

        if (inPreview) {
            statusText = "⭐ Presente na Pré-Visualização";
        } else if (item.isPreset()) {
            statusText = "➕ Estilo Disponível";
        } else {
            statusText = item.hasImage() ? "✓ Imagem Carregada" : "⚠️ Sem Imagem";
        }

        auto* statusItem = new QTableWidgetItem(statusText);
        statusItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_table->setItem(i, 3, statusItem);
    }

    m_summaryLabel->setText(QString("Itens selecionados: %1 de %2").arg(selectedCount).arg(m_exportItems.size()));
}

void ExportDialog::onFormatChanged(const QString& format)
{
    m_format = format;
    QString currentPath = m_pathEdit->text().trimmed();
    QString ext = (format == "PDF") ? "pdf" : "docx";

    if (!currentPath.isEmpty()) {
        QFileInfo fi(currentPath);
        QString newPath = fi.dir().filePath(QString("%1.%2").arg(fi.baseName(), ext));
        m_pathEdit->setText(newPath);
    }
}

void ExportDialog::onBrowsePath()
{
    QString format = m_formatCombo->currentData().toString();
    QString filter = (format == "PDF") ? "Documento PDF (*.pdf)" : "Documento Word (*.docx)";
    QString defaultExt = (format == "PDF") ? ".pdf" : ".docx";

    QString filePath = QFileDialog::getSaveFileName(
        this, "Selecionar Destino de Exportação", m_pathEdit->text(), filter
    );

    if (!filePath.isEmpty()) {
        if (!filePath.endsWith(defaultExt, Qt::CaseInsensitive)) {
            filePath += defaultExt;
        }
        m_pathEdit->setText(filePath);
    }
}

void ExportDialog::moveUp()
{
    int row = m_table->currentRow();
    if (row <= 0 || row >= m_exportItems.size()) return;

    std::swap(m_exportItems[row], m_exportItems[row - 1]);
    updateTable();
    m_table->selectRow(row - 1);
}

void ExportDialog::moveDown()
{
    int row = m_table->currentRow();
    if (row < 0 || row >= m_exportItems.size() - 1) return;

    std::swap(m_exportItems[row], m_exportItems[row + 1]);
    updateTable();
    m_table->selectRow(row + 1);
}

void ExportDialog::onImportCrea()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Importar Imagem CREA / ART para Exportação", "",
        "Imagens (*.png *.jpg *.jpeg *.bmp *.webp);;Todos os Arquivos (*.*)"
    );
    if (filePath.isEmpty()) return;

    DocumentSection sec(QString(), DocumentSection::CreaImage, "Anexo CREA (ART)");
    sec.setImagePath(filePath);
    sec.setEnabled(true);
    m_exportItems.prepend(sec);
    updateTable();
    m_table->selectRow(0);
}

void ExportDialog::onImportSolidworks()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Importar Desenho SOLIDWORKS para Exportação", "",
        "Imagens (*.png *.jpg *.jpeg *.bmp *.webp);;Todos os Arquivos (*.*)"
    );
    if (filePath.isEmpty()) return;

    DocumentSection sec(QString(), DocumentSection::SolidworksImage, "Desenho Técnico SOLIDWORKS");
    sec.setImagePath(filePath);
    sec.setEnabled(true);
    m_exportItems.prepend(sec);
    updateTable();
    m_table->selectRow(0);
}

void ExportDialog::onImportCustomImage()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Importar Imagem / Anexo Adicional", "",
        "Imagens (*.png *.jpg *.jpeg *.bmp *.webp);;Todos os Arquivos (*.*)"
    );
    if (filePath.isEmpty()) return;

    QFileInfo fi(filePath);
    DocumentSection sec(QString(), DocumentSection::CustomImage, fi.baseName());
    sec.setImagePath(filePath);
    sec.setEnabled(true);
    m_exportItems.append(sec);
    updateTable();
    m_table->selectRow(m_exportItems.size() - 1);
}

void ExportDialog::onExecuteExport()
{
    QVector<DocumentSection> selectedSections;
    for (const auto& item : m_exportItems) {
        if (item.isEnabled()) {
            selectedSections.append(item);
        }
    }

    if (selectedSections.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Selecione pelo menos um estilo ou anexo para exportar.");
        return;
    }

    QString targetPath = m_pathEdit->text().trimmed();
    if (targetPath.isEmpty()) {
        onBrowsePath();
        targetPath = m_pathEdit->text().trimmed();
        if (targetPath.isEmpty()) return;
    }

    QString format = m_formatCombo->currentData().toString();
    QString defaultExt = (format == "PDF") ? ".pdf" : ".docx";
    if (!targetPath.endsWith(defaultExt, Qt::CaseInsensitive)) {
        targetPath += defaultExt;
    }

    // Determine base preset from the first preset section
    FormatPreset basePreset = FormatPreset::createDefault();
    for (const auto& s : selectedSections) {
        if (s.isPreset()) {
            basePreset = s.preset();
            break;
        }
    }

    // Generate individual QTextDocuments per section (section-by-section export)
    DocumentGenerator generator;
    QVector<RiggingCalculation*> calcs = m_engine ? m_engine->calculations() : QVector<RiggingCalculation*>();

    QVector<QTextDocument*> sectionDocs;
    const int totalSections = selectedSections.size();
    for (int i = 0; i < totalSections; ++i) {
        QTextDocument* doc = generator.generateSectionDocument(
            selectedSections[i], calcs, m_documentName,
            basePreset, i + 1, totalSections
        );
        if (doc) {
            sectionDocs.append(doc);
        }
    }

    if (sectionDocs.isEmpty()) {
        QMessageBox::critical(this, "Erro", "Não foi possível estruturar as seções para exportação.");
        return;
    }

    bool success = false;
    if (format == "DOCX") {
        DocxExporter exporter;
        success = exporter.exportSectionsToDocx(sectionDocs, basePreset, targetPath);
    } else { // PDF
        PdfExporter exporter;
        success = exporter.exportSectionsToPdf(sectionDocs, targetPath, basePreset);
    }

    // Clean up section documents
    qDeleteAll(sectionDocs);

    if (success) {
        m_exportedFilePath = targetPath;
        m_exportSuccess = true;
        QMessageBox::information(
            this, "Exportação Concluída com Sucesso!",
            QString("O documento contínuo formatado foi gerado com sucesso contendo %1 seções selecionadas!\n\nArquivo:\n%2")
                .arg(selectedSections.size())
                .arg(targetPath)
        );
        accept();
    } else {
        QMessageBox::critical(this, "Erro de Gravação", "Ocorreu uma falha ao gravar o arquivo no disco.");
    }
}
