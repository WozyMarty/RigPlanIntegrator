#include "presetgallery.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

PresetGallery::PresetGallery(QWidget* parent)
    : QWidget(parent)
    , m_manager(nullptr)
{
    setupUi();
}

PresetGallery::PresetGallery(PresetManager* manager, QWidget* parent)
    : QWidget(parent)
    , m_manager(nullptr)
{
    setupUi();
    setManager(manager);
}

void PresetGallery::setManager(PresetManager* manager) {
    m_manager = manager;
    if (m_manager) {
        connect(m_manager, &PresetManager::presetsChanged, this, &PresetGallery::refreshGallery);
    }
    refreshGallery();
}

void PresetGallery::setupUi() {
    setStyleSheet(
        "QWidget#galleryMain {"
        "   background-color: #0D1117;"
        "}"
        "QLabel#galleryTitle {"
        "   color: #FFFFFF;"
        "   font-weight: 800;"
        "   font-size: 20px;"
        "}"
        "QLabel#gallerySub {"
        "   color: #8B949E;"
        "   font-size: 13px;"
        "}"
        "QPushButton#backBtn {"
        "   background-color: #21262D;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 6px 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#backBtn:hover {"
        "   background-color: #30363D;"
        "   color: #FFFFFF;"
        "}"
        "QPushButton#newBtn {"
        "   background-color: #1F6FEB;"
        "   color: #FFFFFF;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 8px 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#newBtn:hover {"
        "   background-color: #388BFD;"
        "}"
        "QTabWidget::pane {"
        "   border: none;"
        "   background: transparent;"
        "}"
        "QTabBar::tab {"
        "   background-color: #161B22;"
        "   color: #8B949E;"
        "   padding: 8px 20px;"
        "   border-top-left-radius: 6px;"
        "   border-top-right-radius: 6px;"
        "   font-weight: bold;"
        "}"
        "QTabBar::tab:selected {"
        "   background-color: #1F6FEB;"
        "   color: #FFFFFF;"
        "}"
    );

    setObjectName("galleryMain");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);

    // --- Header ---
    QHBoxLayout* headerLayout = new QHBoxLayout();

    QVBoxLayout* titleBox = new QVBoxLayout();
    QLabel* titleLabel = new QLabel("🎨 Galeria de Formatação e Estilos (Skins)", this);
    titleLabel->setObjectName("galleryTitle");

    QLabel* subLabel = new QLabel("Selecione, personalize e favorite aparências para exportação dos documentos DOCX e PDF.", this);
    subLabel->setObjectName("gallerySub");

    titleBox->addWidget(titleLabel);
    titleBox->addWidget(subLabel);

    m_backBtn = new QPushButton("← Voltar ao Editor", this);
    m_backBtn->setObjectName("backBtn");

    m_newBtn = new QPushButton("+ Criar Novo Estilo", this);
    m_newBtn->setObjectName("newBtn");

    headerLayout->addLayout(titleBox);
    headerLayout->addStretch(1);
    headerLayout->addWidget(m_newBtn);
    headerLayout->addWidget(m_backBtn);

    mainLayout->addLayout(headerLayout);

    // --- Tabs: Todos vs Favoritos ---
    m_tabWidget = new QTabWidget(this);

    // Tab 1: Todos
    QScrollArea* allScroll = new QScrollArea(m_tabWidget);
    allScroll->setWidgetResizable(true);
    allScroll->setStyleSheet("background: transparent; border: none;");

    m_allContainer = new QWidget(allScroll);
    m_allGrid = new QGridLayout(m_allContainer);
    m_allGrid->setSpacing(16);
    allScroll->setWidget(m_allContainer);

    m_tabWidget->addTab(allScroll, "Todos os Estilos");

    // Tab 2: Favoritos
    QScrollArea* favScroll = new QScrollArea(m_tabWidget);
    favScroll->setWidgetResizable(true);
    favScroll->setStyleSheet("background: transparent; border: none;");

    m_favContainer = new QWidget(favScroll);
    m_favGrid = new QGridLayout(m_favContainer);
    m_favGrid->setSpacing(16);
    favScroll->setWidget(m_favContainer);

    m_tabWidget->addTab(favScroll, "⭐ Favoritos");

    mainLayout->addWidget(m_tabWidget, 1);

    // Connections
    connect(m_backBtn, &QPushButton::clicked, this, &PresetGallery::backToMainRequested);
    connect(m_newBtn, &QPushButton::clicked, this, &PresetGallery::createNewPresetRequested);

    refreshGallery();
}

void PresetGallery::refreshGallery() {
    if (!m_manager) return;

    // Clear old grids
    QLayoutItem* item;
    while ((item = m_allGrid->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    while ((item = m_favGrid->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    const auto& presets = m_manager->allPresets();
    QString selectedId = m_manager->selectedPresetId();

    int row = 0, col = 0;
    int favRow = 0, favCol = 0;
    constexpr int MAX_COLS = 4;

    for (const auto& preset : presets) {
        bool isSel = (preset.id() == selectedId);

        // All grid
        PresetCard* card = new PresetCard(preset, isSel, m_allContainer);
        connect(card, &PresetCard::selectRequested, this, &PresetGallery::onPresetSelected);
        connect(card, &PresetCard::favoriteToggled, this, &PresetGallery::onFavoriteToggled);
        connect(card, &PresetCard::editRequested, this, &PresetGallery::editPresetRequested);
        connect(card, &PresetCard::duplicateRequested, this, &PresetGallery::onDuplicatePreset);
        connect(card, &PresetCard::deleteRequested, this, &PresetGallery::onDeletePreset);

        m_allGrid->addWidget(card, row, col);
        col++;
        if (col >= MAX_COLS) {
            col = 0;
            row++;
        }

        // Favorites grid
        if (preset.isFavorite()) {
            PresetCard* favCard = new PresetCard(preset, isSel, m_favContainer);
            connect(favCard, &PresetCard::selectRequested, this, &PresetGallery::onPresetSelected);
            connect(favCard, &PresetCard::favoriteToggled, this, &PresetGallery::onFavoriteToggled);
            connect(favCard, &PresetCard::editRequested, this, &PresetGallery::editPresetRequested);
            connect(favCard, &PresetCard::duplicateRequested, this, &PresetGallery::onDuplicatePreset);
            connect(favCard, &PresetCard::deleteRequested, this, &PresetGallery::onDeletePreset);

            m_favGrid->addWidget(favCard, favRow, favCol);
            favCol++;
            if (favCol >= MAX_COLS) {
                favCol = 0;
                favRow++;
            }
        }
    }
}

void PresetGallery::onPresetSelected(const QString& presetId) {
    if (m_manager) {
        m_manager->selectPreset(presetId);
        refreshGallery();
    }
}

void PresetGallery::onFavoriteToggled(const QString& presetId) {
    if (m_manager) {
        m_manager->toggleFavorite(presetId);
        refreshGallery();
    }
}

void PresetGallery::onDuplicatePreset(const QString& presetId) {
    if (m_manager) {
        m_manager->duplicatePreset(presetId);
        refreshGallery();
    }
}

void PresetGallery::onDeletePreset(const QString& presetId) {
    if (!m_manager) return;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Excluir Estilo",
        "Tem certeza que deseja excluir este estilo de formatação?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        m_manager->removePreset(presetId);
        refreshGallery();
    }
}
