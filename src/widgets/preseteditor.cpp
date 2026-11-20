#include "preseteditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QColorDialog>
#include <QFontDatabase>
#include <QGroupBox>
#include "core/documentgenerator.h"
#include "models/riggingcalculation.h"

PresetEditor::PresetEditor(QWidget* parent)
    : QWidget(parent)
    , m_manager(nullptr)
{
    setupUi();
}

PresetEditor::PresetEditor(PresetManager* manager, QWidget* parent)
    : QWidget(parent)
    , m_manager(nullptr)
{
    setupUi();
    setManager(manager);
}

void PresetEditor::setManager(PresetManager* manager) {
    m_manager = manager;
}

void PresetEditor::setupUi() {
    setStyleSheet(
        "QWidget#editorMain {"
        "   background-color: #0D1117;"
        "}"
        "QLabel#editorTitle {"
        "   color: #FFFFFF;"
        "   font-weight: 800;"
        "   font-size: 18px;"
        "}"
        "QGroupBox {"
        "   color: #58A6FF;"
        "   font-weight: bold;"
        "   border: 1px solid #30363D;"
        "   border-radius: 8px;"
        "   margin-top: 10px;"
        "   padding-top: 14px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   padding: 0 8px;"
        "}"
        "QLineEdit, QSpinBox, QComboBox {"
        "   background-color: #161B22;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 5px;"
        "}"
        "QPushButton#saveBtn {"
        "   background-color: #2E7D32;"
        "   color: white;"
        "   font-weight: bold;"
        "   border-radius: 6px;"
        "   padding: 8px 20px;"
        "}"
        "QPushButton#saveBtn:hover {"
        "   background-color: #388E3C;"
        "}"
        "QPushButton#cancelBtn {"
        "   background-color: #21262D;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 8px 16px;"
        "}"
        "QPushButton#cancelBtn:hover {"
        "   background-color: #30363D;"
        "}"
    );

    setObjectName("editorMain");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 16, 20, 16);
    mainLayout->setSpacing(12);

    // Header
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("🛠️ Customizador de Aparência (Editor de Estilo)", this);
    titleLabel->setObjectName("editorTitle");

    m_saveBtn = new QPushButton("💾 Salvar Estilo", this);
    m_saveBtn->setObjectName("saveBtn");

    m_cancelBtn = new QPushButton("Cancelar", this);
    m_cancelBtn->setObjectName("cancelBtn");

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch(1);
    headerLayout->addWidget(m_saveBtn);
    headerLayout->addWidget(m_cancelBtn);

    mainLayout->addLayout(headerLayout);

    // Main 2-Column Split: Controls Left, Live Preview Right
    QHBoxLayout* contentSplit = new QHBoxLayout();
    contentSplit->setSpacing(16);

    QVBoxLayout* controlsLayout = new QVBoxLayout();
    controlsLayout->setSpacing(10);

    // 1. Identity Group
    QGroupBox* idGroup = new QGroupBox("Identificação do Estilo", this);
    QFormLayout* idLayout = new QFormLayout(idGroup);

    m_nameEdit = new QLineEdit(this);
    idLayout->addRow("Nome do Estilo:", m_nameEdit);

    m_layoutCombo = new QComboBox(this);
    m_layoutCombo->addItem("📄 Memorial de Cálculos (Folhas com Frações)", static_cast<int>(FormatPreset::MemorialLayout));
    m_layoutCombo->addItem("📋 Plano de Rigging (Tabela Executiva / Folha Completa)", static_cast<int>(FormatPreset::PlanoRiggingLayout));
    m_layoutCombo->addItem("📊 Relatório Executivo (Tabelas e Resumo)", static_cast<int>(FormatPreset::StandardLayout));
    idLayout->addRow("Modelo / Tipo:", m_layoutCombo);

    controlsLayout->addWidget(idGroup);

    // 2. Typography Group
    QGroupBox* typeGroup = new QGroupBox("Tipografia", this);
    QFormLayout* typeLayout = new QFormLayout(typeGroup);

    m_fontCombo = new QComboBox(this);
    m_fontCombo->addItems({"Arial", "Calibri", "Helvetica", "Times New Roman", "Consolas", "Segoe UI", "Trebuchet MS"});

    m_fontSizeTitleSpin = new QSpinBox(this);
    m_fontSizeTitleSpin->setRange(12, 36);

    m_fontSizeSubtitleSpin = new QSpinBox(this);
    m_fontSizeSubtitleSpin->setRange(10, 24);

    m_fontSizeBodySpin = new QSpinBox(this);
    m_fontSizeBodySpin->setRange(8, 18);

    typeLayout->addRow("Família de Fonte:", m_fontCombo);
    typeLayout->addRow("Tam. Título (pt):", m_fontSizeTitleSpin);
    typeLayout->addRow("Tam. Subtítulo (pt):", m_fontSizeSubtitleSpin);
    typeLayout->addRow("Tam. Corpo (pt):", m_fontSizeBodySpin);

    controlsLayout->addWidget(typeGroup);

    // 3. Colors Group
    QGroupBox* colorGroup = new QGroupBox("Paleta de Cores", this);
    QFormLayout* colorLayout = new QFormLayout(colorGroup);

    m_colorTitleBtn = new QPushButton(this);
    m_colorAccentBtn = new QPushButton(this);
    m_colorTableHeaderBtn = new QPushButton(this);
    m_colorBodyBtn = new QPushButton(this);

    colorLayout->addRow("Cor do Título:", m_colorTitleBtn);
    colorLayout->addRow("Cor de Destaque:", m_colorAccentBtn);
    colorLayout->addRow("Cor Cab. Tabela:", m_colorTableHeaderBtn);
    colorLayout->addRow("Cor do Texto:", m_colorBodyBtn);

    controlsLayout->addWidget(colorGroup);

    // 4. Margins & Content Group
    QGroupBox* marginGroup = new QGroupBox("Margens (mm) & Cabeçalho/Rodapé", this);
    QFormLayout* marginLayout = new QFormLayout(marginGroup);

    m_marginTopSpin = new QSpinBox(this); m_marginTopSpin->setRange(5, 50);
    m_marginBottomSpin = new QSpinBox(this); m_marginBottomSpin->setRange(5, 50);
    m_marginLeftSpin = new QSpinBox(this); m_marginLeftSpin->setRange(5, 50);
    m_marginRightSpin = new QSpinBox(this); m_marginRightSpin->setRange(5, 50);

    m_headerTextEdit = new QLineEdit(this);
    m_subtitleEdit = new QLineEdit(this);
    m_artEdit = new QLineEdit(this);
    m_riggingEdit = new QLineEdit(this);
    m_footerTextEdit = new QLineEdit(this);

    QHBoxLayout* marginsBox = new QHBoxLayout();
    marginsBox->addWidget(new QLabel("Superior:")); marginsBox->addWidget(m_marginTopSpin);
    marginsBox->addWidget(new QLabel("Inferior:")); marginsBox->addWidget(m_marginBottomSpin);
    marginsBox->addWidget(new QLabel("Esq:")); marginsBox->addWidget(m_marginLeftSpin);
    marginsBox->addWidget(new QLabel("Dir:")); marginsBox->addWidget(m_marginRightSpin);

    marginLayout->addRow("Margens (mm):", marginsBox);
    marginLayout->addRow("Título (Topo):", m_headerTextEdit);
    marginLayout->addRow("Subtítulo:", m_subtitleEdit);
    marginLayout->addRow("ART Nº:", m_artEdit);
    marginLayout->addRow("RIGGING Nº:", m_riggingEdit);
    marginLayout->addRow("Texto Rodapé:", m_footerTextEdit);

    controlsLayout->addWidget(marginGroup);

    contentSplit->addLayout(controlsLayout, 1);

    // Right Column: Live Mini Preview
    QVBoxLayout* previewLayout = new QVBoxLayout();
    QLabel* prevTitle = new QLabel("👁️ Pré-visualização em Tempo Real", this);
    prevTitle->setStyleSheet("color: #FFFFFF; font-weight: bold; font-size: 13px;");

    m_previewViewer = new QTextBrowser(this);
    m_previewViewer->setStyleSheet("background-color: #FFFFFF; border-radius: 6px;");

    previewLayout->addWidget(prevTitle);
    previewLayout->addWidget(m_previewViewer, 1);

    contentSplit->addLayout(previewLayout, 1);

    mainLayout->addLayout(contentSplit, 1);

    // Connections
    connect(m_colorTitleBtn, &QPushButton::clicked, this, &PresetEditor::onPickColorTitle);
    connect(m_colorAccentBtn, &QPushButton::clicked, this, &PresetEditor::onPickColorAccent);
    connect(m_colorTableHeaderBtn, &QPushButton::clicked, this, &PresetEditor::onPickColorTableHeader);
    connect(m_colorBodyBtn, &QPushButton::clicked, this, &PresetEditor::onPickColorBody);

    connect(m_nameEdit, &QLineEdit::textChanged, this, &PresetEditor::updatePreview);
    connect(m_layoutCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PresetEditor::updatePreview);
    connect(m_fontCombo, &QComboBox::currentTextChanged, this, &PresetEditor::updatePreview);
    connect(m_fontSizeTitleSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &PresetEditor::updatePreview);
    connect(m_fontSizeSubtitleSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &PresetEditor::updatePreview);
    connect(m_fontSizeBodySpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &PresetEditor::updatePreview);
    connect(m_headerTextEdit, &QLineEdit::textChanged, this, &PresetEditor::updatePreview);
    connect(m_subtitleEdit, &QLineEdit::textChanged, this, &PresetEditor::updatePreview);
    connect(m_artEdit, &QLineEdit::textChanged, this, &PresetEditor::updatePreview);
    connect(m_riggingEdit, &QLineEdit::textChanged, this, &PresetEditor::updatePreview);
    connect(m_footerTextEdit, &QLineEdit::textChanged, this, &PresetEditor::updatePreview);

    connect(m_saveBtn, &QPushButton::clicked, this, &PresetEditor::onSaveClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &PresetEditor::backToGalleryRequested);
}

void PresetEditor::loadPreset(const FormatPreset& preset, bool isNew) {
    m_editingPreset = preset;
    m_isNewPreset = isNew;

    m_nameEdit->setText(m_isNewPreset ? QString("%1 (Cópia)").arg(preset.name()) : preset.name());
    int layoutIdx = m_layoutCombo->findData(static_cast<int>(preset.documentLayout()));
    if (layoutIdx != -1) m_layoutCombo->setCurrentIndex(layoutIdx);

    int fontIdx = m_fontCombo->findText(preset.fontFamily());
    if (fontIdx != -1) m_fontCombo->setCurrentIndex(fontIdx);

    m_fontSizeTitleSpin->setValue(preset.fontSizeTitle());
    m_fontSizeSubtitleSpin->setValue(preset.fontSizeSubtitle());
    m_fontSizeBodySpin->setValue(preset.fontSizeBody());

    m_colorTitle = preset.colorTitle();
    setupColorButton(m_colorTitleBtn, m_colorTitle);

    m_colorAccent = preset.colorAccent();
    setupColorButton(m_colorAccentBtn, m_colorAccent);

    m_colorTableHeader = preset.colorTableHeader();
    setupColorButton(m_colorTableHeaderBtn, m_colorTableHeader);

    m_colorBody = preset.colorBody();
    setupColorButton(m_colorBodyBtn, m_colorBody);

    m_marginTopSpin->setValue(preset.marginTop());
    m_marginBottomSpin->setValue(preset.marginBottom());
    m_marginLeftSpin->setValue(preset.marginLeft());
    m_marginRightSpin->setValue(preset.marginRight());

    m_headerTextEdit->setText(preset.headerText());
    m_subtitleEdit->setText(preset.subtitle());
    m_artEdit->setText(preset.artNumber());
    m_riggingEdit->setText(preset.riggingNumber());
    m_footerTextEdit->setText(preset.footerText());

    updatePreview();
}

void PresetEditor::setupColorButton(QPushButton* btn, const QColor& color) {
    btn->setText(color.name());
    btn->setStyleSheet(QString("background-color: %1; color: %2; font-weight: bold; border-radius: 4px; padding: 4px;")
                             .arg(color.name(), (color.lightness() > 128) ? "#000000" : "#FFFFFF"));
}

void PresetEditor::onPickColorTitle() {
    QColor c = QColorDialog::getColor(m_colorTitle, this, "Selecione a Cor do Título");
    if (c.isValid()) {
        m_colorTitle = c;
        setupColorButton(m_colorTitleBtn, m_colorTitle);
        updatePreview();
    }
}

void PresetEditor::onPickColorAccent() {
    QColor c = QColorDialog::getColor(m_colorAccent, this, "Selecione a Cor de Destaque");
    if (c.isValid()) {
        m_colorAccent = c;
        setupColorButton(m_colorAccentBtn, m_colorAccent);
        updatePreview();
    }
}

void PresetEditor::onPickColorTableHeader() {
    QColor c = QColorDialog::getColor(m_colorTableHeader, this, "Selecione a Cor de Tabela");
    if (c.isValid()) {
        m_colorTableHeader = c;
        setupColorButton(m_colorTableHeaderBtn, m_colorTableHeader);
        updatePreview();
    }
}

void PresetEditor::onPickColorBody() {
    QColor c = QColorDialog::getColor(m_colorBody, this, "Selecione a Cor do Texto");
    if (c.isValid()) {
        m_colorBody = c;
        setupColorButton(m_colorBodyBtn, m_colorBody);
        updatePreview();
    }
}

void PresetEditor::updatePreview() {
    FormatPreset temp = m_editingPreset;
    temp.setName(m_nameEdit->text());
    temp.setDocumentLayout(static_cast<FormatPreset::DocumentLayout>(m_layoutCombo->currentData().toInt()));
    temp.setFontFamily(m_fontCombo->currentText());
    temp.setFontSizeTitle(m_fontSizeTitleSpin->value());
    temp.setFontSizeSubtitle(m_fontSizeSubtitleSpin->value());
    temp.setFontSizeBody(m_fontSizeBodySpin->value());
    temp.setColorTitle(m_colorTitle);
    temp.setColorAccent(m_colorAccent);
    temp.setColorTableHeader(m_colorTableHeader);
    temp.setColorBody(m_colorBody);
    temp.setHeaderText(m_headerTextEdit->text());
    temp.setSubtitle(m_subtitleEdit->text());
    temp.setArtNumber(m_artEdit->text());
    temp.setRiggingNumber(m_riggingEdit->text());
    temp.setFooterText(m_footerTextEdit->text());

    // Generate dummy preview html
    RiggingCalculation sampleCalc(1);
    sampleCalc.setName("Cálculo de Exemplo (Carga Útil)");
    sampleCalc.setFormulaText("F_net = (M_cargo + M_rigging) * g");

    QVector<RiggingCalculation*> list = { &sampleCalc };

    DocumentGenerator gen;
    QString html = gen.generateHtml(list, temp, "Exemplo_Preview");
    m_previewViewer->setHtml(html);
}

void PresetEditor::onSaveClicked() {
    FormatPreset updated = m_editingPreset;
    if (m_isNewPreset) {
        updated.setId(QString("preset_%1").arg(QDateTime::currentMSecsSinceEpoch()));
        updated.setDefault(false);
    }
    updated.setName(m_nameEdit->text());
    updated.setDocumentLayout(static_cast<FormatPreset::DocumentLayout>(m_layoutCombo->currentData().toInt()));
    updated.setFontFamily(m_fontCombo->currentText());
    updated.setFontSizeTitle(m_fontSizeTitleSpin->value());
    updated.setFontSizeSubtitle(m_fontSizeSubtitleSpin->value());
    updated.setFontSizeBody(m_fontSizeBodySpin->value());
    updated.setColorTitle(m_colorTitle);
    updated.setColorAccent(m_colorAccent);
    updated.setColorTableHeader(m_colorTableHeader);
    updated.setColorBody(m_colorBody);
    updated.setMarginTop(m_marginTopSpin->value());
    updated.setMarginBottom(m_marginBottomSpin->value());
    updated.setMarginLeft(m_marginLeftSpin->value());
    updated.setMarginRight(m_marginRightSpin->value());
    updated.setHeaderText(m_headerTextEdit->text());
    updated.setSubtitle(m_subtitleEdit->text());
    updated.setArtNumber(m_artEdit->text());
    updated.setRiggingNumber(m_riggingEdit->text());
    updated.setFooterText(m_footerTextEdit->text());
    updated.touch();

    if (m_manager) {
        if (m_isNewPreset) {
            m_manager->addPreset(updated);
        } else {
            m_manager->updatePreset(updated);
        }
    }

    emit presetSaved(updated);
    emit backToGalleryRequested();
}
