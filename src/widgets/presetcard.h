#pragma once

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include "models/formatpreset.h"

/**
 * @brief UI Card representing a FormatPreset (Character skin customizer style)
 *
 * Displays preset title, font, color swatches, favorite star toggle,
 * and select/edit actions.
 */
class PresetCard : public QFrame {
    Q_OBJECT

public:
    explicit PresetCard(const FormatPreset& preset, bool isSelected, QWidget* parent = nullptr);

    FormatPreset preset() const { return m_preset; }
    void setSelected(bool selected);
    bool isSelected() const { return m_isSelected; }

signals:
    void selectRequested(const QString& presetId);
    void favoriteToggled(const QString& presetId);
    void editRequested(const FormatPreset& preset);
    void duplicateRequested(const QString& presetId);
    void deleteRequested(const QString& presetId);

private:
    void setupUi();
    void updateStyleSheet();

    FormatPreset m_preset;
    bool m_isSelected = false;

    QLabel* m_nameLabel;
    QLabel* m_fontLabel;
    QPushButton* m_starBtn;
    QPushButton* m_selectBtn;
    QPushButton* m_editBtn;
    QPushButton* m_duplicateBtn;
    QPushButton* m_deleteBtn;
};
