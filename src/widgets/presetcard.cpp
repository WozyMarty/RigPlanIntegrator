#include "presetcard.h"
#include "utils/constants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

PresetCard::PresetCard(const FormatPreset& preset, bool isSelected, QWidget* parent)
    : QFrame(parent)
    , m_preset(preset)
    , m_isSelected(isSelected)
{
    setFixedSize(Constants::PRESET_CARD_WIDTH, Constants::PRESET_CARD_HEIGHT);
    setupUi();
}

void PresetCard::setupUi() {
    setObjectName("presetCard");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    // --- Header Row: Name & Favorite Star ---
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(4);

    m_nameLabel = new QLabel(m_preset.name(), this);
    m_nameLabel->setStyleSheet("color: #FFFFFF; font-weight: bold; font-size: 13px;");

    m_starBtn = new QPushButton(m_preset.isFavorite() ? "⭐" : "☆", this);
    m_starBtn->setStyleSheet("border: none; background: transparent; font-size: 14px;");
    m_starBtn->setToolTip(m_preset.isFavorite() ? "Remover dos Favoritos" : "Favoritar Estilo");

    headerLayout->addWidget(m_nameLabel, 1);
    headerLayout->addWidget(m_starBtn);

    mainLayout->addLayout(headerLayout);

    // --- Details: Font and Margins ---
    m_fontLabel = new QLabel(QString("Fonte: %1 (%2pt)").arg(m_preset.fontFamily()).arg(m_preset.fontSizeBody()), this);
    m_fontLabel->setStyleSheet("color: #90A4AE; font-size: 11px;");
    mainLayout->addWidget(m_fontLabel);

    // --- Color Swatches Preview ---
    QHBoxLayout* colorLayout = new QHBoxLayout();
    colorLayout->setSpacing(6);

    auto makeSwatch = [this](const QColor& c, const QString& tip) {
        QLabel* sw = new QLabel(this);
        sw->setFixedSize(20, 20);
        sw->setToolTip(tip);
        sw->setStyleSheet(QString("background-color: %1; border-radius: 4px; border: 1px solid rgba(255,255,255,0.2);")
                                .arg(c.name()));
        return sw;
    };

    colorLayout->addWidget(makeSwatch(m_preset.colorTitle(), "Título"));
    colorLayout->addWidget(makeSwatch(m_preset.colorAccent(), "Destaque"));
    colorLayout->addWidget(makeSwatch(m_preset.colorTableHeader(), "Tabela"));
    colorLayout->addWidget(makeSwatch(m_preset.colorBody(), "Texto"));
    colorLayout->addStretch(1);

    mainLayout->addLayout(colorLayout);

    mainLayout->addStretch(1);

    // --- Select Button ---
    m_selectBtn = new QPushButton(m_isSelected ? "✓ EQUIPADO" : "EQUIPAR ESTILO", this);
    m_selectBtn->setObjectName("selectBtn");

    mainLayout->addWidget(m_selectBtn);

    // --- Actions Row: Edit, Duplicate, Delete ---
    QHBoxLayout* actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(4);

    m_editBtn = new QPushButton("✏️ Editar", this);
    m_editBtn->setObjectName("cardAction");

    m_duplicateBtn = new QPushButton("📋 Copiar", this);
    m_duplicateBtn->setObjectName("cardAction");

    m_deleteBtn = new QPushButton("🗑️", this);
    m_deleteBtn->setObjectName("cardAction");
    m_deleteBtn->setEnabled(!m_preset.isDefault());

    actionLayout->addWidget(m_editBtn);
    actionLayout->addWidget(m_duplicateBtn);
    actionLayout->addWidget(m_deleteBtn);

    mainLayout->addLayout(actionLayout);

    updateStyleSheet();

    // Connections
    connect(m_selectBtn, &QPushButton::clicked, [this]() { emit selectRequested(m_preset.id()); });
    connect(m_starBtn, &QPushButton::clicked, [this]() { emit favoriteToggled(m_preset.id()); });
    connect(m_editBtn, &QPushButton::clicked, [this]() { emit editRequested(m_preset); });
    connect(m_duplicateBtn, &QPushButton::clicked, [this]() { emit duplicateRequested(m_preset.id()); });
    connect(m_deleteBtn, &QPushButton::clicked, [this]() { emit deleteRequested(m_preset.id()); });
}

void PresetCard::setSelected(bool selected) {
    m_isSelected = selected;
    m_selectBtn->setText(m_isSelected ? "✓ EQUIPADO" : "EQUIPAR ESTILO");
    updateStyleSheet();
}

void PresetCard::updateStyleSheet() {
    if (m_isSelected) {
        setStyleSheet(
            "QFrame#presetCard {"
            "   background-color: #1E2D4A;"
            "   border: 2px solid #1F6FEB;"
            "   border-radius: 10px;"
            "}"
            "QPushButton#selectBtn {"
            "   background-color: #1F6FEB;"
            "   color: white;"
            "   font-weight: bold;"
            "   border-radius: 6px;"
            "   padding: 6px;"
            "}"
            "QPushButton#cardAction {"
            "   background-color: #161B22;"
            "   color: #C9D1D9;"
            "   border: 1px solid #30363D;"
            "   border-radius: 4px;"
            "   padding: 3px;"
            "   font-size: 10px;"
            "}"
        );
    } else {
        setStyleSheet(
            "QFrame#presetCard {"
            "   background-color: #161B22;"
            "   border: 1px solid #30363D;"
            "   border-radius: 10px;"
            "}"
            "QFrame#presetCard:hover {"
            "   border: 1px solid #58A6FF;"
            "}"
            "QPushButton#selectBtn {"
            "   background-color: #21262D;"
            "   color: #58A6FF;"
            "   border: 1px solid #30363D;"
            "   font-weight: bold;"
            "   border-radius: 6px;"
            "   padding: 6px;"
            "}"
            "QPushButton#selectBtn:hover {"
            "   background-color: #1F6FEB;"
            "   color: white;"
            "}"
            "QPushButton#cardAction {"
            "   background-color: #21262D;"
            "   color: #8B949E;"
            "   border: 1px solid #30363D;"
            "   border-radius: 4px;"
            "   padding: 3px;"
            "   font-size: 10px;"
            "}"
            "QPushButton#cardAction:hover {"
            "   color: white;"
            "}"
        );
    }
}
