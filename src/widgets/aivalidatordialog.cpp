#include "aivalidatordialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QClipboard>
#include <QGuiApplication>
#include <QMessageBox>

AiValidatorDialog::AiValidatorDialog(CalculationEngine* engine,
                                     PresetManager* presetManager,
                                     QWidget* parent)
    : QDialog(parent)
    , m_engine(engine)
    , m_presetManager(presetManager)
{
    setWindowTitle("🔍 Verificador de Conformidade de Cálculos - RigPlan Integrity Guard");
    resize(860, 600);
    setupUi();
    refreshValidation();
}

void AiValidatorDialog::setupUi()
{
    setStyleSheet(
        "QDialog {"
        "   background-color: #0E131F;"
        "   color: #FFFFFF;"
        "}"
        "QLabel {"
        "   color: #C9D1D9;"
        "}"
        "QTabWidget::pane {"
        "   border: 1px solid #2B384E;"
        "   background-color: #121824;"
        "   border-radius: 6px;"
        "}"
        "QTabBar::tab {"
        "   background-color: #161D2B;"
        "   color: #90A4AE;"
        "   padding: 8px 16px;"
        "   border-top-left-radius: 6px;"
        "   border-top-right-radius: 6px;"
        "   font-weight: bold;"
        "   font-size: 11.5px;"
        "   margin-right: 4px;"
        "}"
        "QTabBar::tab:selected {"
        "   background-color: #1F6FEB;"
        "   color: #FFFFFF;"
        "}"
        "QTableWidget {"
        "   background-color: #121824;"
        "   color: #FFFFFF;"
        "   border: none;"
        "   gridline-color: #1F2839;"
        "   selection-background-color: #1F6FEB;"
        "}"
        "QHeaderView::section {"
        "   background-color: #1A2332;"
        "   color: #90A4AE;"
        "   padding: 6px;"
        "   font-weight: bold;"
        "   border: none;"
        "   border-bottom: 1px solid #2B384E;"
        "}"
        "QProgressBar {"
        "   border: 1px solid #2B384E;"
        "   border-radius: 6px;"
        "   text-align: center;"
        "   background-color: #161D2B;"
        "   color: #FFFFFF;"
        "   font-weight: bold;"
        "}"
        "QProgressBar::chunk {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #238636, stop:1 #2EA043);"
        "   border-radius: 5px;"
        "}"
        "QPushButton {"
        "   background-color: #21262D;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 5px;"
        "   padding: 7px 16px;"
        "   font-weight: bold;"
        "   font-size: 11.5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #30363D;"
        "}"
        "QPushButton#syncBtn {"
        "   background-color: #1F6FEB;"
        "   color: #FFFFFF;"
        "   border: 1px solid #388BFD;"
        "}"
        "QPushButton#syncBtn:hover {"
        "   background-color: #388BFD;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(14);

    // Header Card
    QFrame* headerCard = new QFrame(this);
    headerCard->setStyleSheet("background-color: #161D2B; border: 1px solid #2B384E; border-radius: 8px; padding: 12px;");
    QHBoxLayout* headerLayout = new QHBoxLayout(headerCard);

    QLabel* aiIcon = new QLabel("🔍", headerCard);
    aiIcon->setStyleSheet("font-size: 38px;");
    headerLayout->addWidget(aiIcon);

    QVBoxLayout* titleBox = new QVBoxLayout();
    titleBox->setSpacing(3);

    QLabel* titleLbl = new QLabel("Verificador de Integridade e Conformidade de Cálculos", headerCard);
    titleLbl->setStyleSheet("font-size: 15px; font-weight: 900; color: #FFFFFF;");

    QLabel* subLbl = new QLabel(
        "Verificador automatizado baseado em regras determinísticas e normas técnicas (sem uso de IA).\n"
        "Auditoria contínua entre as variáveis das fórmulas do Memorial e os dados inseridos no Plano de Rigging.", headerCard);
    subLbl->setStyleSheet("font-size: 11px; color: #8B949E;");

    titleBox->addWidget(titleLbl);
    titleBox->addWidget(subLbl);
    headerLayout->addLayout(titleBox, 1);

    // Score widget on top right
    QVBoxLayout* scoreBox = new QVBoxLayout();
    scoreBox->setSpacing(4);
    scoreBox->setAlignment(Qt::AlignRight);

    m_statusBadge = new QLabel("100% Protegido & Conforme", headerCard);
    m_statusBadge->setStyleSheet("background-color: #238636; color: #FFFFFF; font-weight: bold; padding: 4px 10px; border-radius: 4px; font-size: 11px;");

    m_scoreBar = new QProgressBar(headerCard);
    m_scoreBar->setRange(0, 100);
    m_scoreBar->setValue(100);
    m_scoreBar->setFixedWidth(160);
    m_scoreBar->setFixedHeight(16);

    scoreBox->addWidget(m_statusBadge, 0, Qt::AlignRight);
    scoreBox->addWidget(m_scoreBar, 0, Qt::AlignRight);
    headerLayout->addLayout(scoreBox);

    mainLayout->addWidget(headerCard);

    // Stats Bar
    m_summaryStats = new QLabel(this);
    m_summaryStats->setStyleSheet("color: #58A6FF; font-weight: bold; font-size: 12px; margin-left: 4px;");
    mainLayout->addWidget(m_summaryStats);

    // Tabs
    m_tabWidget = new QTabWidget(this);

    // Tab 1: Cross-check Matrix
    QWidget* tab1 = new QWidget();
    QVBoxLayout* tab1Layout = new QVBoxLayout(tab1);
    tab1Layout->setContentsMargins(8, 8, 8, 8);

    m_comparisonTable = new QTableWidget(tab1);
    m_comparisonTable->setColumnCount(5);
    m_comparisonTable->setHorizontalHeaderLabels({
        "Variável / Parâmetro", "Valor no Plano de Rigging", "Valor nas Fórmulas do Memorial", "Status", "Diagnóstico do Verificador"
    });
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_comparisonTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_comparisonTable->verticalHeader()->setVisible(false);
    tab1Layout->addWidget(m_comparisonTable);

    m_tabWidget->addTab(tab1, "🔍 Matriz de Conferência Cruzada (Plano ⇄ Fórmulas)");

    // Tab 2: Issues and Diagnosis
    QWidget* tab2 = new QWidget();
    QVBoxLayout* tab2Layout = new QVBoxLayout(tab2);
    tab2Layout->setContentsMargins(8, 8, 8, 8);

    m_issuesTable = new QTableWidget(tab2);
    m_issuesTable->setColumnCount(4);
    m_issuesTable->setHorizontalHeaderLabels({"Severidade", "Categoria", "Inconsistência Identificada", "Ação Recomendada / Proposta"});
    m_issuesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_issuesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_issuesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_issuesTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_issuesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_issuesTable->verticalHeader()->setVisible(false);
    tab2Layout->addWidget(m_issuesTable);

    m_tabWidget->addTab(tab2, "⚠️ Diagnóstico Detalhado & Normas (NR-11 / NR-12)");

    // Tab 3: Text Report
    QWidget* tab3 = new QWidget();
    QVBoxLayout* tab3Layout = new QVBoxLayout(tab3);
    tab3Layout->setContentsMargins(8, 8, 8, 8);

    m_technicalReportEdit = new QTextEdit(tab3);
    m_technicalReportEdit->setReadOnly(true);
    m_technicalReportEdit->setStyleSheet(
        "background-color: #0B0E14; color: #E6EDF3; font-family: monospace; font-size: 11px; border: 1px solid #2B384E; border-radius: 4px; padding: 10px;"
    );
    tab3Layout->addWidget(m_technicalReportEdit);

    m_tabWidget->addTab(tab3, "📜 Parecer Técnico do Verificador");

    mainLayout->addWidget(m_tabWidget, 1);

    // Bottom Action Row
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(10);

    m_syncBtn = new QPushButton("🔄 Sincronizar Variáveis Automaticamente", this);
    m_syncBtn->setObjectName("syncBtn");
    m_syncBtn->setToolTip("Ajusta automaticamente as variáveis divergentes entre o Plano de Rigging e as Fórmulas");

    m_copyBtn = new QPushButton("📋 Copiar Parecer Técnico", this);

    QPushButton* closeBtn = new QPushButton("Fechar", this);

    bottomLayout->addWidget(m_syncBtn);
    bottomLayout->addWidget(m_copyBtn);
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(closeBtn);

    mainLayout->addLayout(bottomLayout);

    // Connections
    connect(m_syncBtn, &QPushButton::clicked, this, &AiValidatorDialog::onAutoSyncClicked);
    connect(m_copyBtn, &QPushButton::clicked, this, &AiValidatorDialog::onCopyReportClicked);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void AiValidatorDialog::refreshValidation()
{
    if (!m_engine || !m_presetManager) return;

    FormatPreset current = m_presetManager->currentPreset();
    RiggingPlanData planData = current.planData();
    QVector<RiggingCalculation*> calcs = m_engine->calculations();

    AiValidationReport report = AiValidator::validate(planData, calcs, current);
    populateReport(report);
}

void AiValidatorDialog::populateReport(const AiValidationReport& report)
{
    // Score Bar & Badge
    m_scoreBar->setValue(report.score);
    m_statusBadge->setText(report.statusTitle);
    m_statusBadge->setStyleSheet(
        QString("background-color: %1; color: #FFFFFF; font-weight: bold; padding: 4px 10px; border-radius: 4px; font-size: 11px;")
            .arg(report.statusBadgeColor)
    );

    m_summaryStats->setText(QString(
        "Índice de Conformidade do Verificador: %1%  |  Críticos: %2  |  Alertas: %3  |  Variáveis Auditadas: %4"
    ).arg(report.score).arg(report.criticalCount).arg(report.warningCount).arg(report.comparisons.size()));

    // 1. Comparison Table
    m_comparisonTable->setRowCount(report.comparisons.size());
    for (int i = 0; i < report.comparisons.size(); ++i) {
        const auto& c = report.comparisons[i];

        auto* itemVar = new QTableWidgetItem(c.variableName);
        itemVar->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_comparisonTable->setItem(i, 0, itemVar);

        auto* itemPlan = new QTableWidgetItem(c.planValue);
        itemPlan->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_comparisonTable->setItem(i, 1, itemPlan);

        auto* itemFormula = new QTableWidgetItem(c.formulaValue);
        itemFormula->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_comparisonTable->setItem(i, 2, itemFormula);

        auto* itemStatus = new QTableWidgetItem(c.isConsistent ? "✓ Conforme" : "⚠️ Inconsistente");
        itemStatus->setForeground(QColor(c.isConsistent ? "#3FB950" : "#F85149"));
        itemStatus->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_comparisonTable->setItem(i, 3, itemStatus);

        auto* itemDiag = new QTableWidgetItem(c.statusMessage);
        itemDiag->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_comparisonTable->setItem(i, 4, itemDiag);
    }

    // 2. Issues Table
    m_issuesTable->setRowCount(report.issues.size());
    for (int i = 0; i < report.issues.size(); ++i) {
        const auto& issue = report.issues[i];

        QString sevStr;
        QColor sevColor;
        if (issue.severity == AiIssueSeverity::Critical) {
            sevStr = "❌ Crítico";
            sevColor = QColor("#F85149");
        } else if (issue.severity == AiIssueSeverity::Warning) {
            sevStr = "⚠️ Atenção";
            sevColor = QColor("#D29922");
        } else {
            sevStr = "ℹ️ Informativo";
            sevColor = QColor("#58A6FF");
        }

        auto* itemSev = new QTableWidgetItem(sevStr);
        itemSev->setForeground(sevColor);
        itemSev->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_issuesTable->setItem(i, 0, itemSev);

        auto* itemCat = new QTableWidgetItem(issue.category);
        itemCat->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_issuesTable->setItem(i, 1, itemCat);

        auto* itemDesc = new QTableWidgetItem(issue.title + "\n" + issue.description);
        itemDesc->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_issuesTable->setItem(i, 2, itemDesc);

        QString actionStr = issue.canAutoSync ? ("🔄 Auto-Sincronizável: " + issue.syncProposal) : "Revisão Manual Necessária";
        auto* itemAction = new QTableWidgetItem(actionStr);
        itemAction->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_issuesTable->setItem(i, 3, itemAction);
    }

    if (report.issues.isEmpty()) {
        m_issuesTable->setRowCount(1);
        auto* itemOk = new QTableWidgetItem("Nenhuma anomalia ou inconsistência encontrada. Todas as variáveis e fórmulas operam em 100% de conformidade técnica.");
        itemOk->setForeground(QColor("#3FB950"));
        m_issuesTable->setItem(0, 2, itemOk);
    }

    // 3. Technical Markdown Report
    QString txt;
    txt += "=======================================================================\n";
    txt += "   RIGPLAN INTEGRATOR — RELATÓRIO TÉCNICO DO VERIFICADOR DE REGRAS\n";
    txt += "   (Verificador algorítmico baseado em regras e normas técnicas — sem IA)\n";
    txt += "=======================================================================\n";
    txt += QString("Status Geral: %1 (Índice de Conformidade: %2%)\n\n").arg(report.statusTitle).arg(report.score);
    txt += "1. MATRIZ DE CONFERÊNCIA CRUZADA (PLANO DE RIGGING vs FÓRMULAS):\n";
    for (const auto& c : report.comparisons) {
        txt += QString(" • %1: Plano [%2] vs Fórmula [%3] -> %4\n")
            .arg(c.variableName, c.planValue, c.formulaValue, c.statusMessage);
    }
    txt += "\n2. DIAGNÓSTICO DE RISCO E NORMAS REGULAMENTADORAS:\n";
    if (report.issues.isEmpty()) {
        txt += " • [OK] Nenhuma não-conformidade identificada. Projeto aprovado.\n";
    } else {
        for (const auto& is : report.issues) {
            QString s = (is.severity == AiIssueSeverity::Critical) ? "[CRÍTICO]" : "[ALERTA]";
            txt += QString(" • %1 %2: %3\n   Detalhes: %4\n").arg(s, is.category, is.title, is.description);
        }
    }
    txt += "\n3. DIRETRIZES DE SEGURANÇA OPERACIONAL (NR-11 / NR-12 / NBR 15637 / ASME B30):\n";
    txt += " • Não permitir inversão de parâmetros ou alteração de valores nominais.\n";
    txt += " • Certificar patolamento do guindaste com dormentes de madeira compatíveis com o esforço ES.\n";
    txt += " • Respeitar a velocidade máxima de vento e o limite operacional do guindaste.\n";
    txt += "=======================================================================\n";

    m_technicalReportEdit->setPlainText(txt);
}

void AiValidatorDialog::onAutoSyncClicked()
{
    if (!m_engine || !m_presetManager) return;

    FormatPreset current = m_presetManager->currentPreset();
    RiggingPlanData planData = current.planData();

    bool changed = AiValidator::autoSyncVariables(planData, m_engine, current);
    if (changed) {
        m_presetManager->updatePreset(current);
        m_hasSynced = true;
        refreshValidation();
        emit dataSynced();

        QMessageBox::information(
            this, "Sincronização Concluída",
            "As variáveis das fórmulas foram sincronizadas com sucesso de acordo com os dados do Plano de Rigging!\n"
            "Todos os cálculos foram recalculados automaticamente."
        );
    } else {
        QMessageBox::information(
            this, "Sincronização",
            "Todas as variáveis já estão perfeitamente sincronizadas entre o Plano de Rigging e as Fórmulas."
        );
    }
}

void AiValidatorDialog::onCopyReportClicked()
{
    QGuiApplication::clipboard()->setText(m_technicalReportEdit->toPlainText());
    QMessageBox::information(this, "Copiado", "O Parecer Técnico do Verificador foi copiado para a área de transferência!");
}
