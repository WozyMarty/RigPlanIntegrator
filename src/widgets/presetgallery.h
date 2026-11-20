#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QPushButton>
#include <QTabWidget>
#include <QGridLayout>
#include "models/presetmanager.h"
#include "presetcard.h"

/**
 * @brief Page/Gallery of document format presets.
 *
 * Allows users to choose, favorite, create, edit, duplicate, and delete format profiles.
 */
class PresetGallery : public QWidget {
    Q_OBJECT

public:
    explicit PresetGallery(QWidget* parent = nullptr);
    explicit PresetGallery(PresetManager* manager, QWidget* parent = nullptr);

    void setManager(PresetManager* manager);
    void refreshGallery();


signals:
    void backToMainRequested();
    void editPresetRequested(const FormatPreset& preset);
    void createNewPresetRequested();

private slots:
    void onPresetSelected(const QString& presetId);
    void onFavoriteToggled(const QString& presetId);
    void onDuplicatePreset(const QString& presetId);
    void onDeletePreset(const QString& presetId);

private:
    void setupUi();
    void populateGrid(const QVector<FormatPreset>& presets);

    PresetManager* m_manager;
    QTabWidget* m_tabWidget;

    QWidget* m_allContainer;
    QGridLayout* m_allGrid;

    QWidget* m_favContainer;
    QGridLayout* m_favGrid;

    QPushButton* m_newBtn;
    QPushButton* m_backBtn;
};
