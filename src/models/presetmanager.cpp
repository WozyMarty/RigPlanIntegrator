#include "presetmanager.h"
#include "../utils/appdatapath.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

PresetManager::PresetManager(QObject* parent)
    : QObject(parent)
{
}

void PresetManager::initialize()
{
    loadAll();
    ensureDefaults();

    // Select Memorial preset by default if available, else first favorite or first preset
    bool foundMemorial = false;
    for (const auto& p : m_presets) {
        if (p.name().contains("Memorial", Qt::CaseInsensitive)) {
            m_selectedId = p.id();
            foundMemorial = true;
            break;
        }
    }

    if (!foundMemorial && m_selectedId.isEmpty() && !m_presets.isEmpty()) {
        auto favs = favoritePresets();
        if (!favs.isEmpty()) {
            m_selectedId = favs.first().id();
        } else {
            m_selectedId = m_presets.first().id();
        }
    }
}

QVector<FormatPreset> PresetManager::favoritePresets() const
{
    QVector<FormatPreset> favs;
    for (const auto& p : m_presets) {
        if (p.isFavorite()) {
            favs.append(p);
        }
    }
    return favs;
}

FormatPreset PresetManager::presetById(const QString& id) const
{
    for (const auto& p : m_presets) {
        if (p.id() == id) return p;
    }
    return FormatPreset::createDefault();
}

FormatPreset PresetManager::currentPreset() const
{
    return presetById(m_selectedId);
}

void PresetManager::addPreset(const FormatPreset& preset)
{
    m_presets.append(preset);
    savePreset(preset);
    emit presetAdded(preset);
    emit presetsChanged();
}

void PresetManager::updatePreset(const FormatPreset& preset)
{
    for (int i = 0; i < m_presets.size(); ++i) {
        if (m_presets[i].id() == preset.id()) {
            m_presets[i] = preset;
            m_presets[i].touch();
            savePreset(m_presets[i]);
            emit presetUpdated(m_presets[i]);
            emit presetsChanged();
            return;
        }
    }
}

void PresetManager::removePreset(const QString& id)
{
    for (int i = 0; i < m_presets.size(); ++i) {
        if (m_presets[i].id() == id) {
            if (m_presets[i].isDefault()) return; // Cannot delete default

            // Remove file
            QFile::remove(presetFilePath(id));
            m_presets.removeAt(i);

            // Reselect if needed
            if (m_selectedId == id && !m_presets.isEmpty()) {
                m_selectedId = m_presets.first().id();
                emit presetSelected(m_presets.first());
            }

            emit presetRemoved(id);
            emit presetsChanged();
            return;
        }
    }
}

FormatPreset PresetManager::duplicatePreset(const QString& id)
{
    FormatPreset original = presetById(id);
    FormatPreset copy;
    // Copy all properties except identity
    copy.setName(original.name() + " (Cópia)");
    copy.setFavorite(false);
    copy.setDefault(false);
    copy.setFontFamily(original.fontFamily());
    copy.setFontSizeBody(original.fontSizeBody());
    copy.setFontSizeTitle(original.fontSizeTitle());
    copy.setFontSizeSubtitle(original.fontSizeSubtitle());
    copy.setColorTitle(original.colorTitle());
    copy.setColorBody(original.colorBody());
    copy.setColorAccent(original.colorAccent());
    copy.setColorTableHeader(original.colorTableHeader());
    copy.setMarginTop(original.marginTop());
    copy.setMarginBottom(original.marginBottom());
    copy.setMarginLeft(original.marginLeft());
    copy.setMarginRight(original.marginRight());
    copy.setHeaderText(original.headerText());
    copy.setFooterText(original.footerText());
    copy.setShowLogo(original.showLogo());
    copy.setLogoPath(original.logoPath());
    copy.setDocumentLayout(original.documentLayout());
    copy.setPlanData(original.planData());
    copy.setTableStyle(original.tableStyle());
    copy.setTableBorders(original.tableBorders());
    copy.setTableAlternatingRows(original.tableAlternatingRows());

    addPreset(copy);
    return copy;
}

void PresetManager::selectPreset(const QString& id)
{
    m_selectedId = id;
    saveAll();
    emit presetSelected(presetById(id));
}

void PresetManager::toggleFavorite(const QString& id)
{
    for (int i = 0; i < m_presets.size(); ++i) {
        if (m_presets[i].id() == id) {
            m_presets[i].setFavorite(!m_presets[i].isFavorite());
            savePreset(m_presets[i]);
            emit presetUpdated(m_presets[i]);
            emit presetsChanged();
            return;
        }
    }
}

void PresetManager::saveAll()
{
    QDir dir(dataDirectory());
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    for (const auto& p : m_presets) {
        savePreset(p);
    }

    // Save selection state
    QJsonObject state;
    state["selected_id"] = m_selectedId;
    QFile stateFile(dataDirectory() + "/state.json");
    if (stateFile.open(QIODevice::WriteOnly)) {
        stateFile.write(QJsonDocument(state).toJson());
        stateFile.close();
    }
}

void PresetManager::loadAll()
{
    m_presets.clear();
    QDir dir(dataDirectory());
    if (!dir.exists()) return;

    // Load all preset files
    QStringList filters;
    filters << "preset_*.json";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);

    for (const auto& fileInfo : files) {
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            file.close();
            if (!doc.isNull() && doc.isObject()) {
                m_presets.append(FormatPreset::fromJson(doc.object()));
            }
        }
    }

    // Load selection state
    QFile stateFile(dataDirectory() + "/state.json");
    if (stateFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(stateFile.readAll());
        stateFile.close();
        if (!doc.isNull() && doc.isObject()) {
            m_selectedId = doc.object()["selected_id"].toString();
        }
    }
}

QString PresetManager::dataDirectory() const
{
    QString appData = applicationDataLocation();
    return appData + "/presets";
}

void PresetManager::ensureDefaults()
{
    bool hasMemorial = false;
    bool hasPlanoRigging = false;
    bool hasPlanoRiggingResultados = false;
    for (const auto& p : m_presets) {
        if (p.name().contains("Memorial", Qt::CaseInsensitive)) {
            hasMemorial = true;
        }
        if (p.name().contains("Plano de Rigging", Qt::CaseInsensitive) || p.isPlanoRiggingLayout()) {
            hasPlanoRigging = true;
        }
        if (p.name().contains("Plano de Rigging", Qt::CaseInsensitive)
            && p.name().contains("Resultados", Qt::CaseInsensitive)) {
            hasPlanoRiggingResultados = true;
        }
    }

    if (!hasMemorial) {
        FormatPreset def = FormatPreset::createDefault();
        m_presets.prepend(def);
        savePreset(def);
        m_selectedId = def.id();

        FormatPreset modern = FormatPreset::createModern();
        m_presets.append(modern);
        savePreset(modern);

        FormatPreset minimal = FormatPreset::createMinimal();
        m_presets.append(minimal);
        savePreset(minimal);
    }

    if (!hasPlanoRigging) {
        FormatPreset pr = FormatPreset::createPlanoRigging();
        m_presets.append(pr);
        savePreset(pr);
    }

    if (!hasPlanoRiggingResultados) {
        FormatPreset pr = FormatPreset::createPlanoRiggingResultados();
        m_presets.append(pr);
        savePreset(pr);
    }
}

void PresetManager::savePreset(const FormatPreset& preset)
{
    QDir dir(dataDirectory());
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile file(presetFilePath(preset.id()));
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(preset.toJson()).toJson(QJsonDocument::Indented));
        file.close();
    }
}

QString PresetManager::presetFilePath(const QString& id) const
{
    return dataDirectory() + "/preset_" + id + ".json";
}
