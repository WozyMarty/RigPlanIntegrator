#include "documentpreview.h"
#include "widgets/riggingplandatadialog.h"
#include "widgets/craneselectiondialog.h"
#include "models/cranemanager.h"
#include "widgets/pageorderdialog.h"
#include "widgets/aivalidatordialog.h"
#include "utils/aivalidator.h"
#include <QHBoxLayout>
#include <QFrame>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>

DocumentPreview::DocumentPreview(QWidget* parent)
    : QWidget(parent)
    , m_engine(nullptr)
    , m_presetManager(nullptr)
    , m_headerEditFrame(nullptr)
{
    setupUi();
}

DocumentPreview::DocumentPreview(CalculationEngine* engine,
                                 PresetManager* presetManager,
                                 QWidget* parent)
    : QWidget(parent)
    , m_engine(nullptr)
    , m_presetManager(nullptr)
    , m_headerEditFrame(nullptr)
{
    setupUi();
    setEngineAndPresetManager(engine, presetManager);
}

void DocumentPreview::setEngineAndPresetManager(CalculationEngine* engine, PresetManager* presetManager) {
    m_engine = engine;
    m_presetManager = presetManager;

    if (m_presetManager) {
        connect(m_presetManager, &PresetManager::presetSelected, this, &DocumentPreview::onPresetSelected);
        connect(m_presetManager, &PresetManager::presetUpdated, this, [this](const FormatPreset&) { updatePreview(); });

        FormatPreset current = m_presetManager->currentPreset();

        // Initialize primary section
        if (m_sections.isEmpty()) {
            DocumentSection primarySec(QString(), DocumentSection::PresetStyle, current.name());
            primarySec.setPreset(current);
            m_sections.append(primarySec);
        } else if (m_sections[0].isPreset()) {
            m_sections[0].setPreset(current);
            m_sections[0].setTitle(current.name());
        }

        m_titleEdit->blockSignals(true);
        m_subtitleEdit->blockSignals(true);
        m_artEdit->blockSignals(true);
        m_riggingEdit->blockSignals(true);

        m_titleEdit->setText(current.headerText().trimmed().isEmpty() ? "Memorial" : current.headerText());
        m_subtitleEdit->setText(current.subtitle());
        m_artEdit->setText(current.artNumber().trimmed().isEmpty() ? "1720264496217" : current.artNumber());
        m_riggingEdit->setText(current.riggingNumber().trimmed().isEmpty() ? "2607036" : current.riggingNumber());

        m_titleEdit->blockSignals(false);
        m_subtitleEdit->blockSignals(false);
        m_artEdit->blockSignals(false);
        m_riggingEdit->blockSignals(false);
    }

    if (m_engine) {
        connect(m_engine, &CalculationEngine::calculationUpdated, this, [this](int, double) { updatePreview(); });
        connect(m_engine, &CalculationEngine::allCalculationsReady, this, &DocumentPreview::updatePreview);
        connect(m_engine, &CalculationEngine::activeCalculationsChanged, this, &DocumentPreview::updatePreview);
    }

    updateAttachmentButtonStates();
    updatePreview();
}

void DocumentPreview::setupUi() {
    setStyleSheet(
        "QWidget#previewMain {"
        "   background-color: #12161F;"
        "}"
        "QFrame#toolbarFrame {"
        "   background-color: #1A1F2C;"
        "   border-bottom: 1px solid #2E384D;"
        "   border-radius: 0px;"
        "}"
        "QLabel#presetBadge {"
        "   background-color: #1565C0;"
        "   color: #FFFFFF;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "   border-radius: 4px;"
        "   padding: 3px 8px;"
        "}"
        "QPushButton#changeBtn {"
        "   background-color: rgba(255, 255, 255, 0.08);"
        "   color: #29B6F6;"
        "   border: 1px solid rgba(41, 182, 246, 0.4);"
        "   border-radius: 4px;"
        "   padding: 4px 10px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QPushButton#changeBtn:hover {"
        "   background-color: rgba(41, 182, 246, 0.2);"
        "}"
        "QPushButton#addStyleBtn {"
        "   background-color: #1F6FEB;"
        "   color: #FFFFFF;"
        "   border: 1px solid #388BFD;"
        "   border-radius: 4px;"
        "   padding: 4px 10px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QPushButton#addStyleBtn:hover {"
        "   background-color: #388BFD;"
        "}"
        "QPushButton#attachBtn {"
        "   background-color: #21262D;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 4px;"
        "   padding: 4px 8px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QPushButton#attachBtn:hover {"
        "   background-color: #30363D;"
        "   color: #FFFFFF;"
        "}"
        "QPushButton#deleteMiniBtn {"
        "   background-color: #DA3633;"
        "   color: #FFFFFF;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 4px 6px;"
        "   font-weight: bold;"
        "   font-size: 10px;"
        "}"
        "QPushButton#deleteMiniBtn:hover {"
        "   background-color: #F85149;"
        "}"
        "QPushButton#editPlanBtn {"
        "   background-color: #E65100;"
        "   color: #FFFFFF;"
        "   border: 1px solid #FF9800;"
        "   border-radius: 4px;"
        "   padding: 4px 10px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QPushButton#editPlanBtn:hover {"
        "   background-color: #F57C00;"
        "}"
        "QPushButton#selectCraneBtn {"
        "   background-color: #0D47A1;"
        "   color: #FFFFFF;"
        "   border: 1px solid #1E88E5;"
        "   border-radius: 4px;"
        "   padding: 4px 10px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QPushButton#selectCraneBtn:hover {"
        "   background-color: #1565C0;"
        "}"
        "QPushButton#orderBtn {"
        "   background-color: #6E40C9;"
        "   color: #FFFFFF;"
        "   border: 1px solid #8957E5;"
        "   border-radius: 4px;"
        "   padding: 4px 10px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QPushButton#orderBtn:hover {"
        "   background-color: #8957E5;"
        "}"
        "QPushButton#aiBtn {"
        "   background-color: #238636;"
        "   color: #FFFFFF;"
        "   border: 1px solid #2EA043;"
        "   border-radius: 4px;"
        "   padding: 4px 10px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QPushButton#aiBtn:hover {"
        "   background-color: #2EA043;"
        "}"
        "QPushButton#zoomBtn {"
        "   background-color: #21262D;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 4px;"
        "   padding: 2px 8px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#zoomBtn:hover {"
        "   background-color: #30363D;"
        "}"
        "QTextBrowser#docViewer {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #B0BEC5;"
        "   border-radius: 4px;"
        "}"
    );

    setObjectName("previewMain");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- Toolbar ---
    QFrame* toolbarFrame = new QFrame(this);
    toolbarFrame->setObjectName("toolbarFrame");

    QHBoxLayout* toolbarLayout = new QHBoxLayout(toolbarFrame);
    toolbarLayout->setContentsMargins(12, 6, 12, 6);
    toolbarLayout->setSpacing(8);

    QLabel* iconLabel = new QLabel("👁️ Pré-Visualização:", toolbarFrame);
    iconLabel->setStyleSheet("color: #FFFFFF; font-weight: bold; font-size: 12px;");

    m_presetBadge = new QLabel("Estilo: Padrão", toolbarFrame);
    m_presetBadge->setObjectName("presetBadge");

    m_changePresetBtn = new QPushButton("⚙️ Alterar Estilo", toolbarFrame);
    m_changePresetBtn->setObjectName("changeBtn");
    m_changePresetBtn->setToolTip("Alterar o estilo de formatação da página principal");

    m_addStyleBtn = new QPushButton("➕ Incluir Estilo", toolbarFrame);
    m_addStyleBtn->setObjectName("addStyleBtn");
    m_addStyleBtn->setToolTip("Incluir outra página/estilo contínuo similar ao Plano de Rigging");

    m_editPlanBtn = new QPushButton("✏️ Editar Plano", toolbarFrame);
    m_editPlanBtn->setObjectName("editPlanBtn");
    m_editPlanBtn->setToolTip("Editar os parâmetros e dados estruturados da carga e do guindaste");

    m_selectCraneBtn = new QPushButton("🏗️ Selecionar Guindaste", toolbarFrame);
    m_selectCraneBtn->setObjectName("selectCraneBtn");
    m_selectCraneBtn->setToolTip("Selecionar guindaste cadastrado, criar novo ou configurar parâmetros operacionais");

    // CREA buttons
    m_importCreaBtn = new QPushButton("📷 CREA", toolbarFrame);
    m_importCreaBtn->setObjectName("attachBtn");
    m_importCreaBtn->setToolTip("Importar imagem da ART / CREA diretamente na visualização contínua");

    m_deleteCreaBtn = new QPushButton("✖", toolbarFrame);
    m_deleteCreaBtn->setObjectName("deleteMiniBtn");
    m_deleteCreaBtn->setToolTip("Excluir imagem do CREA");
    m_deleteCreaBtn->setVisible(false);

    // SOLIDWORKS buttons
    m_importSwBtn = new QPushButton("📐 SOLIDWORKS", toolbarFrame);
    m_importSwBtn->setObjectName("attachBtn");
    m_importSwBtn->setToolTip("Importar desenho 3D / CAD do SOLIDWORKS diretamente na visualização contínua");

    m_deleteSwBtn = new QPushButton("✖", toolbarFrame);
    m_deleteSwBtn->setObjectName("deleteMiniBtn");
    m_deleteSwBtn->setToolTip("Excluir imagem do SOLIDWORKS");
    m_deleteSwBtn->setVisible(false);

    // Page Order button
    m_orderBtn = new QPushButton("📑 Ordem", toolbarFrame);
    m_orderBtn->setObjectName("orderBtn");
    m_orderBtn->setToolTip("Definir a sequência e ordem das páginas na visualização e exportação");

    // Verifier button
    m_aiStatusBtn = new QPushButton("🔍 Verificador: 100%", toolbarFrame);
    m_aiStatusBtn->setObjectName("aiBtn");
    m_aiStatusBtn->setToolTip("Conferência de regras e integridade entre fórmulas e edições do Plano de Rigging (Verificador)");

    toolbarLayout->addWidget(iconLabel);
    toolbarLayout->addWidget(m_presetBadge);
    toolbarLayout->addWidget(m_changePresetBtn);
    toolbarLayout->addWidget(m_addStyleBtn);
    toolbarLayout->addWidget(m_editPlanBtn);
    toolbarLayout->addWidget(m_selectCraneBtn);

    toolbarLayout->addWidget(m_importCreaBtn);
    toolbarLayout->addWidget(m_deleteCreaBtn);
    toolbarLayout->addWidget(m_importSwBtn);
    toolbarLayout->addWidget(m_deleteSwBtn);

    toolbarLayout->addWidget(m_orderBtn);
    toolbarLayout->addWidget(m_aiStatusBtn);

    toolbarLayout->addStretch(1);

    // Zoom Controls
    m_zoomOutBtn = new QPushButton("−", toolbarFrame);
    m_zoomOutBtn->setObjectName("zoomBtn");

    m_zoomLabel = new QLabel("100%", toolbarFrame);
    m_zoomLabel->setStyleSheet("color: #C9D1D9; font-weight: bold; font-size: 11px;");

    m_zoomInBtn = new QPushButton("+", toolbarFrame);
    m_zoomInBtn->setObjectName("zoomBtn");

    toolbarLayout->addWidget(m_zoomOutBtn);
    toolbarLayout->addWidget(m_zoomLabel);
    toolbarLayout->addWidget(m_zoomInBtn);

    mainLayout->addWidget(toolbarFrame);

    // --- Header Information Edit Bar ---
    m_headerEditFrame = new QFrame(this);
    m_headerEditFrame->setObjectName("headerEditFrame");
    m_headerEditFrame->setStyleSheet(
        "QFrame#headerEditFrame {"
        "   background-color: #161D2B;"
        "   border-bottom: 1px solid #2B384E;"
        "   padding: 4px 12px;"
        "}"
        "QLabel {"
        "   color: #90A4AE;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "}"
        "QLineEdit {"
        "   background-color: #0E131E;"
        "   color: #FFFFFF;"
        "   border: 1px solid #37474F;"
        "   border-radius: 4px;"
        "   padding: 3px 8px;"
        "   font-size: 12px;"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid #29B6F6;"
        "   background-color: #141B2B;"
        "}"
    );

    QHBoxLayout* headerEditLayout = new QHBoxLayout(m_headerEditFrame);
    headerEditLayout->setContentsMargins(12, 4, 12, 4);
    headerEditLayout->setSpacing(8);

    QLabel* titleIcon = new QLabel("📝 Cabeçalho:", m_headerEditFrame);
    titleIcon->setStyleSheet("color: #FFFFFF; font-weight: bold; font-size: 11px;");

    QLabel* titleLbl = new QLabel("Título:", m_headerEditFrame);
    m_titleEdit = new QLineEdit(m_headerEditFrame);
    m_titleEdit->setPlaceholderText("Memorial");
    m_titleEdit->setMinimumWidth(110);

    QLabel* subLbl = new QLabel("Subtítulo:", m_headerEditFrame);
    m_subtitleEdit = new QLineEdit(m_headerEditFrame);
    m_subtitleEdit->setPlaceholderText("Içamento Chiller");
    m_subtitleEdit->setMinimumWidth(130);

    QLabel* artLbl = new QLabel("ART Nº:", m_headerEditFrame);
    m_artEdit = new QLineEdit(m_headerEditFrame);
    m_artEdit->setPlaceholderText("1720264496217");
    m_artEdit->setFixedWidth(120);

    QLabel* rigLbl = new QLabel("RIGGING Nº:", m_headerEditFrame);
    m_riggingEdit = new QLineEdit(m_headerEditFrame);
    m_riggingEdit->setPlaceholderText("2607036");
    m_riggingEdit->setFixedWidth(90);

    headerEditLayout->addWidget(titleIcon);
    headerEditLayout->addWidget(titleLbl);
    headerEditLayout->addWidget(m_titleEdit);
    headerEditLayout->addWidget(subLbl);
    headerEditLayout->addWidget(m_subtitleEdit);
    headerEditLayout->addWidget(artLbl);
    headerEditLayout->addWidget(m_artEdit);
    headerEditLayout->addWidget(rigLbl);
    headerEditLayout->addWidget(m_riggingEdit);
    headerEditLayout->addStretch(1);

    mainLayout->addWidget(m_headerEditFrame);

    // --- Document Viewing Container (Paper effect) ---
    QWidget* paperContainer = new QWidget(this);
    paperContainer->setObjectName("paperContainer");
    paperContainer->setStyleSheet("QWidget#paperContainer { background-color: #0E131F; }");

    QVBoxLayout* paperLayout = new QVBoxLayout(paperContainer);
    paperLayout->setContentsMargins(24, 16, 24, 16);

    m_textBrowser = new QTextBrowser(paperContainer);
    m_textBrowser->setObjectName("docViewer");
    m_textBrowser->setStyleSheet(
        "QTextBrowser#docViewer {"
        "   background-color: #FFFFFF;"
        "   color: #1F2328;"
        "   border: 1px solid #B0BEC5;"
        "   border-radius: 4px;"
        "   padding: 16px;"
        "}"
    );
    m_textBrowser->setOpenExternalLinks(false);

    paperLayout->addWidget(m_textBrowser);
    mainLayout->addWidget(paperContainer, 1);

    // Connections
    connect(m_changePresetBtn, &QPushButton::clicked, this, &DocumentPreview::openPresetGalleryRequested);
    connect(m_addStyleBtn, &QPushButton::clicked, this, &DocumentPreview::onAddStyleRequested);
    connect(m_editPlanBtn, &QPushButton::clicked, this, [this]() {
        if (!m_presetManager) return;
        FormatPreset preset = m_presetManager->currentPreset();
        RiggingPlanDataDialog dlg(preset.planData(), this);
        if (dlg.exec() == QDialog::Accepted) {
            preset.setPlanData(dlg.planData());
            m_presetManager->updatePreset(preset);

            // Also update any matching section
            for (auto& sec : m_sections) {
                if (sec.isPreset() && sec.preset().isPlanoRiggingLayout()) {
                    sec.setPreset(preset);
                }
            }

            updatePreview();
        }
    });

    connect(m_selectCraneBtn, &QPushButton::clicked, this, [this]() {
        if (!m_presetManager) return;
        FormatPreset preset = m_presetManager->currentPreset();
        RiggingPlanData plan = preset.planData();
        CraneSelectionDialog dlg(plan, this);
        if (dlg.exec() == QDialog::Accepted) {
            CraneProfile crane = dlg.selectedCrane();
            plan.guindasteModelo = crane.modelo;
            plan.guindasteCaminhao = crane.caminhao;
            plan.guindastePlacas = crane.placas;
            plan.guindasteChassi = crane.chassi;
            plan.guindasteAno = crane.ano;
            plan.capNominal = crane.capNominal;
            plan.compLanca = crane.compLanca;
            plan.raioOperacao = crane.raioOperacao;
            plan.capCargaNoRaio = crane.capCargaNoRaio;
            plan.quadranteInicial = crane.quadranteInicial;
            plan.quadranteFinal = crane.quadranteFinal;

            preset.setPlanData(plan);
            m_presetManager->updatePreset(preset);

            // Also update any matching section
            for (auto& sec : m_sections) {
                if (sec.isPreset() && sec.preset().isPlanoRiggingLayout()) {
                    sec.setPreset(preset);
                }
            }

            updatePreview();
        }
    });

    connect(m_importCreaBtn, &QPushButton::clicked, this, &DocumentPreview::onImportCrea);
    connect(m_deleteCreaBtn, &QPushButton::clicked, this, &DocumentPreview::onDeleteCrea);
    connect(m_importSwBtn, &QPushButton::clicked, this, &DocumentPreview::onImportSolidworks);
    connect(m_deleteSwBtn, &QPushButton::clicked, this, &DocumentPreview::onDeleteSolidworks);

    connect(m_orderBtn, &QPushButton::clicked, this, &DocumentPreview::onOpenPageOrder);
    connect(m_aiStatusBtn, &QPushButton::clicked, this, &DocumentPreview::onOpenAiValidator);

    connect(m_zoomInBtn, &QPushButton::clicked, this, &DocumentPreview::zoomIn);
    connect(m_zoomOutBtn, &QPushButton::clicked, this, &DocumentPreview::zoomOut);

    auto updatePresetHeader = [this]() {
        if (!m_presetManager) return;
        FormatPreset p = m_presetManager->currentPreset();
        p.setHeaderText(m_titleEdit->text());
        p.setSubtitle(m_subtitleEdit->text());
        p.setArtNumber(m_artEdit->text());
        p.setRiggingNumber(m_riggingEdit->text());
        m_presetManager->updatePreset(p);

        if (!m_sections.isEmpty() && m_sections[0].isPreset()) {
            m_sections[0].setPreset(p);
        }
    };

    connect(m_titleEdit, &QLineEdit::textChanged, this, updatePresetHeader);
    connect(m_subtitleEdit, &QLineEdit::textChanged, this, updatePresetHeader);
    connect(m_artEdit, &QLineEdit::textChanged, this, updatePresetHeader);
    connect(m_riggingEdit, &QLineEdit::textChanged, this, updatePresetHeader);
}

void DocumentPreview::setDocumentName(const QString& name) {
    m_documentName = name;
    updatePreview();
}

void DocumentPreview::setSections(const QVector<DocumentSection>& sections) {
    m_sections = sections;
    updateAttachmentButtonStates();
    updatePreview();
}

QTextDocument* DocumentPreview::documentForExport() const {
    if (!m_engine) return m_textBrowser ? m_textBrowser->document()->clone() : nullptr;
    DocumentGenerator gen;
    return gen.generateContinuous(m_sections, m_engine->calculations(), m_documentName);
}

void DocumentPreview::updateAttachmentButtonStates() {
    bool hasCrea = false;
    bool hasSw = false;

    for (const auto& sec : m_sections) {
        if (sec.type() == DocumentSection::CreaImage && sec.isEnabled()) {
            hasCrea = true;
        } else if (sec.type() == DocumentSection::SolidworksImage && sec.isEnabled()) {
            hasSw = true;
        }
    }

    if (hasCrea) {
        m_importCreaBtn->setText("📷 CREA ✓");
        m_importCreaBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: bold; border-radius: 4px; padding: 4px 8px;");
        m_deleteCreaBtn->setVisible(true);
    } else {
        m_importCreaBtn->setText("📷 Importar CREA");
        m_importCreaBtn->setStyleSheet("background-color: #21262D; color: #C9D1D9; border: 1px solid #30363D; border-radius: 4px; padding: 4px 8px; font-weight: bold;");
        m_deleteCreaBtn->setVisible(false);
    }

    if (hasSw) {
        m_importSwBtn->setText("📐 SOLIDWORKS ✓");
        m_importSwBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: bold; border-radius: 4px; padding: 4px 8px;");
        m_deleteSwBtn->setVisible(true);
    } else {
        m_importSwBtn->setText("📐 Importar SOLIDWORKS");
        m_importSwBtn->setStyleSheet("background-color: #21262D; color: #C9D1D9; border: 1px solid #30363D; border-radius: 4px; padding: 4px 8px; font-weight: bold;");
        m_deleteSwBtn->setVisible(false);
    }

    int activeCount = 0;
    for (const auto& s : m_sections) if (s.isEnabled()) activeCount++;
    m_orderBtn->setText(QString("📑 Ordem (%1)").arg(activeCount));
}

void DocumentPreview::updateAiBadge() {
    if (!m_engine || !m_presetManager) return;

    FormatPreset current = m_presetManager->currentPreset();
    AiValidationReport report = AiValidator::validate(current.planData(), m_engine->calculations(), current);

    if (report.criticalCount > 0) {
        m_aiStatusBtn->setText(QString("🔍 Verificador: %1 Crítico").arg(report.criticalCount));
        m_aiStatusBtn->setStyleSheet("background-color: #DA3633; color: #FFFFFF; border: 1px solid #F85149; border-radius: 4px; padding: 4px 10px; font-weight: bold; font-size: 11px;");
    } else if (report.warningCount > 0) {
        m_aiStatusBtn->setText(QString("🔍 Verificador: %1 Alerta").arg(report.warningCount));
        m_aiStatusBtn->setStyleSheet("background-color: #D29922; color: #FFFFFF; border: 1px solid #E3B341; border-radius: 4px; padding: 4px 10px; font-weight: bold; font-size: 11px;");
    } else {
        m_aiStatusBtn->setText("🔍 Verificador: Conforme");
        m_aiStatusBtn->setStyleSheet("background-color: #238636; color: #FFFFFF; border: 1px solid #2EA043; border-radius: 4px; padding: 4px 10px; font-weight: bold; font-size: 11px;");
    }
}

void DocumentPreview::updatePreview() {
    if (!m_engine || !m_presetManager) return;

    FormatPreset preset = m_presetManager->currentPreset();
    m_presetBadge->setText(QString("Estilo: %1").arg(preset.name()));

    bool hasPlanSection = false;
    for (const auto& s : m_sections) {
        if (s.isEnabled() && s.isPreset() && s.preset().isPlanoRiggingLayout()) {
            hasPlanSection = true;
            break;
        }
    }
    m_editPlanBtn->setVisible(hasPlanSection || preset.isPlanoRiggingLayout());
    m_selectCraneBtn->setVisible(hasPlanSection || preset.isPlanoRiggingLayout());

    // Update section 0 if preset changed
    if (!m_sections.isEmpty() && m_sections[0].isPreset()) {
        m_sections[0].setPreset(preset);
        m_sections[0].setTitle(preset.name());
    }

    // Generate unified continuous HTML
    QString html = m_generator.generateContinuousHtml(m_sections, m_engine->calculations(), m_documentName);
    m_textBrowser->setHtml(html);

    updateAttachmentButtonStates();
    updateAiBadge();
}

void DocumentPreview::onPresetSelected(const FormatPreset& preset) {
    m_presetBadge->setText(QString("Estilo: %1").arg(preset.name()));

    if (!m_sections.isEmpty() && m_sections[0].isPreset()) {
        m_sections[0].setPreset(preset);
        m_sections[0].setTitle(preset.name());
    }

    m_titleEdit->blockSignals(true);
    m_subtitleEdit->blockSignals(true);
    m_artEdit->blockSignals(true);
    m_riggingEdit->blockSignals(true);

    m_titleEdit->setText(preset.headerText().trimmed().isEmpty() ? "Memorial" : preset.headerText());
    m_subtitleEdit->setText(preset.subtitle());
    m_artEdit->setText(preset.artNumber().trimmed().isEmpty() ? "1720264496217" : preset.artNumber());
    m_riggingEdit->setText(preset.riggingNumber().trimmed().isEmpty() ? "2607036" : preset.riggingNumber());

    m_titleEdit->blockSignals(false);
    m_subtitleEdit->blockSignals(false);
    m_artEdit->blockSignals(false);
    m_riggingEdit->blockSignals(false);

    updatePreview();
}

void DocumentPreview::onImportCrea() {
    QString filePath = QFileDialog::getOpenFileName(
        this, "Importar Imagem CREA / ART para Pré-Visualização", "",
        "Imagens (*.png *.jpg *.jpeg *.bmp *.webp);;Todos os Arquivos (*.*)"
    );
    if (filePath.isEmpty()) return;

    int creaIdx = -1;
    for (int i = 0; i < m_sections.size(); ++i) {
        if (m_sections[i].type() == DocumentSection::CreaImage) {
            creaIdx = i;
            break;
        }
    }

    if (creaIdx != -1) {
        m_sections[creaIdx].setImagePath(filePath);
        m_sections[creaIdx].setEnabled(true);
    } else {
        DocumentSection sec(QString(), DocumentSection::CreaImage, "Anexo CREA (ART)");
        sec.setImagePath(filePath);
        sec.setEnabled(true);
        m_sections.append(sec);
    }

    updateAttachmentButtonStates();
    updatePreview();
}

void DocumentPreview::onDeleteCrea() {
    for (int i = m_sections.size() - 1; i >= 0; --i) {
        if (m_sections[i].type() == DocumentSection::CreaImage) {
            m_sections.removeAt(i);
        }
    }
    updateAttachmentButtonStates();
    updatePreview();
}

void DocumentPreview::onImportSolidworks() {
    QString filePath = QFileDialog::getOpenFileName(
        this, "Importar Desenho Técnico SOLIDWORKS para Pré-Visualização", "",
        "Imagens (*.png *.jpg *.jpeg *.bmp *.webp);;Todos os Arquivos (*.*)"
    );
    if (filePath.isEmpty()) return;

    int swIdx = -1;
    for (int i = 0; i < m_sections.size(); ++i) {
        if (m_sections[i].type() == DocumentSection::SolidworksImage) {
            swIdx = i;
            break;
        }
    }

    if (swIdx != -1) {
        m_sections[swIdx].setImagePath(filePath);
        m_sections[swIdx].setEnabled(true);
    } else {
        DocumentSection sec(QString(), DocumentSection::SolidworksImage, "Desenho Técnico SOLIDWORKS");
        sec.setImagePath(filePath);
        sec.setEnabled(true);
        m_sections.append(sec);
    }

    updateAttachmentButtonStates();
    updatePreview();
}

void DocumentPreview::onDeleteSolidworks() {
    for (int i = m_sections.size() - 1; i >= 0; --i) {
        if (m_sections[i].type() == DocumentSection::SolidworksImage) {
            m_sections.removeAt(i);
        }
    }
    updateAttachmentButtonStates();
    updatePreview();
}

void DocumentPreview::onAddStyleRequested() {
    if (!m_presetManager) return;

    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background-color: #1F2430; color: #FFFFFF; border: 1px solid #30363D; padding: 4px; }"
        "QMenu::item:selected { background-color: #1F6FEB; }"
    );

    for (const auto& p : m_presetManager->allPresets()) {
        QAction* act = menu.addAction(QString("➕ Incluir %1").arg(p.name()));
        connect(act, &QAction::triggered, this, [this, p]() {
            DocumentSection sec(QString(), DocumentSection::PresetStyle, p.name());
            sec.setPreset(p);
            sec.setEnabled(true);
            m_sections.append(sec);
            updateAttachmentButtonStates();
            updatePreview();
        });
    }

    menu.exec(m_addStyleBtn->mapToGlobal(QPoint(0, m_addStyleBtn->height())));
}

void DocumentPreview::onOpenPageOrder() {
    PageOrderDialog dlg(m_sections, m_presetManager, this);
    connect(&dlg, &PageOrderDialog::sectionsChanged, this, [this](const QVector<DocumentSection>& updated) {
        m_sections = updated;
        updateAttachmentButtonStates();
        updatePreview();
    });
    dlg.exec();
}

void DocumentPreview::onOpenAiValidator() {
    AiValidatorDialog dlg(m_engine, m_presetManager, this);
    connect(&dlg, &AiValidatorDialog::dataSynced, this, [this]() {
        if (!m_presetManager) return;
        FormatPreset current = m_presetManager->currentPreset();
        for (auto& s : m_sections) {
            if (s.isPreset() && s.presetId() == current.id()) {
                s.setPreset(current);
            }
        }
        updatePreview();
    });
    dlg.exec();
}

void DocumentPreview::zoomIn() {
    if (m_zoomPercent < 200) {
        m_zoomPercent += 10;
        m_zoomLabel->setText(QString("%1%").arg(m_zoomPercent));
        m_textBrowser->zoomIn(1);
    }
}

void DocumentPreview::zoomOut() {
    if (m_zoomPercent > 50) {
        m_zoomPercent -= 10;
        m_zoomLabel->setText(QString("%1%").arg(m_zoomPercent));
        m_textBrowser->zoomOut(1);
    }
}

void DocumentPreview::zoomReset() {
    m_zoomPercent = 100;
    m_zoomLabel->setText("100%");
}
