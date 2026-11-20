#include "calculationblock.h"
#include <QHBoxLayout>
#include <QDoubleValidator>
#include <QStyle>
#include <QToolTip>
#include <QLocale>

CalculationBlock::CalculationBlock(RiggingCalculation* model, QWidget* parent)
    : QFrame(parent)
    , m_model(model)
{
    setObjectName("calcBlock");
    setupUi();

    if (m_model) {
        connect(m_model, &RiggingCalculation::resultChanged, this, &CalculationBlock::onModelResultChanged);
        connect(m_model, &RiggingCalculation::statusChanged, this, &CalculationBlock::onModelStatusChanged);
        connect(m_model, &RiggingCalculation::descriptionChanged, this, [this]() {
            if (m_descEdit && !m_descEdit->hasFocus() && m_model) {
                m_updatingFromModel = true;
                m_descEdit->setPlainText(m_model->description());
                m_updatingFromModel = false;
            }
        });
        updateValuesFromModel();
    }
}

void CalculationBlock::setupUi() {
    setStyleSheet(
        "QFrame#calcBlock {"
        "   background-color: #1A1F2C;"
        "   border: 1px solid #2E384D;"
        "   border-radius: 8px;"
        "}"
        "QFrame#calcBlock:hover {"
        "   border: 1px solid #3A4B6E;"
        "}"
        "QLabel#blockTitle {"
        "   color: #FFFFFF;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QLabel#formulaText {"
        "   color: #00E5FF;"
        "   font-family: 'Consolas', 'Courier New', monospace;"
        "   font-size: 11px;"
        "   font-weight: 600;"
        "}"
        "QLabel#statusBadge {"
        "   border-radius: 4px;"
        "   padding: 2px 6px;"
        "   font-size: 10px;"
        "   font-weight: bold;"
        "}"
        "QLineEdit {"
        "   background-color: #0F131D;"
        "   color: #E0E0E0;"
        "   border: 1px solid #3A4560;"
        "   border-radius: 4px;"
        "   padding: 4px 8px;"
        "   font-size: 12px;"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid #1F6FEB;"
        "   background-color: #141A29;"
        "}"
        "QPlainTextEdit#descEdit {"
        "   background-color: #0F131D;"
        "   color: #E0E0E0;"
        "   border: 1px solid #3A4560;"
        "   border-radius: 4px;"
        "   padding: 4px 6px;"
        "   font-size: 11px;"
        "}"
        "QPlainTextEdit#descEdit:focus {"
        "   border: 1px solid #1F6FEB;"
        "   background-color: #141A29;"
        "}"
        "QLabel#paramLabel {"
        "   color: #90A4AE;"
        "   font-size: 11px;"
        "}"
        "QLabel#paramUnit {"
        "   color: #78909C;"
        "   font-size: 11px;"
        "}"
        "QLabel#resultText {"
        "   color: #3FB950;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton#collapseBtn {"
        "   background: transparent;"
        "   color: #90A4AE;"
        "   border: none;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "}"
        "QPushButton#collapseBtn:hover {"
        "   color: #FFFFFF;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 8, 10, 8);
    mainLayout->setSpacing(8);

    // --- Header ---
    m_headerWidget = new QWidget(this);
    QHBoxLayout* headerLayout = new QHBoxLayout(m_headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);

    m_titleLabel = new QLabel(m_model ? m_model->name() : "Bloco de Cálculo", this);
    m_titleLabel->setObjectName("blockTitle");

    m_statusBadge = new QLabel("Pendente", this);
    m_statusBadge->setObjectName("statusBadge");

    m_collapseBtn = new QPushButton("▼", this);
    m_collapseBtn->setObjectName("collapseBtn");
    m_collapseBtn->setFixedWidth(20);

    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch(1);
    headerLayout->addWidget(m_statusBadge);
    headerLayout->addWidget(m_collapseBtn);

    mainLayout->addWidget(m_headerWidget);

    // --- Formula display line ---
    if (m_model && !m_model->formulaText().isEmpty()) {
        m_formulaLabel = new QLabel(m_model->formulaText(), this);
        m_formulaLabel->setObjectName("formulaText");
        mainLayout->addWidget(m_formulaLabel);
    }

    // --- Content Widget (Parameters + Result) ---
    m_contentWidget = new QWidget(this);
    QVBoxLayout* contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(0, 4, 0, 0);
    contentLayout->setSpacing(6);

    m_paramsLayout = new QVBoxLayout();
    m_paramsLayout->setSpacing(4);

    if (m_model) {
        const auto& params = m_model->parameters();
        for (int i = 0; i < params.size(); ++i) {
            const auto& p = params[i];

            QHBoxLayout* pRow = new QHBoxLayout();
            pRow->setContentsMargins(0, 0, 0, 0);
            pRow->setSpacing(6);

            QLabel* pLabel = new QLabel(QString("%1 =").arg(p.name), this);
            pLabel->setObjectName("paramLabel");
            pLabel->setMinimumWidth(80);

            QLineEdit* pInput = new QLineEdit(this);
            pInput->setPlaceholderText(p.tooltip.isEmpty() ? "Insira valor..." : p.tooltip);
            pInput->setToolTip(QString("%1 (%2)").arg(p.name, p.unit));

            QDoubleValidator* val = new QDoubleValidator(p.minValue, p.maxValue, 4, pInput);
            val->setNotation(QDoubleValidator::StandardNotation);
            pInput->setValidator(val);

            QLabel* pUnit = new QLabel(p.unit, this);
            pUnit->setObjectName("paramUnit");
            pUnit->setFixedWidth(40);

            pRow->addWidget(pLabel);
            pRow->addWidget(pInput, 1);
            pRow->addWidget(pUnit);

            m_paramsLayout->addLayout(pRow);
            m_paramInputs.append(pInput);

            int pIdx = i;
            connect(pInput, &QLineEdit::textChanged, [this, pIdx](const QString& text) {
                onParameterEdited(pIdx, text);
            });
        }
    }

    contentLayout->addLayout(m_paramsLayout);

    // --- Description Field ---
    QVBoxLayout* descLayout = new QVBoxLayout();
    descLayout->setSpacing(2);
    descLayout->setContentsMargins(0, 4, 0, 0);

    QLabel* descTitle = new QLabel("📝 Descrição / Observações:", this);
    descTitle->setStyleSheet("color: #90A4AE; font-size: 11px; font-weight: bold;");

    m_descEdit = new QPlainTextEdit(this);
    m_descEdit->setObjectName("descEdit");
    m_descEdit->setPlaceholderText("Ex: Considerando, no máximo, 85% de taxa de utilização (TU)...");
    m_descEdit->setMaximumHeight(54);
    if (m_model) {
        m_descEdit->setPlainText(m_model->description());
    }

    connect(m_descEdit, &QPlainTextEdit::textChanged, this, [this]() {
        if (m_model && !m_updatingFromModel) {
            m_model->setDescription(m_descEdit->toPlainText());
        }
    });

    descLayout->addWidget(descTitle);
    descLayout->addWidget(m_descEdit);
    contentLayout->addLayout(descLayout);

    // --- Result Row ---
    QHBoxLayout* resultRow = new QHBoxLayout();
    resultRow->setContentsMargins(0, 4, 0, 0);

    QLabel* rTitle = new QLabel("Resultado:", this);
    rTitle->setStyleSheet("color: #B0BEC5; font-size: 11px; font-weight: bold;");

    m_resultLabel = new QLabel("---", this);
    m_resultLabel->setObjectName("resultText");

    resultRow->addWidget(rTitle);
    resultRow->addWidget(m_resultLabel);
    resultRow->addStretch(1);

    contentLayout->addLayout(resultRow);
    mainLayout->addWidget(m_contentWidget);

    connect(m_collapseBtn, &QPushButton::clicked, this, &CalculationBlock::toggleCollapse);
    updateStatusIndicator();
}

void CalculationBlock::onParameterEdited(int index, const QString& text) {
    if (!m_model) return;

    bool ok = false;
    double val = text.toDouble(&ok);
    if (!ok) {
        val = QLocale().toDouble(text, &ok);
    }

    if (ok) {
        emit valueChanged(m_model->id(), index, val);
    } else if (text.isEmpty()) {
        emit valueChanged(m_model->id(), index, 0.0);
    }
}

void CalculationBlock::onModelResultChanged() {
    updateValuesFromModel();
}

void CalculationBlock::onModelStatusChanged(RiggingCalculation::Status status) {
    Q_UNUSED(status);
    updateStatusIndicator();
}

void CalculationBlock::updateValuesFromModel() {
    if (!m_model) return;

    // Update parameter input texts if not currently focused to prevent cursor jumps
    const auto& params = m_model->parameters();
    for (int i = 0; i < params.size() && i < m_paramInputs.size(); ++i) {
        if (!m_paramInputs[i]->hasFocus()) {
            if (params[i].value != 0.0) {
                m_paramInputs[i]->setText(QString::number(params[i].value, 'g', 6));
            } else {
                m_paramInputs[i]->clear();
            }
        }
    }

    // Update Description text if not focused
    if (m_descEdit && !m_descEdit->hasFocus()) {
        m_updatingFromModel = true;
        m_descEdit->setPlainText(m_model->description());
        m_updatingFromModel = false;
    }

    // Update Result text
    if (m_model->isValid()) {
        m_resultLabel->setText(QString("%1 %2")
                                   .arg(m_model->result(), 0, 'f', 2)
                                   .arg(m_model->resultUnit()));
        m_resultLabel->setStyleSheet("color: #3FB950; font-weight: bold; font-size: 13px;");
    } else {
        m_resultLabel->setText("Pendente...");
        m_resultLabel->setStyleSheet("color: #78909C; font-style: italic; font-size: 12px;");
    }

    updateStatusIndicator();
}

void CalculationBlock::updateStatusIndicator() {
    if (!m_model) return;

    switch (m_model->status()) {
    case RiggingCalculation::Computed:
        m_statusBadge->setText("OK");
        m_statusBadge->setStyleSheet("background-color: #1F3A2B; color: #3FB950; border: 1px solid #2EA043;");
        break;
    case RiggingCalculation::Incomplete:
        m_statusBadge->setText("Parcial");
        m_statusBadge->setStyleSheet("background-color: #382A10; color: #D29922; border: 1px solid #BB8009;");
        break;
    case RiggingCalculation::Error:
        m_statusBadge->setText("Erro");
        m_statusBadge->setStyleSheet("background-color: #3D1C24; color: #F85149; border: 1px solid #DA3633;");
        break;
    case RiggingCalculation::Empty:
    default:
        m_statusBadge->setText("Vazio");
        m_statusBadge->setStyleSheet("background-color: #21262D; color: #8B949E; border: 1px solid #30363D;");
        break;
    }
}

void CalculationBlock::toggleCollapse() {
    setCollapsed(!m_isCollapsed);
    emit toggleCollapseRequested(m_model ? m_model->id() : -1);
}

void CalculationBlock::setCollapsed(bool collapsed) {
    m_isCollapsed = collapsed;
    m_contentWidget->setVisible(!collapsed);
    m_collapseBtn->setText(collapsed ? "▲" : "▼");
}
