#pragma once

#include <QObject>
#include <QVector>
#include <QMap>
#include "formatpreset.h"

/**
 * @brief Manages loading, saving, and organizing format presets.
 *
 * Presets are stored as individual JSON files in a user data directory.
 * Provides CRUD operations, favoriting, and preset selection.
 */
class PresetManager : public QObject {
    Q_OBJECT

public:
    explicit PresetManager(QObject* parent = nullptr);

    // --- Initialization ---
    void initialize();

    // --- CRUD ---
    QVector<FormatPreset> allPresets() const { return m_presets; }
    QVector<FormatPreset> favoritePresets() const;
    FormatPreset presetById(const QString& id) const;
    FormatPreset currentPreset() const;
    int presetCount() const { return m_presets.size(); }

    void addPreset(const FormatPreset& preset);
    void updatePreset(const FormatPreset& preset);
    void removePreset(const QString& id);
    FormatPreset duplicatePreset(const QString& id);

    // --- Selection ---
    void selectPreset(const QString& id);
    QString selectedPresetId() const { return m_selectedId; }

    // --- Favorites ---
    void toggleFavorite(const QString& id);

    // --- Persistence ---
    void saveAll();
    void loadAll();

    // --- Data directory ---
    QString dataDirectory() const;

signals:
    void presetAdded(const FormatPreset& preset);
    void presetUpdated(const FormatPreset& preset);
    void presetRemoved(const QString& id);
    void presetSelected(const FormatPreset& preset);
    void presetsChanged();

private:
    void ensureDefaults();
    void savePreset(const FormatPreset& preset);
    QString presetFilePath(const QString& id) const;

    QVector<FormatPreset> m_presets;
    QString m_selectedId;
};
