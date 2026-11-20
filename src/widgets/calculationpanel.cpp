#include "calculationpanel.h"
#include "calculationpresetdialog.h"
#include "customcalcdialog.h"
#include "utils/constants.h"
#include <QHBoxLayout>
#include <QMessageBox>

CalculationPanel::CalculationPanel(QWidget* parent)
    : QWidget(parent)
    , m_engine(nullptr)
{
    setMinimumWidth(Constants::CALC_PANEL_MIN_WIDTH);
    setMaximumWidth(Constants::CALC_PANEL_MAX_WIDTH);
    setupUi();
}

CalculationPanel::CalculationPanel(CalculationEngine* engine, QWidget* parent)
    : QWidget(parent)
    , m_engine(nullptr)
{
    setMinimumWidth(Constants::CALC_PANEL_MIN_WIDTH);
    setMaximumWidth(Constants::CALC_PANEL_MAX_WIDTH);
    setupUi();
    setEngine(engine);
}

void CalculationPanel::setEngine(CalculationEngine* engine)
{
    if (m_engine == engine && m_engine != nullptr) return;
    m_engine = engine;

    if (m_engine) {
        connect(m_engine, &CalculationEngine::activeCalculationsChanged,
                this, &CalculationPanel::rebuildBlocks);
        connect(m_engine, &CalculationEngine::calculationPresetsChanged,
                this, &CalculationPanel::updatePresetCombo);
        connect(m_engine, &CalculationEngine::progressChanged,
                this, [this](int comp, int tot) {
            m_progressLabel->setText(QString("%1/%2 Concluídos").arg(comp).arg(tot));
        });

        updatePresetCombo();
        rebuildBlocks();
    }
}

void CalculationPanel::rebuildBlocks()
{
    qDeleteAll(m_blocks);
    m_blocks.clear();

    if (!m_engine) return;

    const auto& calcs = m_engine->activeCalculations();
    for (RiggingCalculation* calc : calcs) {
        CalculationBlock* block = new CalculationBlock(calc, m_scrollContainer);
        m_blocksLayout->insertWidget(m_blocksLayout->count() - 1, block);
        m_blocks.append(block);

        connect(block, &CalculationBlock::valueChanged, this, &CalculationPanel::parameterValueChanged);
    }

    m_titleLabel->setText(QString("⚡ Painel de Cálculos (%1)").arg(calcs.size()));
    updateProgressCounter();
    filterBlocks(m_searchEdit->text());
}

void CalculationPanel::updatePresetCombo()
{
    if (!m_engine) return;

    m_presetCombo->blockSignals(true);
    m_presetCombo->clear();

    const auto& presets = m_engine->calculationPresets();
    for (const auto& p : presets) {
        QString label = p.name();
        m_presetCombo->addItem(label, p.id());
    }

    int idx = m_presetCombo->findData(m_engine->currentPresetId());
    if (idx >= 0) {
        m_presetCombo->setCurrentIndex(idx);
    }

    m_presetCombo->blockSignals(false);
}

void CalculationPanel::onPresetComboActivated(int index)
{
    if (!m_engine) return;
    QString presetId = m_presetCombo->itemData(index).toString();
    if (!presetId.isEmpty()) {
        m_engine->applyPreset(presetId);
    }
}

void CalculationPanel::onSelectCalculationsClicked()
{
    if (!m_engine) return;
    CalculationPresetDialog dialog(m_engine, this);
    if (dialog.exec() == QDialog::Accepted) {
        updatePresetCombo();
        rebuildBlocks();
    }
}

void CalculationPanel::onNewCalculationClicked()
{
    if (!m_engine) return;
    CustomCalcDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        RiggingCalculation* calc = m_engine->addCustomCalculation(
            dialog.title(),
            dialog.formula(),
            dialog.parameters(),
            dialog.resultUnit(),
            dialog.description()
        );

        if (calc) {
            updatePresetCombo();
            rebuildBlocks();
            QMessageBox::information(this, "Cálculo Adicionado",
                                     QString("O cálculo \"%1\" foi criado e adicionado com sucesso!\n"
                                             "Você já pode preencher os parâmetros no painel.")
                                         .arg(calc->name()));
        }
    }
}

void CalculationPanel::setupUi()
{
    setStyleSheet(
        "QWidget#calcPanelMain {"
        "   background-color: #0D1117;"
        "   border-left: 1px solid #21262D;"
        "}"
        "QLabel#panelTitle {"
        "   color: #FFFFFF;"
        "   font-weight: 800;"
        "   font-size: 14px;"
        "}"
        "QLabel#progressText {"
        "   color: #3FB950;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "}"
        "QLabel#presetLabel {"
        "   color: #8B949E;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "}"
        "QComboBox {"
        "   background-color: #161B22;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 5px 8px;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "}"
        "QComboBox:focus {"
        "   border: 1px solid #58A6FF;"
        "}"
        "QLineEdit#searchBox {"
        "   background-color: #161B22;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 6px 12px;"
        "   font-size: 12px;"
        "}"
        "QLineEdit#searchBox:focus {"
        "   border: 1px solid #58A6FF;"
        "}"
        "QPushButton#actionBtn {"
        "   background-color: #21262D;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 4px;"
        "   padding: 3px 8px;"
        "   font-size: 11px;"
        "}"
        "QPushButton#actionBtn:hover {"
        "   background-color: #30363D;"
        "   color: #FFFFFF;"
        "}"
        "QPushButton#newCalcBtn {"
        "   background-color: #238636;"
        "   color: #FFFFFF;"
        "   border: 1px solid rgba(240, 246, 252, 0.1);"
        "   border-radius: 4px;"
        "   padding: 4px 10px;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#newCalcBtn:hover {"
        "   background-color: #2EA043;"
        "}"
        "QPushButton#selectCalcsBtn {"
        "   background-color: #1F6FEB;"
        "   color: #FFFFFF;"
        "   border: 1px solid rgba(240, 246, 252, 0.1);"
        "   border-radius: 4px;"
        "   padding: 4px 10px;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#selectCalcsBtn:hover {"
        "   background-color: #388BFD;"
        "}"
        "QScrollArea {"
        "   border: none;"
        "   background-color: transparent;"
        "}"
    );

    setObjectName("calcPanelMain");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    // --- Header Title & Progress ---
    QHBoxLayout* headerLayout = new QHBoxLayout();
    m_titleLabel = new QLabel("⚡ Painel de Cálculos", this);
    m_titleLabel->setObjectName("panelTitle");

    m_progressLabel = new QLabel("0 Concluídos", this);
    m_progressLabel->setObjectName("progressText");

    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch(1);
    headerLayout->addWidget(m_progressLabel);

    mainLayout->addLayout(headerLayout);

    // --- Preset Selector Row ---
    QHBoxLayout* presetRow = new QHBoxLayout();
    QLabel* pLbl = new QLabel("Preset:", this);
    pLbl->setObjectName("presetLabel");
    m_presetCombo = new QComboBox(this);

    m_selectCalcsBtn = new QPushButton("⚙️ Selecionar", this);
    m_selectCalcsBtn->setObjectName("selectCalcsBtn");
    m_selectCalcsBtn->setToolTip("Selecionar quais cálculos estão ativos e gerenciar presets");

    presetRow->addWidget(pLbl);
    presetRow->addWidget(m_presetCombo, 1);
    presetRow->addWidget(m_selectCalcsBtn);

    mainLayout->addLayout(presetRow);

    // --- Custom Calc Button Row ---
    QHBoxLayout* actionTopRow = new QHBoxLayout();
    m_newCalcBtn = new QPushButton("➕ Novo Cálculo", this);
    m_newCalcBtn->setObjectName("newCalcBtn");
    m_newCalcBtn->setToolTip("Criar um novo cálculo personalizado com fórmula e variáveis livres");

    actionTopRow->addWidget(m_newCalcBtn);
    actionTopRow->addStretch(1);

    m_expandAllBtn = new QPushButton("Expandir", this);
    m_expandAllBtn->setObjectName("actionBtn");

    m_collapseAllBtn = new QPushButton("Recolher", this);
    m_collapseAllBtn->setObjectName("actionBtn");

    m_clearAllBtn = new QPushButton("Limpar", this);
    m_clearAllBtn->setObjectName("actionBtn");

    actionTopRow->addWidget(m_expandAllBtn);
    actionTopRow->addWidget(m_collapseAllBtn);
    actionTopRow->addWidget(m_clearAllBtn);

    mainLayout->addLayout(actionTopRow);

    // --- Search Filter Bar ---
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setObjectName("searchBox");
    m_searchEdit->setPlaceholderText("🔍 Buscar cálculo ou fórmula...");
    m_searchEdit->setClearButtonEnabled(true);

    mainLayout->addWidget(m_searchEdit);

    // --- Scroll Area for Calculation Blocks ---
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_scrollContainer = new QWidget(m_scrollArea);
    m_scrollContainer->setStyleSheet("background: transparent;");

    m_blocksLayout = new QVBoxLayout(m_scrollContainer);
    m_blocksLayout->setContentsMargins(0, 0, 4, 0);
    m_blocksLayout->setSpacing(10);
    m_blocksLayout->addStretch(1);

    m_scrollArea->setWidget(m_scrollContainer);
    mainLayout->addWidget(m_scrollArea, 1);

    // Connections
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::activated),
            this, &CalculationPanel::onPresetComboActivated);
    connect(m_selectCalcsBtn, &QPushButton::clicked,
            this, &CalculationPanel::onSelectCalculationsClicked);
    connect(m_newCalcBtn, &QPushButton::clicked,
            this, &CalculationPanel::onNewCalculationClicked);
    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &CalculationPanel::filterBlocks);
    connect(m_expandAllBtn, &QPushButton::clicked,
            this, &CalculationPanel::onExpandAllClicked);
    connect(m_collapseAllBtn, &QPushButton::clicked,
            this, &CalculationPanel::onCollapseAllClicked);
    connect(m_clearAllBtn, &QPushButton::clicked,
            this, &CalculationPanel::onClearAllClicked);
}

void CalculationPanel::refreshAllBlocks()
{
    for (CalculationBlock* block : m_blocks) {
        block->updateValuesFromModel();
    }
    updateProgressCounter();
}

void CalculationPanel::filterBlocks(const QString& query)
{
    QString q = query.trimmed().toLower();
    for (CalculationBlock* block : m_blocks) {
        if (!block->model()) continue;

        bool match = q.isEmpty() ||
                     block->model()->name().contains(q, Qt::CaseInsensitive) ||
                     block->model()->formulaText().contains(q, Qt::CaseInsensitive);

        block->setVisible(match);
    }
}

void CalculationPanel::updateProgressCounter()
{
    if (!m_engine) return;
    m_progressLabel->setText(QString("%1/%2 Concluídos")
                                 .arg(m_engine->completedCount())
                                 .arg(m_engine->activeCount()));
}

void CalculationPanel::onExpandAllClicked()
{
    for (CalculationBlock* block : m_blocks) {
        block->setCollapsed(false);
    }
}

void CalculationPanel::onCollapseAllClicked()
{
    for (CalculationBlock* block : m_blocks) {
        block->setCollapsed(true);
    }
}

void CalculationPanel::onClearAllClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Limpar Dados",
        "Tem certeza que deseja redefinir os campos dos cálculos ativos?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes && m_engine) {
        m_engine->clearAll();
        refreshAllBlocks();
    }
}
