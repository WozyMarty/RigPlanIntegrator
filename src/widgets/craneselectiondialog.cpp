#include "craneselectiondialog.h"
#include "models/cranemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QScrollArea>
#include <QMessageBox>
#include <QDate>
#include <cmath>

CraneSelectionDialog::CraneSelectionDialog(const RiggingPlanData& currentPlanData, QWidget* parent)
    : QDialog(parent)
    , m_planData(currentPlanData)
{
    setWindowTitle("🏗️ Selecionar e Gerenciar Guindastes - Plano de Rigging");
    resize(940, 680);
    setMinimumSize(860, 600);

    setupUi();
    refreshCraneList();

    // Try to select crane matching planData modelo, or default to first
    bool found = false;
    for (int i = 0; i < m_craneList->count(); ++i) {
        if (m_craneList->item(i)->text().contains(m_planData.guindasteModelo, Qt::CaseInsensitive)) {
            m_craneList->setCurrentRow(i);
            found = true;
            break;
        }
    }
    if (!found && m_craneList->count() > 0) {
        m_craneList->setCurrentRow(0);
    }
}

void CraneSelectionDialog::setupUi()
{
    setStyleSheet(
        "QDialog {"
        "   background-color: #0D1117;"
        "   color: #E6EDF3;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        "QLabel {"
        "   color: #C9D1D9;"
        "   font-size: 12px;"
        "}"
        "QGroupBox {"
        "   background-color: #161B22;"
        "   border: 1px solid #30363D;"
        "   border-radius: 8px;"
        "   margin-top: 14px;"
        "   padding: 12px;"
        "   font-weight: bold;"
        "   color: #58A6FF;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   left: 12px;"
        "   padding: 0 6px;"
        "   background-color: #0D1117;"
        "}"
        "QLineEdit, QDoubleSpinBox, QComboBox {"
        "   background-color: #0D1117;"
        "   color: #FFFFFF;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 5px 8px;"
        "   font-size: 12px;"
        "}"
        "QLineEdit:focus, QDoubleSpinBox:focus, QComboBox:focus {"
        "   border-color: #1F6FEB;"
        "}"
        "QListWidget {"
        "   background-color: #161B22;"
        "   color: #E6EDF3;"
        "   border: 1px solid #30363D;"
        "   border-radius: 8px;"
        "   padding: 6px;"
        "}"
        "QListWidget::item {"
        "   padding: 8px 10px;"
        "   border-radius: 6px;"
        "   margin-bottom: 4px;"
        "   border: 1px solid transparent;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #21262D;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #1F6FEB;"
        "   color: #FFFFFF;"
        "   font-weight: bold;"
        "}"
        "QPushButton#actionBtn {"
        "   background-color: #21262D;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 6px 12px;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#actionBtn:hover {"
        "   background-color: #30363D;"
        "   color: #FFFFFF;"
        "}"
        "QPushButton#primaryBtn {"
        "   background-color: #238636;"
        "   color: #FFFFFF;"
        "   border: 1px solid #2EA043;"
        "   border-radius: 6px;"
        "   padding: 8px 18px;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#primaryBtn:hover {"
        "   background-color: #2EA043;"
        "}"
        "QPushButton#saveBtn {"
        "   background-color: #1F6FEB;"
        "   color: #FFFFFF;"
        "   border: 1px solid #388BFD;"
        "   border-radius: 6px;"
        "   padding: 6px 14px;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#saveBtn:hover {"
        "   background-color: #388BFD;"
        "}"
    );

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(16, 16, 16, 16);
    rootLayout->setSpacing(12);

    // --- Header Banner ---
    QFrame* headerFrame = new QFrame(this);
    headerFrame->setStyleSheet("background-color: #161B22; border: 1px solid #30363D; border-radius: 8px; padding: 10px;");
    QHBoxLayout* headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(8, 4, 8, 4);

    QLabel* titleIcon = new QLabel("🏗️", headerFrame);
    titleIcon->setStyleSheet("font-size: 24px;");

    QVBoxLayout* headerTextLayout = new QVBoxLayout();
    headerTextLayout->setSpacing(2);
    QLabel* titleLabel = new QLabel("Selecionar Guindaste & Configuração Operacional", headerFrame);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #FFFFFF;");
    QLabel* subTitleLabel = new QLabel("Selecione um guindaste registrado, adicione um novo ou use os dados do 'Editar Plano'. Atualiza dinamicamente as tabelas do Plano de Rigging.", headerFrame);
    subTitleLabel->setStyleSheet("font-size: 11px; color: #8B949E;");
    headerTextLayout->addWidget(titleLabel);
    headerTextLayout->addWidget(subTitleLabel);

    headerLayout->addWidget(titleIcon);
    headerLayout->addLayout(headerTextLayout, 1);
    rootLayout->addWidget(headerFrame);

    // --- Splitter (Left: list, Right: editor + preview) ---
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(false);

    // -------------------------------------------------------------
    // LEFT PANEL: Lista de Guindastes
    // -------------------------------------------------------------
    QWidget* leftWidget = new QWidget(splitter);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 8, 0);
    leftLayout->setSpacing(8);

    QLabel* listTitle = new QLabel("Guindastes Registrados / Utilizados:", leftWidget);
    listTitle->setStyleSheet("font-weight: bold; color: #FFFFFF; font-size: 12px;");
    leftLayout->addWidget(listTitle);

    m_searchEdit = new QLineEdit(leftWidget);
    m_searchEdit->setPlaceholderText("🔍 Filtrar por modelo, caminhão, placa...");
    leftLayout->addWidget(m_searchEdit);

    m_craneList = new QListWidget(leftWidget);
    leftLayout->addWidget(m_craneList, 1);

    // Buttons under list
    QGridLayout* btnGrid = new QGridLayout();
    btnGrid->setSpacing(6);

    m_addNewBtn = new QPushButton("➕ Novo Guindaste", leftWidget);
    m_addNewBtn->setObjectName("actionBtn");
    m_addNewBtn->setToolTip("Cadastrar um novo guindaste no catálogo");

    m_usePlanDataBtn = new QPushButton("📥 Utilizar Dados do Plano", leftWidget);
    m_usePlanDataBtn->setObjectName("actionBtn");
    m_usePlanDataBtn->setToolTip("Importar os dados do guindaste atualmente configurados no 'Editar Plano'");

    m_duplicateBtn = new QPushButton("📋 Duplicar", leftWidget);
    m_duplicateBtn->setObjectName("actionBtn");

    m_deleteBtn = new QPushButton("🗑️ Excluir", leftWidget);
    m_deleteBtn->setObjectName("actionBtn");
    m_deleteBtn->setStyleSheet("QPushButton#actionBtn:hover { background-color: #8E1519; color: #FFFFFF; border-color: #F85149; }");

    btnGrid->addWidget(m_addNewBtn, 0, 0);
    btnGrid->addWidget(m_usePlanDataBtn, 0, 1);
    btnGrid->addWidget(m_duplicateBtn, 1, 0);
    btnGrid->addWidget(m_deleteBtn, 1, 1);
    leftLayout->addLayout(btnGrid);

    splitter->addWidget(leftWidget);

    // -------------------------------------------------------------
    // RIGHT PANEL: Detalhes, Configuração & Pré-visualização Dinâmica
    // -------------------------------------------------------------
    QScrollArea* scrollArea = new QScrollArea(splitter);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { background-color: transparent; border: none; }");

    QWidget* rightWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(8, 0, 0, 0);
    rightLayout->setSpacing(12);

    // Group 1: Informações do Guindaste
    QGroupBox* infoGroup = new QGroupBox("📋 Informações do Guindaste", rightWidget);
    QFormLayout* infoLayout = new QFormLayout(infoGroup);
    infoLayout->setSpacing(6);

    m_modeloEdit = new QLineEdit(infoGroup);
    m_fabricanteEdit = new QLineEdit(infoGroup);
    m_caminhaoEdit = new QLineEdit(infoGroup);
    m_placasEdit = new QLineEdit(infoGroup);
    m_chassiEdit = new QLineEdit(infoGroup);
    m_anoEdit = new QLineEdit(infoGroup);

    infoLayout->addRow("Modelo:", m_modeloEdit);
    infoLayout->addRow("Fabricante:", m_fabricanteEdit);
    infoLayout->addRow("Caminhão:", m_caminhaoEdit);
    infoLayout->addRow("Placas:", m_placasEdit);
    infoLayout->addRow("Chassi:", m_chassiEdit);
    infoLayout->addRow("Ano de Fabricação:", m_anoEdit);
    rightLayout->addWidget(infoGroup);

    // Group 2: Configuração Operacional
    QGroupBox* configGroup = new QGroupBox("⚙️ Configuração Operacional (Atualiza a Seção do Plano)", rightWidget);
    QGridLayout* cfgGrid = new QGridLayout(configGroup);
    cfgGrid->setSpacing(8);

    m_capNominalSpin = new QDoubleSpinBox(configGroup);
    m_capNominalSpin->setRange(1.0, 5000.0);
    m_capNominalSpin->setDecimals(1);
    m_capNominalSpin->setSuffix(" t");

    m_compLancaSpin = new QDoubleSpinBox(configGroup);
    m_compLancaSpin->setRange(1.0, 200.0);
    m_compLancaSpin->setDecimals(1);
    m_compLancaSpin->setSuffix(" m");

    m_raioOperacaoSpin = new QDoubleSpinBox(configGroup);
    m_raioOperacaoSpin->setRange(1.0, 200.0);
    m_raioOperacaoSpin->setDecimals(1);
    m_raioOperacaoSpin->setSuffix(" m");

    m_capCargaNoRaioSpin = new QDoubleSpinBox(configGroup);
    m_capCargaNoRaioSpin->setRange(0.1, 5000.0);
    m_capCargaNoRaioSpin->setDecimals(1);
    m_capCargaNoRaioSpin->setSuffix(" t");

    m_quadranteInicialCombo = new QComboBox(configGroup);
    m_quadranteInicialCombo->setEditable(true);
    m_quadranteInicialCombo->addItems({"LATERAL", "TRASEIRA", "360°", "FRENTE", "SOBRE A CABINE"});

    m_quadranteFinalCombo = new QComboBox(configGroup);
    m_quadranteFinalCombo->setEditable(true);
    m_quadranteFinalCombo->addItems({"LATERAL", "TRASEIRA", "360°", "FRENTE", "SOBRE A CABINE"});

    cfgGrid->addWidget(new QLabel("Capacidade Nominal:"), 0, 0);
    cfgGrid->addWidget(m_capNominalSpin, 0, 1);
    cfgGrid->addWidget(new QLabel("Comprimento da Lança:"), 0, 2);
    cfgGrid->addWidget(m_compLancaSpin, 0, 3);

    cfgGrid->addWidget(new QLabel("Raio de Operação:"), 1, 0);
    cfgGrid->addWidget(m_raioOperacaoSpin, 1, 1);
    cfgGrid->addWidget(new QLabel("Capacidade no Raio:"), 1, 2);
    cfgGrid->addWidget(m_capCargaNoRaioSpin, 1, 3);

    cfgGrid->addWidget(new QLabel("Quadrante Inicial:"), 2, 0);
    cfgGrid->addWidget(m_quadranteInicialCombo, 2, 1);
    cfgGrid->addWidget(new QLabel("Quadrante Final:"), 2, 2);
    cfgGrid->addWidget(m_quadranteFinalCombo, 2, 3);

    rightLayout->addWidget(configGroup);

    // Group 3: Live Preview Table (Matching exact tables from user's image)
    QGroupBox* previewGroup = new QGroupBox("👁️ Pré-Visualização Dinâmica no Plano de Rigging (Região Alvo)", rightWidget);
    QVBoxLayout* prevLayout = new QVBoxLayout(previewGroup);
    prevLayout->setSpacing(6);

    QFrame* paperPreviewFrame = new QFrame(previewGroup);
    paperPreviewFrame->setStyleSheet("background-color: #FFFFFF; border: 1px solid #000000; border-radius: 2px; padding: 4px;");
    QHBoxLayout* tablesLayout = new QHBoxLayout(paperPreviewFrame);
    tablesLayout->setContentsMargins(2, 2, 2, 2);
    tablesLayout->setSpacing(6);

    // Left Table: CONFIGURAÇÃO DO GUINDASTE & QUADRANTE OPERACIONAL
    QFrame* leftTableFrame = new QFrame(paperPreviewFrame);
    leftTableFrame->setStyleSheet("background-color: #FFFFFF; border: 1px solid #000000;");
    QVBoxLayout* ltfLayout = new QVBoxLayout(leftTableFrame);
    ltfLayout->setContentsMargins(0, 0, 0, 0);
    ltfLayout->setSpacing(0);

    QLabel* cfgHeader = new QLabel("CONFIGURAÇÃO DO GUINDASTE", leftTableFrame);
    cfgHeader->setAlignment(Qt::AlignCenter);
    cfgHeader->setStyleSheet("background-color: #FFFFFF; color: #000000; font-weight: bold; font-size: 10px; border-bottom: 1px solid #000000; padding: 2px;");
    ltfLayout->addWidget(cfgHeader);

    QGridLayout* lTableGrid = new QGridLayout();
    lTableGrid->setContentsMargins(4, 2, 4, 2);
    lTableGrid->setSpacing(2);

    auto makeTableLabel = [](const QString& text, bool bold = false, Qt::Alignment align = Qt::AlignLeft) {
        QLabel* l = new QLabel(text);
        l->setStyleSheet("color: #000000; font-size: 10px;" + QString(bold ? " font-weight: bold;" : ""));
        l->setAlignment(align | Qt::AlignVCenter);
        return l;
    };

    m_prevCapNominal = makeTableLabel("30,0t", true, Qt::AlignRight);
    m_prevCompLanca = makeTableLabel("5,1m", true, Qt::AlignRight);
    m_prevRaioOperacao = makeTableLabel("5,0m", true, Qt::AlignRight);
    m_prevCapCarga = makeTableLabel("8,0t", true, Qt::AlignRight);

    lTableGrid->addWidget(makeTableLabel("Capacidade nominal"), 0, 0);
    lTableGrid->addWidget(m_prevCapNominal, 0, 1);
    lTableGrid->addWidget(makeTableLabel("Comprimento da lança"), 1, 0);
    lTableGrid->addWidget(m_prevCompLanca, 1, 1);
    lTableGrid->addWidget(makeTableLabel("Raio de operação"), 2, 0);
    lTableGrid->addWidget(m_prevRaioOperacao, 2, 1);
    lTableGrid->addWidget(makeTableLabel("Capacidade de carga"), 3, 0);
    lTableGrid->addWidget(m_prevCapCarga, 3, 1);
    ltfLayout->addLayout(lTableGrid);

    QLabel* quadHeader = new QLabel("QUADRANTE OPERACIONAL", leftTableFrame);
    quadHeader->setAlignment(Qt::AlignCenter);
    quadHeader->setStyleSheet("background-color: #FFFFFF; color: #000000; font-weight: bold; font-size: 10px; border-top: 1px solid #000000; border-bottom: 1px solid #000000; padding: 2px;");
    ltfLayout->addWidget(quadHeader);

    QGridLayout* quadGrid = new QGridLayout();
    quadGrid->setContentsMargins(4, 2, 4, 2);
    quadGrid->setSpacing(2);
    m_prevQuadInicial = makeTableLabel("LATERAL", true, Qt::AlignCenter);
    m_prevQuadFinal = makeTableLabel("LATERAL", true, Qt::AlignCenter);

    quadGrid->addWidget(makeTableLabel("Inicial", false, Qt::AlignCenter), 0, 0);
    quadGrid->addWidget(makeTableLabel("Final", false, Qt::AlignCenter), 0, 1);
    quadGrid->addWidget(m_prevQuadInicial, 1, 0);
    quadGrid->addWidget(m_prevQuadFinal, 1, 1);
    ltfLayout->addLayout(quadGrid);

    tablesLayout->addWidget(leftTableFrame, 1);

    // Right Table: COMPOSIÇÃO DA CARGA BRUTA
    QFrame* rightTableFrame = new QFrame(paperPreviewFrame);
    rightTableFrame->setStyleSheet("background-color: #FFFFFF; border: 1px solid #000000;");
    QVBoxLayout* rtfLayout = new QVBoxLayout(rightTableFrame);
    rtfLayout->setContentsMargins(0, 0, 0, 0);
    rtfLayout->setSpacing(0);

    QLabel* cargaHeader = new QLabel("COMPOSIÇÃO DA CARGA BRUTA", rightTableFrame);
    cargaHeader->setAlignment(Qt::AlignCenter);
    cargaHeader->setStyleSheet("background-color: #FFFFFF; color: #000000; font-weight: bold; font-size: 10px; border-bottom: 1px solid #000000; padding: 2px;");
    rtfLayout->addWidget(cargaHeader);

    QGridLayout* rTableGrid = new QGridLayout();
    rTableGrid->setContentsMargins(4, 2, 4, 2);
    rTableGrid->setSpacing(2);

    m_prevCargaLiquida = makeTableLabel("3450kg", true, Qt::AlignRight);
    m_prevLingada = makeTableLabel("50,62kg", true, Qt::AlignRight);
    m_prevBalancim = makeTableLabel("250kg", true, Qt::AlignRight);
    m_prevCargasExtras = makeTableLabel("-kg", true, Qt::AlignRight);
    m_prevCargaBruta = makeTableLabel("3751kg", true, Qt::AlignRight);

    rTableGrid->addWidget(makeTableLabel("Carga líquida"), 0, 0);
    rTableGrid->addWidget(m_prevCargaLiquida, 0, 1);
    rTableGrid->addWidget(makeTableLabel("Lingada / Amarração"), 1, 0);
    rTableGrid->addWidget(m_prevLingada, 1, 1);
    rTableGrid->addWidget(makeTableLabel("Dispositivos / Balancim"), 2, 0);
    rTableGrid->addWidget(m_prevBalancim, 2, 1);
    rTableGrid->addWidget(makeTableLabel("Cargas extras"), 3, 0);
    rTableGrid->addWidget(m_prevCargasExtras, 3, 1);
    rTableGrid->addWidget(makeTableLabel("Carga bruta", true), 4, 0);
    rTableGrid->addWidget(m_prevCargaBruta, 4, 1);
    rtfLayout->addLayout(rTableGrid);

    tablesLayout->addWidget(rightTableFrame, 1);
    prevLayout->addWidget(paperPreviewFrame);

    // Utilization rate status bar
    QHBoxLayout* tuLayout = new QHBoxLayout();
    m_prevTaxaUtilizacao = new QLabel("Taxa de Utilização do Guindaste: -- %", previewGroup);
    m_prevTaxaUtilizacao->setStyleSheet("font-weight: bold; font-size: 11px; color: #3FB950;");
    m_saveCatalogBtn = new QPushButton("💾 Salvar no Catálogo", previewGroup);
    m_saveCatalogBtn->setObjectName("saveBtn");
    m_saveCatalogBtn->setToolTip("Salvar as alterações deste guindaste no catálogo permanente");

    tuLayout->addWidget(m_prevTaxaUtilizacao, 1);
    tuLayout->addWidget(m_saveCatalogBtn);
    prevLayout->addLayout(tuLayout);

    rightLayout->addWidget(previewGroup);
    scrollArea->setWidget(rightWidget);
    splitter->addWidget(scrollArea);
    splitter->setStretchFactor(0, 4);
    splitter->setStretchFactor(1, 6);

    rootLayout->addWidget(splitter, 1);

    // --- Bottom Buttons ---
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(10);

    QLabel* tipLabel = new QLabel("💡 Ao clicar em 'Aplicar Guindaste', o Plano de Rigging será atualizado instantaneamente.", this);
    tipLabel->setStyleSheet("color: #8B949E; font-size: 11px;");

    m_cancelBtn = new QPushButton("Cancelar", this);
    m_cancelBtn->setObjectName("actionBtn");

    m_applyBtn = new QPushButton("✓ Aplicar Guindaste Selecionado no Plano", this);
    m_applyBtn->setObjectName("primaryBtn");

    bottomLayout->addWidget(tipLabel, 1);
    bottomLayout->addWidget(m_cancelBtn);
    bottomLayout->addWidget(m_applyBtn);
    rootLayout->addLayout(bottomLayout);

    // --- Connections ---
    connect(m_searchEdit, &QLineEdit::textChanged, this, &CraneSelectionDialog::onSearchTextChanged);
    connect(m_craneList, &QListWidget::currentRowChanged, this, &CraneSelectionDialog::onCraneListItemSelected);
    connect(m_addNewBtn, &QPushButton::clicked, this, &CraneSelectionDialog::onAddNewCrane);
    connect(m_usePlanDataBtn, &QPushButton::clicked, this, &CraneSelectionDialog::onUsePlanData);
    connect(m_duplicateBtn, &QPushButton::clicked, this, &CraneSelectionDialog::onDuplicateCrane);
    connect(m_deleteBtn, &QPushButton::clicked, this, &CraneSelectionDialog::onDeleteCrane);
    connect(m_saveCatalogBtn, &QPushButton::clicked, this, &CraneSelectionDialog::onSaveCraneToCatalog);
    connect(m_applyBtn, &QPushButton::clicked, this, &CraneSelectionDialog::onApplyAndClose);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    // Live update connections
    connect(m_capNominalSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CraneSelectionDialog::onFieldChanged);
    connect(m_compLancaSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CraneSelectionDialog::onFieldChanged);
    connect(m_raioOperacaoSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CraneSelectionDialog::onFieldChanged);
    connect(m_capCargaNoRaioSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CraneSelectionDialog::onFieldChanged);
    connect(m_quadranteInicialCombo, &QComboBox::currentTextChanged, this, &CraneSelectionDialog::onFieldChanged);
    connect(m_quadranteFinalCombo, &QComboBox::currentTextChanged, this, &CraneSelectionDialog::onFieldChanged);
}

void CraneSelectionDialog::refreshCraneList(const QString& filter)
{
    m_craneList->clear();
    const auto cranes = CraneManager::instance()->allCranes();

    for (const auto& c : cranes) {
        if (!filter.isEmpty()) {
            bool matches = c.modelo.contains(filter, Qt::CaseInsensitive)
                        || c.fabricante.contains(filter, Qt::CaseInsensitive)
                        || c.caminhao.contains(filter, Qt::CaseInsensitive)
                        || c.placas.contains(filter, Qt::CaseInsensitive);
            if (!matches) continue;
        }

        QString itemText = QString("%1\n%2 | Placa: %3 | Cap: %4 t")
                               .arg(c.modelo.isEmpty() ? "(Sem nome)" : c.modelo)
                               .arg(c.caminhao)
                               .arg(c.placas)
                               .arg(QString::number(c.capNominal, 'f', 1).replace('.', ','));

        QListWidgetItem* item = new QListWidgetItem(itemText, m_craneList);
        item->setData(Qt::UserRole, c.id);
        m_craneList->addItem(item);
    }
}

void CraneSelectionDialog::onSearchTextChanged(const QString& text)
{
    refreshCraneList(text.trimmed());
    if (m_craneList->count() > 0) {
        m_craneList->setCurrentRow(0);
    }
}

void CraneSelectionDialog::onCraneListItemSelected(int row)
{
    if (row < 0 || row >= m_craneList->count()) return;
    QString id = m_craneList->item(row)->data(Qt::UserRole).toString();
    CraneProfile crane = CraneManager::instance()->craneById(id);
    m_isCreatingNew = false;
    loadCraneIntoForm(crane);
}

void CraneSelectionDialog::loadCraneIntoForm(const CraneProfile& crane)
{
    m_updatingForm = true;
    m_currentCrane = crane;

    m_modeloEdit->setText(crane.modelo);
    m_fabricanteEdit->setText(crane.fabricante);
    m_caminhaoEdit->setText(crane.caminhao);
    m_placasEdit->setText(crane.placas);
    m_chassiEdit->setText(crane.chassi);
    m_anoEdit->setText(crane.ano);

    m_capNominalSpin->setValue(crane.capNominal);
    m_compLancaSpin->setValue(crane.compLanca);
    m_raioOperacaoSpin->setValue(crane.raioOperacao);
    m_capCargaNoRaioSpin->setValue(crane.capCargaNoRaio);

    int idxInit = m_quadranteInicialCombo->findText(crane.quadranteInicial);
    if (idxInit >= 0) m_quadranteInicialCombo->setCurrentIndex(idxInit);
    else m_quadranteInicialCombo->setEditText(crane.quadranteInicial);

    int idxEnd = m_quadranteFinalCombo->findText(crane.quadranteFinal);
    if (idxEnd >= 0) m_quadranteFinalCombo->setCurrentIndex(idxEnd);
    else m_quadranteFinalCombo->setEditText(crane.quadranteFinal);

    m_updatingForm = false;
    updateLiveTablePreview();
}

CraneProfile CraneSelectionDialog::formToCrane() const
{
    CraneProfile c = m_currentCrane;
    c.modelo = m_modeloEdit->text().trimmed();
    c.fabricante = m_fabricanteEdit->text().trimmed();
    c.caminhao = m_caminhaoEdit->text().trimmed();
    c.placas = m_placasEdit->text().trimmed();
    c.chassi = m_chassiEdit->text().trimmed();
    c.ano = m_anoEdit->text().trimmed();

    c.capNominal = m_capNominalSpin->value();
    c.compLanca = m_compLancaSpin->value();
    c.raioOperacao = m_raioOperacaoSpin->value();
    c.capCargaNoRaio = m_capCargaNoRaioSpin->value();
    c.quadranteInicial = m_quadranteInicialCombo->currentText().trimmed();
    c.quadranteFinal = m_quadranteFinalCombo->currentText().trimmed();

    return c;
}

void CraneSelectionDialog::onFieldChanged()
{
    if (m_updatingForm) return;
    updateLiveTablePreview();
}

void CraneSelectionDialog::updateLiveTablePreview()
{
    // Update left preview table
    m_prevCapNominal->setText(QString("%1t").arg(QString::number(m_capNominalSpin->value(), 'f', 1).replace('.', ',')));
    m_prevCompLanca->setText(QString("%1m").arg(QString::number(m_compLancaSpin->value(), 'f', 1).replace('.', ',')));
    m_prevRaioOperacao->setText(QString("%1m").arg(QString::number(m_raioOperacaoSpin->value(), 'f', 1).replace('.', ',')));
    m_prevCapCarga->setText(QString("%1t").arg(QString::number(m_capCargaNoRaioSpin->value(), 'f', 1).replace('.', ',')));

    m_prevQuadInicial->setText(m_quadranteInicialCombo->currentText());
    m_prevQuadFinal->setText(m_quadranteFinalCombo->currentText());

    // Update right preview table with current plan data
    double pesoLingada = m_planData.pesoTotalLingada();
    double pesoBalancim = m_planData.pesoTotalBalancim();
    double cargaBrutaKg = m_planData.cargaBruta();

    m_prevCargaLiquida->setText(QString("%1kg").arg(QString::number(m_planData.cargaPeso, 'f', 0)));
    m_prevLingada->setText(QString("%1kg").arg(QString::number(pesoLingada, 'f', 2).replace('.', ',')));
    m_prevBalancim->setText(QString("%1kg").arg(QString::number(pesoBalancim, 'f', 0)));
    m_prevCargasExtras->setText(m_planData.cargasExtras > 0 ? QString("%1kg").arg(QString::number(m_planData.cargasExtras, 'f', 0)) : "-kg");
    m_prevCargaBruta->setText(QString("%1kg").arg(QString::number(std::round(cargaBrutaKg), 'f', 0)));

    // Calculate live utilization rate
    double capRaio = m_capCargaNoRaioSpin->value();
    if (capRaio > 0.0) {
        double cargaBrutaTon = cargaBrutaKg / 1000.0;
        double tu = (cargaBrutaTon / capRaio) * 100.0;
        QString color = (tu <= 85.0) ? "#3FB950" : (tu <= 100.0 ? "#D29922" : "#F85149");
        m_prevTaxaUtilizacao->setText(QString("Taxa de Utilização do Guindaste: %1% (Carga: %2 t / Cap: %3 t)")
                                         .arg(QString::number(tu, 'f', 1).replace('.', ','))
                                         .arg(QString::number(cargaBrutaTon, 'f', 2).replace('.', ','))
                                         .arg(QString::number(capRaio, 'f', 1).replace('.', ',')));
        m_prevTaxaUtilizacao->setStyleSheet(QString("font-weight: bold; font-size: 11px; color: %1;").arg(color));
    } else {
        m_prevTaxaUtilizacao->setText("Taxa de Utilização: -- %");
    }
}

void CraneSelectionDialog::onAddNewCrane()
{
    m_isCreatingNew = true;
    CraneProfile newCrane;
    newCrane.id = QString();
    newCrane.modelo = "Novo Guindaste";
    newCrane.fabricante = "";
    newCrane.caminhao = "";
    newCrane.placas = "";
    newCrane.chassi = "";
    newCrane.ano = QString::number(QDate::currentDate().year());
    newCrane.capNominal = 50.0;
    newCrane.compLanca = 15.0;
    newCrane.raioOperacao = 8.0;
    newCrane.capCargaNoRaio = 10.0;
    newCrane.quadranteInicial = "LATERAL";
    newCrane.quadranteFinal = "LATERAL";

    loadCraneIntoForm(newCrane);
    m_modeloEdit->setFocus();
    m_modeloEdit->selectAll();
}

void CraneSelectionDialog::onUsePlanData()
{
    m_updatingForm = true;
    m_isCreatingNew = true;

    CraneProfile planCrane;
    planCrane.id = QString();
    planCrane.modelo = m_planData.guindasteModelo.isEmpty() ? "Guindaste do Plano" : m_planData.guindasteModelo;
    planCrane.caminhao = m_planData.guindasteCaminhao;
    planCrane.placas = m_planData.guindastePlacas;
    planCrane.chassi = m_planData.guindasteChassi;
    planCrane.ano = m_planData.guindasteAno;
    planCrane.capNominal = m_planData.capNominal;
    planCrane.compLanca = m_planData.compLanca;
    planCrane.raioOperacao = m_planData.raioOperacao;
    planCrane.capCargaNoRaio = m_planData.capCargaNoRaio;
    planCrane.quadranteInicial = m_planData.quadranteInicial;
    planCrane.quadranteFinal = m_planData.quadranteFinal;

    loadCraneIntoForm(planCrane);
    m_updatingForm = false;
    updateLiveTablePreview();

    QMessageBox::information(this, "Dados Carregados",
                             "Os parâmetros do guindaste atualmente preenchidos no 'Editar Plano' foram carregados no formulário com sucesso!\n\n"
                             "Você pode ajustar os valores e clicar em 'Salvar no Catálogo' ou 'Aplicar Guindaste no Plano'.");
}

void CraneSelectionDialog::onDuplicateCrane()
{
    CraneProfile current = formToCrane();
    current.id = QString();
    current.modelo += " (Cópia)";
    CraneManager::instance()->addCrane(current);
    refreshCraneList();

    // Select the newly added copy
    for (int i = 0; i < m_craneList->count(); ++i) {
        if (m_craneList->item(i)->text().startsWith(current.modelo)) {
            m_craneList->setCurrentRow(i);
            break;
        }
    }
}

void CraneSelectionDialog::onDeleteCrane()
{
    int row = m_craneList->currentRow();
    if (row < 0) return;

    QString id = m_craneList->item(row)->data(Qt::UserRole).toString();
    CraneProfile c = CraneManager::instance()->craneById(id);

    int ret = QMessageBox::question(this, "Excluir Guindaste",
                                    QString("Deseja realmente remover o guindaste '%1' do catálogo?")
                                        .arg(c.modelo),
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        CraneManager::instance()->removeCrane(id);
        refreshCraneList();
        if (m_craneList->count() > 0) {
            m_craneList->setCurrentRow(0);
        }
    }
}

void CraneSelectionDialog::onSaveCraneToCatalog()
{
    CraneProfile c = formToCrane();
    if (c.modelo.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Informe ao menos o modelo do guindaste.");
        return;
    }

    if (m_isCreatingNew || c.id.isEmpty()) {
        CraneManager::instance()->addCrane(c);
        m_isCreatingNew = false;
    } else {
        CraneManager::instance()->updateCrane(c);
    }

    refreshCraneList();

    // Reselect
    for (int i = 0; i < m_craneList->count(); ++i) {
        if (m_craneList->item(i)->data(Qt::UserRole).toString() == c.id ||
            m_craneList->item(i)->text().startsWith(c.modelo)) {
            m_craneList->setCurrentRow(i);
            break;
        }
    }

    QMessageBox::information(this, "Salvo", "Guindaste salvo no catálogo permanente com sucesso!");
}

void CraneSelectionDialog::onApplyAndClose()
{
    m_currentCrane = formToCrane();
    if (m_currentCrane.modelo.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Por favor, selecione ou informe o modelo do guindaste.");
        return;
    }

    // Auto-save to catalog if new or updated
    if (m_isCreatingNew || m_currentCrane.id.isEmpty()) {
        CraneManager::instance()->addCrane(m_currentCrane);
    } else {
        CraneManager::instance()->updateCrane(m_currentCrane);
    }

    accept();
}

CraneProfile CraneSelectionDialog::selectedCrane() const
{
    return m_currentCrane;
}
