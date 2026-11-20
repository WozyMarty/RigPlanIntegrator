#include "calculationpresetdialog.h"
#include <QMessageBox>
#include <QGroupBox>

CalculationPresetDialog::CalculationPresetDialog(CalculationEngine* engine, QWidget* parent)
    : QDialog(parent)
    , m_engine(engine)
{
    setWindowTitle("Selecionar Cálculos & Presets de Operação");
    resize(620, 600);
    setupUi();

    populatePresetsCombo();
    populateChecklist();
}

void CalculationPresetDialog::setupUi()
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
        "QComboBox, QLineEdit {"
        "   background-color: #161B22;"
        "   color: #FFFFFF;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 6px 10px;"
        "   font-size: 12px;"
        "}"
        "QListWidget {"
        "   background-color: #161B22;"
        "   color: #E6EDF3;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 4px;"
        "}"
        "QListWidget::item {"
        "   padding: 6px 8px;"
        "   border-radius: 4px;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #21262D;"
        "}"
        "QPushButton#actionBtn {"
        "   background-color: #21262D;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 5px 12px;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#actionBtn:hover {"
        "   background-color: #30363D;"
        "   color: #FFFFFF;"
        "}"
        "QPushButton#applyBtn {"
        "   background-color: #238636;"
        "   color: #FFFFFF;"
        "   border-radius: 6px;"
        "   padding: 8px 18px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton#applyBtn:hover {"
        "   background-color: #2EA043;"
        "}"
    );

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(18, 18, 18, 18);
    rootLayout->setSpacing(12);

    // --- Top Section: Preset Selector ---
    QGroupBox* presetGroup = new QGroupBox("Perfil de Cálculos (Preset)", this);
    presetGroup->setStyleSheet(
        "QGroupBox {"
        "   background-color: #161B22;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   margin-top: 10px;"
        "   padding-top: 12px;"
        "   font-weight: bold;"
        "   color: #58A6FF;"
        "}"
        "QGroupBox::title { left: 12px; }"
    );

    QVBoxLayout* presetGroupLayout = new QVBoxLayout(presetGroup);
    presetGroupLayout->setContentsMargins(12, 12, 12, 12);
    presetGroupLayout->setSpacing(8);

    QHBoxLayout* comboRow = new QHBoxLayout();
    QLabel* pLbl = new QLabel("Carregar Preset:", presetGroup);
    pLbl->setFixedWidth(110);
    m_presetsCombo = new QComboBox(presetGroup);

    m_deletePresetBtn = new QPushButton("🗑️ Excluir", presetGroup);
    m_deletePresetBtn->setObjectName("actionBtn");
    m_deletePresetBtn->setToolTip("Excluir o preset selecionado");

    comboRow->addWidget(pLbl);
    comboRow->addWidget(m_presetsCombo, 1);
    comboRow->addWidget(m_deletePresetBtn);
    presetGroupLayout->addLayout(comboRow);

    rootLayout->addWidget(presetGroup);

    // --- Search & Selection Toolbar ---
    QHBoxLayout* searchRow = new QHBoxLayout();
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("🔍 Filtrar cálculos da lista...");
    m_searchEdit->setClearButtonEnabled(true);

    m_selectAllBtn = new QPushButton("Selecionar Todos", this);
    m_selectAllBtn->setObjectName("actionBtn");

    m_deselectAllBtn = new QPushButton("Desmarcar Todos", this);
    m_deselectAllBtn->setObjectName("actionBtn");

    searchRow->addWidget(m_searchEdit, 1);
    searchRow->addWidget(m_selectAllBtn);
    searchRow->addWidget(m_deselectAllBtn);
    rootLayout->addLayout(searchRow);

    // --- Checklist of Calculations ---
    m_calcListWidget = new QListWidget(this);
    rootLayout->addWidget(m_calcListWidget, 1);

    // Count label
    m_countLabel = new QLabel(this);
    m_countLabel->setStyleSheet("color: #8B949E; font-size: 11px;");
    rootLayout->addWidget(m_countLabel);

    // --- Save Selection as New Preset ---
    QHBoxLayout* savePresetRow = new QHBoxLayout();
    QLabel* newPreLbl = new QLabel("Salvar seleção como novo preset:", this);
    newPreLbl->setStyleSheet("color: #C9D1D9; font-size: 12px;");
    m_newPresetNameEdit = new QLineEdit(this);
    m_newPresetNameEdit->setPlaceholderText("Nome do novo preset (ex: Içamento Torre 1)");

    m_savePresetBtn = new QPushButton("💾 Salvar Preset", this);
    m_savePresetBtn->setObjectName("actionBtn");
    m_savePresetBtn->setStyleSheet(
        "background-color: #1F6FEB; color: #FFFFFF; font-weight: bold; border-radius: 6px; padding: 6px 14px;"
    );

    savePresetRow->addWidget(newPreLbl);
    savePresetRow->addWidget(m_newPresetNameEdit, 1);
    savePresetRow->addWidget(m_savePresetBtn);
    rootLayout->addLayout(savePresetRow);

    // --- Dialog Action Buttons ---
    QHBoxLayout* footerRow = new QHBoxLayout();
    footerRow->addStretch(1);

    m_cancelBtn = new QPushButton("Cancelar", this);
    m_cancelBtn->setObjectName("actionBtn");

    m_applyBtn = new QPushButton("✓ Aplicar Seleção", this);
    m_applyBtn->setObjectName("applyBtn");

    footerRow->addWidget(m_cancelBtn);
    footerRow->addWidget(m_applyBtn);
    rootLayout->addLayout(footerRow);

    // Connections
    connect(m_presetsCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CalculationPresetDialog::onPresetComboChanged);
    connect(m_deletePresetBtn, &QPushButton::clicked, this, &CalculationPresetDialog::onDeletePresetClicked);
    connect(m_selectAllBtn, &QPushButton::clicked, this, &CalculationPresetDialog::onSelectAllClicked);
    connect(m_deselectAllBtn, &QPushButton::clicked, this, &CalculationPresetDialog::onDeselectAllClicked);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &CalculationPresetDialog::onFilterChanged);
    connect(m_savePresetBtn, &QPushButton::clicked, this, &CalculationPresetDialog::onSaveNewPresetClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_applyBtn, &QPushButton::clicked, this, &CalculationPresetDialog::onApplyClicked);

    connect(m_calcListWidget, &QListWidget::itemChanged, this, [this](QListWidgetItem*) {
        int selected = selectedCalculationIds().size();
        int total = m_calcListWidget->count();
        m_countLabel->setText(QString("%1 de %2 cálculos selecionados para uso").arg(selected).arg(total));
    });
}

void CalculationPresetDialog::populatePresetsCombo()
{
    m_presetsCombo->blockSignals(true);
    m_presetsCombo->clear();

    m_presetsCombo->addItem("— Seleção Personalizada Atual —", "");

    if (m_engine) {
        const auto& presets = m_engine->calculationPresets();
        for (const auto& p : presets) {
            QString label = p.name();
            if (p.isBuiltin()) {
                label += " [Padrão]";
            }
            m_presetsCombo->addItem(label, p.id());
        }

        // Set to current engine preset if any
        int idx = m_presetsCombo->findData(m_engine->currentPresetId());
        if (idx >= 0) {
            m_presetsCombo->setCurrentIndex(idx);
        }
    }

    m_presetsCombo->blockSignals(false);
}

void CalculationPresetDialog::populateChecklist()
{
    m_calcListWidget->clear();
    if (!m_engine) return;

    const auto& calcs = m_engine->allCalculations();
    for (auto* c : calcs) {
        QString tag = c->isCustom() ? " [Personalizado]" : "";
        QString text = QString("%1. %2 — (%3)%4")
                           .arg(c->id())
                           .arg(c->name())
                           .arg(c->formulaText())
                           .arg(tag);

        QListWidgetItem* item = new QListWidgetItem(text, m_calcListWidget);
        item->setData(Qt::UserRole, c->id());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

        bool isActive = m_engine->isCalculationActive(c->id());
        item->setCheckState(isActive ? Qt::Checked : Qt::Unchecked);

        if (c->isCustom()) {
            item->setForeground(QBrush(QColor("#58A6FF")));
        }
    }

    int selected = selectedCalculationIds().size();
    m_countLabel->setText(QString("%1 de %2 cálculos selecionados para uso").arg(selected).arg(calcs.size()));
}

void CalculationPresetDialog::updateCheckedFromIds(const QVector<int>& ids)
{
    QSet<int> idSet(ids.begin(), ids.end());
    m_calcListWidget->blockSignals(true);

    for (int i = 0; i < m_calcListWidget->count(); ++i) {
        QListWidgetItem* item = m_calcListWidget->item(i);
        int id = item->data(Qt::UserRole).toInt();
        item->setCheckState(idSet.contains(id) ? Qt::Checked : Qt::Unchecked);
    }

    m_calcListWidget->blockSignals(false);

    int selected = selectedCalculationIds().size();
    m_countLabel->setText(QString("%1 de %2 cálculos selecionados para uso").arg(selected).arg(m_calcListWidget->count()));
}

void CalculationPresetDialog::onPresetComboChanged(int index)
{
    QString presetId = m_presetsCombo->itemData(index).toString();
    if (presetId.isEmpty() || !m_engine) return;

    CalculationPreset p = m_engine->presetById(presetId);
    if (!p.id().isEmpty()) {
        updateCheckedFromIds(p.calculationIds());
    }
}

void CalculationPresetDialog::onSelectAllClicked()
{
    m_calcListWidget->blockSignals(true);
    for (int i = 0; i < m_calcListWidget->count(); ++i) {
        m_calcListWidget->item(i)->setCheckState(Qt::Checked);
    }
    m_calcListWidget->blockSignals(false);

    int selected = selectedCalculationIds().size();
    m_countLabel->setText(QString("%1 de %2 cálculos selecionados para uso").arg(selected).arg(m_calcListWidget->count()));
}

void CalculationPresetDialog::onDeselectAllClicked()
{
    m_calcListWidget->blockSignals(true);
    for (int i = 0; i < m_calcListWidget->count(); ++i) {
        m_calcListWidget->item(i)->setCheckState(Qt::Unchecked);
    }
    m_calcListWidget->blockSignals(false);

    int selected = selectedCalculationIds().size();
    m_countLabel->setText(QString("%1 de %2 cálculos selecionados para uso").arg(selected).arg(m_calcListWidget->count()));
}

void CalculationPresetDialog::onFilterChanged(const QString& query)
{
    QString q = query.trimmed().toLower();
    for (int i = 0; i < m_calcListWidget->count(); ++i) {
        QListWidgetItem* item = m_calcListWidget->item(i);
        bool match = q.isEmpty() || item->text().toLower().contains(q);
        item->setHidden(!match);
    }
}

void CalculationPresetDialog::onSaveNewPresetClicked()
{
    QString name = m_newPresetNameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Salvar Preset", "Informe um nome para o novo preset.");
        m_newPresetNameEdit->setFocus();
        return;
    }

    QVector<int> checkedIds = selectedCalculationIds();
    if (checkedIds.isEmpty()) {
        QMessageBox::warning(this, "Salvar Preset", "Selecione pelo menos um cálculo para compor o preset.");
        return;
    }

    CalculationPreset newPreset(name, checkedIds, "Preset personalizado pelo usuário");
    m_engine->savePreset(newPreset);

    populatePresetsCombo();
    int idx = m_presetsCombo->findData(newPreset.id());
    if (idx >= 0) {
        m_presetsCombo->setCurrentIndex(idx);
    }

    m_newPresetNameEdit->clear();
    QMessageBox::information(this, "Preset Salvo",
                             QString("O preset \"%1\" foi salvo com sucesso com %2 cálculos!")
                                 .arg(name).arg(checkedIds.size()));
}

void CalculationPresetDialog::onDeletePresetClicked()
{
    int idx = m_presetsCombo->currentIndex();
    QString presetId = m_presetsCombo->itemData(idx).toString();
    if (presetId.isEmpty()) {
        QMessageBox::information(this, "Excluir Preset", "Nenhum preset selecionado para exclusão.");
        return;
    }

    CalculationPreset p = m_engine->presetById(presetId);
    if (p.isBuiltin()) {
        QMessageBox::warning(this, "Excluir Preset", "Os presets padrão do sistema não podem ser excluídos.");
        return;
    }

    auto reply = QMessageBox::question(
        this, "Excluir Preset",
        QString("Deseja realmente excluir o preset \"%1\"?").arg(p.name()),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        m_engine->deletePreset(presetId);
        populatePresetsCombo();
    }
}

void CalculationPresetDialog::onApplyClicked()
{
    QVector<int> ids = selectedCalculationIds();
    if (ids.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Selecione pelo menos um cálculo para ficar ativo.");
        return;
    }

    if (m_engine) {
        m_engine->setActiveCalculations(ids);
    }

    accept();
}

QVector<int> CalculationPresetDialog::selectedCalculationIds() const
{
    QVector<int> ids;
    for (int i = 0; i < m_calcListWidget->count(); ++i) {
        QListWidgetItem* item = m_calcListWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            ids.append(item->data(Qt::UserRole).toInt());
        }
    }
    return ids;
}
