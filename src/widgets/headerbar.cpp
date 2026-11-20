#include "headerbar.h"
#include "utils/constants.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

HeaderBar::HeaderBar(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(Constants::HEADER_HEIGHT);
    setupUi();
}

void HeaderBar::setupUi() {
    setObjectName("headerBar");
    setStyleSheet(
        "QWidget#headerBar {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #9A0007, stop:0.5 #D32F2F, stop:1 #B71C1C);"
        "   border-bottom: 2px solid #E57373;"
        "}"
        "QLabel {"
        "   color: #FFFFFF;"
        "}"
        "QLineEdit {"
        "   background-color: rgba(255, 255, 255, 0.15);"
        "   color: #FFFFFF;"
        "   border: 1px solid rgba(255, 255, 255, 0.4);"
        "   border-radius: 6px;"
        "   padding: 4px 10px;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "QLineEdit:focus {"
        "   background-color: rgba(255, 255, 255, 0.25);"
        "   border: 1px solid #FFFFFF;"
        "}"
        "QComboBox {"
        "   background-color: rgba(0, 0, 0, 0.3);"
        "   color: #FFFFFF;"
        "   border: 1px solid rgba(255, 255, 255, 0.4);"
        "   border-radius: 6px;"
        "   padding: 4px 10px;"
        "   font-weight: bold;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #1E1E1E;"
        "   color: #FFFFFF;"
        "   selection-background-color: #D32F2F;"
        "}"
        "QPushButton#exportBtn {"
        "   background-color: #2E7D32;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 6px 16px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton#exportBtn:hover {"
        "   background-color: #388E3C;"
        "}"
        "QPushButton#exportBtn:pressed {"
        "   background-color: #1B5E20;"
        "}"
        "QPushButton#navBtn {"
        "   background-color: rgba(255, 255, 255, 0.15);"
        "   color: white;"
        "   border: 1px solid rgba(255, 255, 255, 0.3);"
        "   border-radius: 6px;"
        "   padding: 6px 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#navBtn:hover {"
        "   background-color: rgba(255, 255, 255, 0.25);"
        "}"
    );

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(16, 8, 16, 8);
    mainLayout->setSpacing(16);

    // Left side: Branding
    QHBoxLayout* brandLayout = new QHBoxLayout();
    brandLayout->setSpacing(10);

    QLabel* logoLabel = new QLabel(this);
    logoLabel->setText("🏗️");
    logoLabel->setStyleSheet("font-size: 24px;");

    QVBoxLayout* titleBox = new QVBoxLayout();
    titleBox->setSpacing(1);

    QLabel* titleLabel = new QLabel("RIGPLAN INTEGRATOR", this);
    titleLabel->setStyleSheet("font-weight: 900; font-size: 15px; letter-spacing: 1px; color: #FFFFFF;");

    QLabel* subTitleLabel = new QLabel("Sistemas Integrados de Rigging", this);
    subTitleLabel->setStyleSheet("font-size: 10px; color: #FFCDD2; font-weight: 500;");

    titleBox->addWidget(titleLabel);
    titleBox->addWidget(subTitleLabel);

    brandLayout->addWidget(logoLabel);
    brandLayout->addLayout(titleBox);

    // Center: File renaming area (Parte Vermelha Destaque)
    QHBoxLayout* fileLayout = new QHBoxLayout();
    fileLayout->setSpacing(8);

    QLabel* fileIcon = new QLabel("📄 Documento:", this);
    fileIcon->setStyleSheet("font-weight: bold; color: #FFEBEB; font-size: 12px;");

    m_fileNameEdit = new QLineEdit(this);
    m_fileNameEdit->setText("Plano_de_Rigging_001");
    m_fileNameEdit->setToolTip("Clique para alterar o nome do arquivo a ser exportado");
    m_fileNameEdit->setMinimumWidth(220);

    QLabel* formatIcon = new QLabel("Formato:", this);
    formatIcon->setStyleSheet("font-weight: bold; color: #FFEBEB; font-size: 12px;");

    m_formatCombo = new QComboBox(this);
    m_formatCombo->addItem("DOCX (.docx)", "DOCX");
    m_formatCombo->addItem("PDF (.pdf)", "PDF");
    m_formatCombo->setToolTip("Selecione o formato de exportação final");

    fileLayout->addWidget(fileIcon);
    fileLayout->addWidget(m_fileNameEdit);
    fileLayout->addWidget(formatIcon);
    fileLayout->addWidget(m_formatCombo);

    // Right side: Preset navigation & Export button
    QHBoxLayout* rightLayout = new QHBoxLayout();
    rightLayout->setSpacing(10);

    m_presetsButton = new QPushButton("🎨 Formatador de Estilos", this);
    m_presetsButton->setObjectName("navBtn");
    m_presetsButton->setToolTip("Personalizar e selecionar a aparência do documento (Estilos salvos)");

    m_aiButton = new QPushButton("🔍 Verificador", this);
    m_aiButton->setObjectName("navBtn");
    m_aiButton->setToolTip("Central de Verificação e Diagnóstico de Rigging (Verificador de Regras)");

    m_exportButton = new QPushButton("💾 EXPORTAR", this);
    m_exportButton->setObjectName("exportBtn");
    m_exportButton->setToolTip("Gerar e salvar o arquivo no formato selecionado");

    rightLayout->addWidget(m_presetsButton);
    rightLayout->addWidget(m_aiButton);
    rightLayout->addWidget(m_exportButton);

    // Assemble main bar layout
    mainLayout->addLayout(brandLayout);
    mainLayout->addStretch(1);
    mainLayout->addLayout(fileLayout);
    mainLayout->addStretch(1);
    mainLayout->addLayout(rightLayout);

    // Connections
    connect(m_fileNameEdit, &QLineEdit::textChanged, this, &HeaderBar::documentNameChanged);
    connect(m_formatCombo, &QComboBox::currentTextChanged, [this](const QString&) {
        emit exportFormatChanged(exportFormat());
    });
    connect(m_exportButton, &QPushButton::clicked, this, &HeaderBar::exportRequested);
    connect(m_presetsButton, &QPushButton::clicked, this, &HeaderBar::togglePresetGalleryRequested);
    connect(m_aiButton, &QPushButton::clicked, this, &HeaderBar::toggleAiAssistantRequested);
}

QString HeaderBar::documentName() const {
    return m_fileNameEdit->text().trimmed();
}

void HeaderBar::setDocumentName(const QString& name) {
    m_fileNameEdit->setText(name);
}

QString HeaderBar::exportFormat() const {
    return m_formatCombo->currentData().toString();
}

void HeaderBar::setExportFormat(const QString& format) {
    int idx = m_formatCombo->findData(format);
    if (idx != -1) {
        m_formatCombo->setCurrentIndex(idx);
    }
}
