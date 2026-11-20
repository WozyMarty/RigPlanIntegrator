#include "pageorderdialog.h"
#include <QHeaderView>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>

PageOrderDialog::PageOrderDialog(const QVector<DocumentSection>& sections,
                                 PresetManager* presetManager,
                                 QWidget* parent)
    : QDialog(parent)
    , m_sections(sections)
    , m_presetManager(presetManager)
{
    setWindowTitle("📑 Organizar Páginas e Seções Contínuas");
    resize(720, 480);
    setupUi();
    updateTable();
}

void PageOrderDialog::setupUi()
{
    setStyleSheet(
        "QDialog {"
        "   background-color: #0E131F;"
        "   color: #FFFFFF;"
        "}"
        "QLabel {"
        "   color: #C9D1D9;"
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
        "   border-radius: 6px;"
        "   padding: 6px 14px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #30363D;"
        "}"
        "QPushButton#primaryBtn {"
        "   background-color: #238636;"
        "   color: #FFFFFF;"
        "   border: 1px solid #2EA043;"
        "}"
        "QPushButton#primaryBtn:hover {"
        "   background-color: #2EA043;"
        "}"
        "QPushButton#removeBtn:hover {"
        "   background-color: #DA3633;"
        "   color: #FFFFFF;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(12);

    QLabel* titleLabel = new QLabel("📑 Estrutura e Sequência das Páginas (Pré-Visualização Contínua)", this);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #FFFFFF;");
    mainLayout->addWidget(titleLabel);

    QLabel* descLabel = new QLabel(
        "Defina a ordem em que as páginas, estilos adicionais e anexos (CREA e SOLIDWORKS) são exibidos e exportados.", this);
    descLabel->setStyleSheet("color: #8B949E; font-size: 11px;");
    mainLayout->addWidget(descLabel);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"Ativo", "Ordem / Tipo", "Título da Página / Estilo", "Status / Detalhes"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->verticalHeader()->setVisible(false);
    mainLayout->addWidget(m_table, 1);

    // Side Actions & Reordering Buttons
    QHBoxLayout* actionsLayout = new QHBoxLayout();
    actionsLayout->setSpacing(8);

    m_upBtn = new QPushButton("▲ Mover para Cima", this);
    m_downBtn = new QPushButton("▼ Mover para Baixo", this);
    m_addStyleBtn = new QPushButton("➕ Incluir Novo Estilo...", this);
    m_importCreaBtn = new QPushButton("📷 Importar CREA", this);
    m_importSolidworksBtn = new QPushButton("📐 Importar SOLIDWORKS", this);
    m_removeBtn = new QPushButton("🗑️ Excluir", this);
    m_removeBtn->setObjectName("removeBtn");

    actionsLayout->addWidget(m_upBtn);
    actionsLayout->addWidget(m_downBtn);
    actionsLayout->addSpacing(12);
    actionsLayout->addWidget(m_addStyleBtn);
    actionsLayout->addWidget(m_importCreaBtn);
    actionsLayout->addWidget(m_importSolidworksBtn);
    actionsLayout->addSpacing(12);
    actionsLayout->addWidget(m_removeBtn);
    actionsLayout->addStretch(1);

    mainLayout->addLayout(actionsLayout);

    // Bottom Bar
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setStyleSheet("color: #58A6FF; font-weight: bold; font-size: 12px;");

    QPushButton* okBtn = new QPushButton("✓ Concluir & Aplicar Ordem", this);
    okBtn->setObjectName("primaryBtn");
    QPushButton* cancelBtn = new QPushButton("Cancelar", this);

    bottomLayout->addWidget(m_summaryLabel);
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(cancelBtn);
    bottomLayout->addWidget(okBtn);

    mainLayout->addLayout(bottomLayout);

    // Connections
    connect(m_upBtn, &QPushButton::clicked, this, &PageOrderDialog::moveUp);
    connect(m_downBtn, &QPushButton::clicked, this, &PageOrderDialog::moveDown);
    connect(m_addStyleBtn, &QPushButton::clicked, this, &PageOrderDialog::addStylePreset);
    connect(m_importCreaBtn, &QPushButton::clicked, this, &PageOrderDialog::onImportCreaClicked);
    connect(m_importSolidworksBtn, &QPushButton::clicked, this, &PageOrderDialog::onImportSolidworksClicked);
    connect(m_removeBtn, &QPushButton::clicked, this, &PageOrderDialog::removeSelected);

    connect(okBtn, &QPushButton::clicked, this, [this]() {
        emit sectionsChanged(m_sections);
        accept();
    });
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void PageOrderDialog::updateTable()
{
    m_table->setRowCount(m_sections.size());

    int activeCount = 0;
    for (int i = 0; i < m_sections.size(); ++i) {
        const auto& sec = m_sections[i];
        if (sec.isEnabled()) activeCount++;

        // 1. Checkbox Ativo
        QCheckBox* cb = new QCheckBox();
        cb->setChecked(sec.isEnabled());
        cb->setStyleSheet("margin-left: 10px;");
        connect(cb, &QCheckBox::toggled, this, [this, i](bool checked) {
            m_sections[i].setEnabled(checked);
            updateTable();
        });
        m_table->setCellWidget(i, 0, cb);

        // 2. Ordem & Tipo
        QString typeStr = QString("%1. %2 %3").arg(i + 1).arg(sec.typeIcon(), sec.typeName());
        auto* itemType = new QTableWidgetItem(typeStr);
        itemType->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_table->setItem(i, 1, itemType);

        // 3. Título
        auto* itemTitle = new QTableWidgetItem(sec.title());
        itemTitle->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_table->setItem(i, 2, itemTitle);

        // 4. Status / Imagem
        QString statusStr;
        if (sec.isPreset()) {
            statusStr = QString("Layout: %1").arg(sec.preset().isPlanoRiggingLayout() ? "Plano de Rigging" : "Memorial");
        } else {
            statusStr = sec.hasImage() ? "✓ Imagem Anexada" : "⚠️ Sem Imagem";
        }
        auto* itemStatus = new QTableWidgetItem(statusStr);
        itemStatus->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_table->setItem(i, 3, itemStatus);
    }

    m_summaryLabel->setText(QString("Total de Páginas Ativas: %1 de %2").arg(activeCount).arg(m_sections.size()));
}

void PageOrderDialog::moveUp()
{
    int row = m_table->currentRow();
    if (row <= 0 || row >= m_sections.size()) return;

    std::swap(m_sections[row], m_sections[row - 1]);
    updateTable();
    m_table->selectRow(row - 1);
}

void PageOrderDialog::moveDown()
{
    int row = m_table->currentRow();
    if (row < 0 || row >= m_sections.size() - 1) return;

    std::swap(m_sections[row], m_sections[row + 1]);
    updateTable();
    m_table->selectRow(row + 1);
}

void PageOrderDialog::addStylePreset()
{
    if (!m_presetManager) return;

    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background-color: #1F2430; color: #FFFFFF; border: 1px solid #30363D; padding: 4px; }"
        "QMenu::item:selected { background-color: #1F6FEB; }"
    );

    for (const auto& p : m_presetManager->allPresets()) {
        QAction* act = menu.addAction(QString("➕ %1").arg(p.name()));
        connect(act, &QAction::triggered, this, [this, p]() {
            DocumentSection sec(QString(), DocumentSection::PresetStyle, p.name());
            sec.setPreset(p);
            m_sections.append(sec);
            updateTable();
            m_table->selectRow(m_sections.size() - 1);
        });
    }

    menu.exec(m_addStyleBtn->mapToGlobal(QPoint(0, m_addStyleBtn->height())));
}

void PageOrderDialog::removeSelected()
{
    int row = m_table->currentRow();
    if (row < 0 || row >= m_sections.size()) return;

    if (m_sections.size() <= 1) {
        QMessageBox::warning(this, "Aviso", "O documento precisa ter pelo menos uma página ou estilo ativo.");
        return;
    }

    m_sections.removeAt(row);
    updateTable();
    if (row < m_sections.size()) m_table->selectRow(row);
    else if (!m_sections.isEmpty()) m_table->selectRow(m_sections.size() - 1);
}

void PageOrderDialog::onImportCreaClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Importar Imagem CREA / ART", "",
        "Imagens (*.png *.jpg *.jpeg *.bmp *.webp);;Todos os Arquivos (*.*)"
    );
    if (filePath.isEmpty()) return;

    // Check if CREA already exists
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
        DocumentSection sec(QString(), DocumentSection::CreaImage, "Anexo CREA / ART");
        sec.setImagePath(filePath);
        m_sections.append(sec);
    }

    updateTable();
}

void PageOrderDialog::onImportSolidworksClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Importar Desenho Técnico SOLIDWORKS", "",
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
        m_sections.append(sec);
    }

    updateTable();
}
