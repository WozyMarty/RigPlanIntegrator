#include "riggingplandatadialog.h"
#include "craneselectiondialog.h"
#include "models/cranemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>

RiggingPlanDataDialog::RiggingPlanDataDialog(const RiggingPlanData& data, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("📝 Editor de Dados do Plano de Rigging (Campos Editáveis)");
    resize(780, 560);
    setupUi();
    populateFields(data);
    recalculateTotals();
}

void RiggingPlanDataDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QTabWidget* tabs = new QTabWidget(this);

    // ==========================================
    // TAB 1: IDENTIFICAÇÃO & CARGA
    // ==========================================
    QWidget* tab1 = new QWidget();
    QVBoxLayout* tab1Layout = new QVBoxLayout(tab1);

    QGroupBox* identGroup = new QGroupBox("Identificação do Plano", tab1);
    QFormLayout* idLayout = new QFormLayout(identGroup);
    m_clienteEdit = new QLineEdit(tab1);
    m_localEdit = new QLineEdit(tab1);
    m_dataEdit = new QLineEdit(tab1);
    m_planoNumEdit = new QLineEdit(tab1);

    idLayout->addRow("Cliente:", m_clienteEdit);
    idLayout->addRow("Local:", m_localEdit);
    idLayout->addRow("Data:", m_dataEdit);
    idLayout->addRow("Plano de Rigging nº:", m_planoNumEdit);
    tab1Layout->addWidget(identGroup);

    QGroupBox* cargaGroup = new QGroupBox("Dados da Carga", tab1);
    QFormLayout* cargaLayout = new QFormLayout(cargaGroup);
    m_cargaDescEdit = new QLineEdit(tab1);
    m_cargaCompSpin = new QDoubleSpinBox(tab1); m_cargaCompSpin->setRange(0.1, 500.0); m_cargaCompSpin->setSuffix(" m");
    m_cargaLarguraSpin = new QDoubleSpinBox(tab1); m_cargaLarguraSpin->setRange(0.1, 500.0); m_cargaLarguraSpin->setSuffix(" m");
    m_cargaAlturaSpin = new QDoubleSpinBox(tab1); m_cargaAlturaSpin->setRange(0.1, 500.0); m_cargaAlturaSpin->setSuffix(" m");
    m_cargaPesoSpin = new QDoubleSpinBox(tab1); m_cargaPesoSpin->setRange(1.0, 1000000.0); m_cargaPesoSpin->setSuffix(" kg"); m_cargaPesoSpin->setDecimals(1);

    cargaLayout->addRow("Descrição da Carga:", m_cargaDescEdit);
    cargaLayout->addRow("Comprimento:", m_cargaCompSpin);
    cargaLayout->addRow("Largura:", m_cargaLarguraSpin);
    cargaLayout->addRow("Altura:", m_cargaAlturaSpin);
    cargaLayout->addRow("Peso Líquido:", m_cargaPesoSpin);
    tab1Layout->addWidget(cargaGroup);

    tab1Layout->addStretch(1);
    tabs->addTab(tab1, "Identificação & Carga");

    // ==========================================
    // TAB 2: LINGADA / AMARRAS
    // ==========================================
    QWidget* tab2 = new QWidget();
    QVBoxLayout* tab2Layout = new QVBoxLayout(tab2);

    QLabel* lingadaInfo = new QLabel("Linhas azuis (Qtd × Unitário) geram automaticamente os pontos verdes (Total kg):", tab2);
    lingadaInfo->setStyleSheet("color: #0D47A1; font-weight: bold;");
    tab2Layout->addWidget(lingadaInfo);

    m_lingadaTable = new QTableWidget(0, 5, tab2);
    m_lingadaTable->setHorizontalHeaderLabels({"Item", "Descrição", "Qtd. (pç)", "Unitário (kg)", "Total (kg)"});
    m_lingadaTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tab2Layout->addWidget(m_lingadaTable);

    QHBoxLayout* lingadaBtnLayout = new QHBoxLayout();
    QPushButton* addLingadaBtn = new QPushButton("+ Adicionar Item de Lingada", tab2);
    QPushButton* remLingadaBtn = new QPushButton("- Remover Item", tab2);
    m_lingadaTotalLabel = new QLabel("Peso Total da Lingada: 0,00 kg", tab2);
    m_lingadaTotalLabel->setStyleSheet("font-weight: bold; font-size: 11pt; color: #166534;");

    lingadaBtnLayout->addWidget(addLingadaBtn);
    lingadaBtnLayout->addWidget(remLingadaBtn);
    lingadaBtnLayout->addStretch(1);
    lingadaBtnLayout->addWidget(m_lingadaTotalLabel);
    tab2Layout->addLayout(lingadaBtnLayout);

    connect(addLingadaBtn, &QPushButton::clicked, this, &RiggingPlanDataDialog::onAddLingadaItem);
    connect(remLingadaBtn, &QPushButton::clicked, this, &RiggingPlanDataDialog::onRemoveLingadaItem);
    tabs->addTab(tab2, "Lingada / Amarras");

    // ==========================================
    // TAB 3: DISPOSITIVOS / BALANCIM
    // ==========================================
    QWidget* tab3 = new QWidget();
    QVBoxLayout* tab3Layout = new QVBoxLayout(tab3);

    m_balancimTable = new QTableWidget(0, 6, tab3);
    m_balancimTable->setHorizontalHeaderLabels({"Item", "Descrição", "Ref. /Desenho", "Qtd. (pç)", "Unitário (kg)", "Total (kg)"});
    m_balancimTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tab3Layout->addWidget(m_balancimTable);

    QHBoxLayout* balancimBtnLayout = new QHBoxLayout();
    QPushButton* addBalancimBtn = new QPushButton("+ Adicionar Dispositivo", tab3);
    QPushButton* remBalancimBtn = new QPushButton("- Remover Item", tab3);
    m_balancimTotalLabel = new QLabel("Peso Total Dispositivos: 0 kg", tab3);
    m_balancimTotalLabel->setStyleSheet("font-weight: bold; font-size: 11pt; color: #166534;");

    balancimBtnLayout->addWidget(addBalancimBtn);
    balancimBtnLayout->addWidget(remBalancimBtn);
    balancimBtnLayout->addStretch(1);
    balancimBtnLayout->addWidget(m_balancimTotalLabel);
    tab3Layout->addLayout(balancimBtnLayout);

    connect(addBalancimBtn, &QPushButton::clicked, this, &RiggingPlanDataDialog::onAddBalancimItem);
    connect(remBalancimBtn, &QPushButton::clicked, this, &RiggingPlanDataDialog::onRemoveBalancimItem);
    tabs->addTab(tab3, "Dispositivos / Balancim");

    // ==========================================
    // TAB 4: GUINDASTE & OPERAÇÃO
    // ==========================================
    QWidget* tab4 = new QWidget();
    QVBoxLayout* tab4Layout = new QVBoxLayout(tab4);

    QHBoxLayout* craneCatalogLayout = new QHBoxLayout();
    QPushButton* selectFromCatalogBtn = new QPushButton("🏗️ Selecionar do Catálogo de Guindastes...", tab4);
    selectFromCatalogBtn->setStyleSheet("background-color: #1F6FEB; color: #FFFFFF; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    QPushButton* saveToCatalogBtn = new QPushButton("💾 Salvar este Guindaste no Catálogo", tab4);
    saveToCatalogBtn->setStyleSheet("background-color: #21262D; color: #C9D1D9; border: 1px solid #30363D; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    craneCatalogLayout->addWidget(selectFromCatalogBtn);
    craneCatalogLayout->addWidget(saveToCatalogBtn);
    craneCatalogLayout->addStretch(1);
    tab4Layout->addLayout(craneCatalogLayout);

    connect(selectFromCatalogBtn, &QPushButton::clicked, this, [this]() {
        CraneSelectionDialog dlg(planData(), this);
        if (dlg.exec() == QDialog::Accepted) {
            CraneProfile c = dlg.selectedCrane();
            m_guindasteModeloEdit->setText(c.modelo);
            m_guindasteCaminhaoEdit->setText(c.caminhao);
            m_guindastePlacasEdit->setText(c.placas);
            m_guindasteChassiEdit->setText(c.chassi);
            m_guindasteAnoEdit->setText(c.ano);
            m_capNominalSpin->setValue(c.capNominal);
            m_compLancaSpin->setValue(c.compLanca);
            m_raioOperacaoSpin->setValue(c.raioOperacao);
            m_capCargaNoRaioSpin->setValue(c.capCargaNoRaio);
            m_quadranteInicialEdit->setText(c.quadranteInicial);
            m_quadranteFinalEdit->setText(c.quadranteFinal);
            recalculateTotals();
        }
    });

    connect(saveToCatalogBtn, &QPushButton::clicked, this, [this]() {
        CraneProfile c;
        c.modelo = m_guindasteModeloEdit->text().trimmed();
        c.caminhao = m_guindasteCaminhaoEdit->text().trimmed();
        c.placas = m_guindastePlacasEdit->text().trimmed();
        c.chassi = m_guindasteChassiEdit->text().trimmed();
        c.ano = m_guindasteAnoEdit->text().trimmed();
        c.capNominal = m_capNominalSpin->value();
        c.compLanca = m_compLancaSpin->value();
        c.raioOperacao = m_raioOperacaoSpin->value();
        c.capCargaNoRaio = m_capCargaNoRaioSpin->value();
        c.quadranteInicial = m_quadranteInicialEdit->text().trimmed();
        c.quadranteFinal = m_quadranteFinalEdit->text().trimmed();

        if (c.modelo.isEmpty()) {
            QMessageBox::warning(this, "Aviso", "Informe o modelo do guindaste antes de salvar no catálogo.");
            return;
        }

        CraneManager::instance()->addCrane(c);
        QMessageBox::information(this, "Catálogo Atualizado", QString("O guindaste '%1' foi registrado no catálogo com sucesso!").arg(c.modelo));
    });

    QGroupBox* guindasteGroup = new QGroupBox("Informações do Guindaste", tab4);
    QFormLayout* gLayout = new QFormLayout(guindasteGroup);
    m_guindasteModeloEdit = new QLineEdit(tab4);
    m_guindasteCaminhaoEdit = new QLineEdit(tab4);
    m_guindastePlacasEdit = new QLineEdit(tab4);
    m_guindasteChassiEdit = new QLineEdit(tab4);
    m_guindasteAnoEdit = new QLineEdit(tab4);

    gLayout->addRow("Modelo:", m_guindasteModeloEdit);
    gLayout->addRow("Caminhão:", m_guindasteCaminhaoEdit);
    gLayout->addRow("Placas:", m_guindastePlacasEdit);
    gLayout->addRow("Chassi:", m_guindasteChassiEdit);
    gLayout->addRow("Ano Fabricação:", m_guindasteAnoEdit);
    tab4Layout->addWidget(guindasteGroup);

    QGroupBox* configGroup = new QGroupBox("Configuração Operacional", tab4);
    QFormLayout* cLayout = new QFormLayout(configGroup);
    m_capNominalSpin = new QDoubleSpinBox(tab4); m_capNominalSpin->setRange(1.0, 5000.0); m_capNominalSpin->setSuffix(" t");
    m_compLancaSpin = new QDoubleSpinBox(tab4); m_compLancaSpin->setRange(1.0, 200.0); m_compLancaSpin->setSuffix(" m");
    m_raioOperacaoSpin = new QDoubleSpinBox(tab4); m_raioOperacaoSpin->setRange(1.0, 200.0); m_raioOperacaoSpin->setSuffix(" m");
    m_capCargaNoRaioSpin = new QDoubleSpinBox(tab4); m_capCargaNoRaioSpin->setRange(0.1, 5000.0); m_capCargaNoRaioSpin->setSuffix(" t");
    m_quadranteInicialEdit = new QLineEdit(tab4);
    m_quadranteFinalEdit = new QLineEdit(tab4);

    cLayout->addRow("Capacidade Nominal:", m_capNominalSpin);
    cLayout->addRow("Comprimento da Lança:", m_compLancaSpin);
    cLayout->addRow("Raio de Operação:", m_raioOperacaoSpin);
    cLayout->addRow("Capacidade de Carga no Raio:", m_capCargaNoRaioSpin);
    cLayout->addRow("Quadrante Inicial:", m_quadranteInicialEdit);
    cLayout->addRow("Quadrante Final:", m_quadranteFinalEdit);
    tab4Layout->addWidget(configGroup);

    tab4Layout->addStretch(1);
    tabs->addTab(tab4, "Guindaste & Operação");

    // ==========================================
    // TAB 5: EXTRAS, ART & ASSINATURAS
    // ==========================================
    QWidget* tab5 = new QWidget();
    QVBoxLayout* tab5Layout = new QVBoxLayout(tab5);

    QGroupBox* extraGroup = new QGroupBox("Cargas Extras & ART", tab5);
    QFormLayout* exLayout = new QFormLayout(extraGroup);
    m_cargasExtrasSpin = new QDoubleSpinBox(tab5); m_cargasExtrasSpin->setRange(0.0, 100000.0); m_cargasExtrasSpin->setSuffix(" kg");
    m_artNumeroEdit = new QLineEdit(tab5);
    m_observacoesEdit = new QLineEdit(tab5);

    exLayout->addRow("Cargas Extras:", m_cargasExtrasSpin);
    exLayout->addRow("ART nº:", m_artNumeroEdit);
    exLayout->addRow("Observações / Anexos:", m_observacoesEdit);
    tab5Layout->addWidget(extraGroup);

    QGroupBox* assGroup = new QGroupBox("Assinaturas do Documento", tab5);
    QFormLayout* assLayout = new QFormLayout(assGroup);
    m_riggerNomeEdit = new QLineEdit(tab5);
    m_supervisorNomeEdit = new QLineEdit(tab5);

    assLayout->addRow("Rigger Responsável:", m_riggerNomeEdit);
    assLayout->addRow("Supervisor Responsável:", m_supervisorNomeEdit);
    tab5Layout->addWidget(assGroup);

    tab5Layout->addStretch(1);
    tabs->addTab(tab5, "Extras & Assinaturas");

    mainLayout->addWidget(tabs);

    // ==========================================
    // BOTTOM BAR: TOTAIS EM TEMPO REAL & BOTÕES
    // ==========================================
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    m_cargaBrutaLabel = new QLabel("Carga Bruta Total: 0 kg", this);
    m_cargaBrutaLabel->setStyleSheet("font-size: 12pt; font-weight: bold; color: #0D47A1;");

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    bottomLayout->addWidget(m_cargaBrutaLabel);
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(buttons);
    mainLayout->addLayout(bottomLayout);

    // Connections to recalculate totals
    connect(m_cargaPesoSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RiggingPlanDataDialog::recalculateTotals);
    connect(m_cargasExtrasSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RiggingPlanDataDialog::recalculateTotals);
    connect(m_lingadaTable, &QTableWidget::cellChanged, this, &RiggingPlanDataDialog::recalculateTotals);
    connect(m_balancimTable, &QTableWidget::cellChanged, this, &RiggingPlanDataDialog::recalculateTotals);
}

void RiggingPlanDataDialog::populateFields(const RiggingPlanData& d)
{
    m_clienteEdit->setText(d.cliente);
    m_localEdit->setText(d.local);
    m_dataEdit->setText(d.data);
    m_planoNumEdit->setText(d.planoNumero);

    m_cargaDescEdit->setText(d.cargaDescricao);
    m_cargaCompSpin->setValue(d.cargaComp);
    m_cargaLarguraSpin->setValue(d.cargaLargura);
    m_cargaAlturaSpin->setValue(d.cargaAltura);
    m_cargaPesoSpin->setValue(d.cargaPeso);

    // Lingada table
    m_lingadaTable->blockSignals(true);
    m_lingadaTable->setRowCount(d.lingadaItems.size());
    for (int r = 0; r < d.lingadaItems.size(); ++r) {
        const auto& it = d.lingadaItems[r];
        m_lingadaTable->setItem(r, 0, new QTableWidgetItem(QString::number(it.item)));
        m_lingadaTable->setItem(r, 1, new QTableWidgetItem(it.descricao));
        m_lingadaTable->setItem(r, 2, new QTableWidgetItem(QString::number(it.qtd)));
        m_lingadaTable->setItem(r, 3, new QTableWidgetItem(QString::number(it.unitario, 'f', 2)));
        auto* totalItem = new QTableWidgetItem(QString::number(it.total(), 'f', 2));
        totalItem->setFlags(totalItem->flags() & ~Qt::ItemIsEditable);
        m_lingadaTable->setItem(r, 4, totalItem);
    }
    m_lingadaTable->blockSignals(false);

    // Balancim table
    m_balancimTable->blockSignals(true);
    m_balancimTable->setRowCount(d.balancimItems.size());
    for (int r = 0; r < d.balancimItems.size(); ++r) {
        const auto& it = d.balancimItems[r];
        m_balancimTable->setItem(r, 0, new QTableWidgetItem(QString::number(it.item)));
        m_balancimTable->setItem(r, 1, new QTableWidgetItem(it.descricao));
        m_balancimTable->setItem(r, 2, new QTableWidgetItem(it.refDesenho));
        m_balancimTable->setItem(r, 3, new QTableWidgetItem(QString::number(it.qtd)));
        m_balancimTable->setItem(r, 4, new QTableWidgetItem(QString::number(it.unitario, 'f', 1)));
        auto* totalItem = new QTableWidgetItem(QString::number(it.total(), 'f', 1));
        totalItem->setFlags(totalItem->flags() & ~Qt::ItemIsEditable);
        m_balancimTable->setItem(r, 5, totalItem);
    }
    m_balancimTable->blockSignals(false);

    m_guindasteModeloEdit->setText(d.guindasteModelo);
    m_guindasteCaminhaoEdit->setText(d.guindasteCaminhao);
    m_guindastePlacasEdit->setText(d.guindastePlacas);
    m_guindasteChassiEdit->setText(d.guindasteChassi);
    m_guindasteAnoEdit->setText(d.guindasteAno);

    m_capNominalSpin->setValue(d.capNominal);
    m_compLancaSpin->setValue(d.compLanca);
    m_raioOperacaoSpin->setValue(d.raioOperacao);
    m_capCargaNoRaioSpin->setValue(d.capCargaNoRaio);
    m_quadranteInicialEdit->setText(d.quadranteInicial);
    m_quadranteFinalEdit->setText(d.quadranteFinal);

    m_cargasExtrasSpin->setValue(d.cargasExtras);
    m_artNumeroEdit->setText(d.artNumero);
    m_observacoesEdit->setText(d.observacoes);
    m_riggerNomeEdit->setText(d.riggerNome);
    m_supervisorNomeEdit->setText(d.supervisorNome);
}

void RiggingPlanDataDialog::onAddLingadaItem()
{
    int row = m_lingadaTable->rowCount();
    m_lingadaTable->blockSignals(true);
    m_lingadaTable->insertRow(row);
    m_lingadaTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
    m_lingadaTable->setItem(row, 1, new QTableWidgetItem("Novo Item de Lingada"));
    m_lingadaTable->setItem(row, 2, new QTableWidgetItem("1"));
    m_lingadaTable->setItem(row, 3, new QTableWidgetItem("0.0"));
    auto* totalItem = new QTableWidgetItem("0.00");
    totalItem->setFlags(totalItem->flags() & ~Qt::ItemIsEditable);
    m_lingadaTable->setItem(row, 4, totalItem);
    m_lingadaTable->blockSignals(false);
    recalculateTotals();
}

void RiggingPlanDataDialog::onRemoveLingadaItem()
{
    int row = m_lingadaTable->currentRow();
    if (row >= 0 && row < m_lingadaTable->rowCount()) {
        m_lingadaTable->removeRow(row);
        recalculateTotals();
    }
}

void RiggingPlanDataDialog::onAddBalancimItem()
{
    int row = m_balancimTable->rowCount();
    m_balancimTable->blockSignals(true);
    m_balancimTable->insertRow(row);
    m_balancimTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
    m_balancimTable->setItem(row, 1, new QTableWidgetItem("Novo Dispositivo"));
    m_balancimTable->setItem(row, 2, new QTableWidgetItem("-"));
    m_balancimTable->setItem(row, 3, new QTableWidgetItem("1"));
    m_balancimTable->setItem(row, 4, new QTableWidgetItem("0.0"));
    auto* totalItem = new QTableWidgetItem("0.00");
    totalItem->setFlags(totalItem->flags() & ~Qt::ItemIsEditable);
    m_balancimTable->setItem(row, 5, totalItem);
    m_balancimTable->blockSignals(false);
    recalculateTotals();
}

void RiggingPlanDataDialog::onRemoveBalancimItem()
{
    int row = m_balancimTable->currentRow();
    if (row >= 0 && row < m_balancimTable->rowCount()) {
        m_balancimTable->removeRow(row);
        recalculateTotals();
    }
}

void RiggingPlanDataDialog::recalculateTotals()
{
    // 1. Lingada: Qtd * Unitario -> Total (Green dots) & Sum -> Yellow dot
    double sumLingada = 0.0;
    m_lingadaTable->blockSignals(true);
    for (int r = 0; r < m_lingadaTable->rowCount(); ++r) {
        double qtd = m_lingadaTable->item(r, 2) ? m_lingadaTable->item(r, 2)->text().replace(',', '.').toDouble() : 0.0;
        double un = m_lingadaTable->item(r, 3) ? m_lingadaTable->item(r, 3)->text().replace(',', '.').toDouble() : 0.0;
        double tot = qtd * un;
        sumLingada += tot;
        if (m_lingadaTable->item(r, 4)) {
            m_lingadaTable->item(r, 4)->setText(QString::number(tot, 'f', 2));
        }
    }
    m_lingadaTable->blockSignals(false);
    m_lingadaTotalLabel->setText(QString("Peso Total da Lingada: %1 kg").arg(QString::number(sumLingada, 'f', 2).replace('.', ',')));

    // 2. Balancim: Qtd * Unitario -> Total & Sum
    double sumBalancim = 0.0;
    m_balancimTable->blockSignals(true);
    for (int r = 0; r < m_balancimTable->rowCount(); ++r) {
        double qtd = m_balancimTable->item(r, 3) ? m_balancimTable->item(r, 3)->text().replace(',', '.').toDouble() : 0.0;
        double un = m_balancimTable->item(r, 4) ? m_balancimTable->item(r, 4)->text().replace(',', '.').toDouble() : 0.0;
        double tot = qtd * un;
        sumBalancim += tot;
        if (m_balancimTable->item(r, 5)) {
            m_balancimTable->item(r, 5)->setText(QString::number(tot, 'f', 1));
        }
    }
    m_balancimTable->blockSignals(false);
    m_balancimTotalLabel->setText(QString("Peso Total Dispositivos: %1 kg").arg(QString::number(sumBalancim, 'f', 1).replace('.', ',')));

    // 3. Carga Bruta Total
    double cargaLiquida = m_cargaPesoSpin->value();
    double extras = m_cargasExtrasSpin->value();
    double cargaBruta = cargaLiquida + sumLingada + sumBalancim + extras;

    m_cargaBrutaLabel->setText(QString("Carga Bruta Total: %1 kg (%2 t)")
                               .arg(QString::number(cargaBruta, 'f', 0))
                               .arg(QString::number(cargaBruta / 1000.0, 'f', 2).replace('.', ',')));
}

RiggingPlanData RiggingPlanDataDialog::planData() const
{
    RiggingPlanData d;
    d.cliente = m_clienteEdit->text();
    d.local = m_localEdit->text();
    d.data = m_dataEdit->text();
    d.planoNumero = m_planoNumEdit->text();

    d.cargaDescricao = m_cargaDescEdit->text();
    d.cargaComp = m_cargaCompSpin->value();
    d.cargaLargura = m_cargaLarguraSpin->value();
    d.cargaAltura = m_cargaAlturaSpin->value();
    d.cargaPeso = m_cargaPesoSpin->value();

    // Lingada items
    d.lingadaItems.clear();
    for (int r = 0; r < m_lingadaTable->rowCount(); ++r) {
        RiggingPlanItem it;
        it.item = m_lingadaTable->item(r, 0) ? m_lingadaTable->item(r, 0)->text().toInt() : (r + 1);
        it.descricao = m_lingadaTable->item(r, 1) ? m_lingadaTable->item(r, 1)->text() : "";
        it.qtd = m_lingadaTable->item(r, 2) ? m_lingadaTable->item(r, 2)->text().replace(',', '.').toDouble() : 1.0;
        it.unitario = m_lingadaTable->item(r, 3) ? m_lingadaTable->item(r, 3)->text().replace(',', '.').toDouble() : 0.0;
        d.lingadaItems.append(it);
    }

    // Balancim items
    d.balancimItems.clear();
    for (int r = 0; r < m_balancimTable->rowCount(); ++r) {
        RiggingPlanItem it;
        it.item = m_balancimTable->item(r, 0) ? m_balancimTable->item(r, 0)->text().toInt() : (r + 1);
        it.descricao = m_balancimTable->item(r, 1) ? m_balancimTable->item(r, 1)->text() : "";
        it.refDesenho = m_balancimTable->item(r, 2) ? m_balancimTable->item(r, 2)->text() : "-";
        it.qtd = m_balancimTable->item(r, 3) ? m_balancimTable->item(r, 3)->text().replace(',', '.').toDouble() : 1.0;
        it.unitario = m_balancimTable->item(r, 4) ? m_balancimTable->item(r, 4)->text().replace(',', '.').toDouble() : 0.0;
        d.balancimItems.append(it);
    }

    d.guindasteModelo = m_guindasteModeloEdit->text();
    d.guindasteCaminhao = m_guindasteCaminhaoEdit->text();
    d.guindastePlacas = m_guindastePlacasEdit->text();
    d.guindasteChassi = m_guindasteChassiEdit->text();
    d.guindasteAno = m_guindasteAnoEdit->text();

    d.capNominal = m_capNominalSpin->value();
    d.compLanca = m_compLancaSpin->value();
    d.raioOperacao = m_raioOperacaoSpin->value();
    d.capCargaNoRaio = m_capCargaNoRaioSpin->value();
    d.quadranteInicial = m_quadranteInicialEdit->text();
    d.quadranteFinal = m_quadranteFinalEdit->text();

    d.cargasExtras = m_cargasExtrasSpin->value();
    d.artNumero = m_artNumeroEdit->text();
    d.observacoes = m_observacoesEdit->text();
    d.riggerNome = m_riggerNomeEdit->text();
    d.supervisorNome = m_supervisorNomeEdit->text();

    return d;
}
