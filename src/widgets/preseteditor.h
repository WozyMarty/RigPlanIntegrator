#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QTextBrowser>
#include "models/formatpreset.h"
#include "models/presetmanager.h"

/**
 * @brief Detailed visual editor for document format presets (Skins).
 *
 * Allows modifying font family, sizes, title/accent/body colors,
 * margins, header/footer, and table style with live preview.
 */
class PresetEditor : public QWidget {
    Q_OBJECT

public:
    explicit PresetEditor(QWidget* parent = nullptr);
    explicit PresetEditor(PresetManager* manager, QWidget* parent = nullptr);

    void setManager(PresetManager* manager);
    void loadPreset(const FormatPreset& preset, bool isNew = false);


signals:
    void backToGalleryRequested();
    void presetSaved(const FormatPreset& preset);

private slots:
    void updatePreview();
    void onSaveClicked();
    void onPickColorTitle();
    void onPickColorAccent();
    void onPickColorTableHeader();
    void onPickColorBody();

private:
    void setupUi();
    void setupColorButton(QPushButton* btn, const QColor& color);

    PresetManager* m_manager;
    FormatPreset m_editingPreset;
    bool m_isNewPreset = false;

    // UI Fields
    QLineEdit* m_nameEdit;
    QComboBox* m_layoutCombo;
    QComboBox* m_fontCombo;
    QSpinBox* m_fontSizeTitleSpin;
    QSpinBox* m_fontSizeSubtitleSpin;
    QSpinBox* m_fontSizeBodySpin;

    QPushButton* m_colorTitleBtn;
    QColor m_colorTitle;

    QPushButton* m_colorAccentBtn;
    QColor m_colorAccent;

    QPushButton* m_colorTableHeaderBtn;
    QColor m_colorTableHeader;

    QPushButton* m_colorBodyBtn;
    QColor m_colorBody;

    QSpinBox* m_marginTopSpin;
    QSpinBox* m_marginBottomSpin;
    QSpinBox* m_marginLeftSpin;
    QSpinBox* m_marginRightSpin;

    QLineEdit* m_headerTextEdit;
    QLineEdit* m_subtitleEdit;
    QLineEdit* m_artEdit;
    QLineEdit* m_riggingEdit;
    QLineEdit* m_footerTextEdit;

    QComboBox* m_tableStyleCombo;
    QCheckBox* m_tableBordersCheck;
    QCheckBox* m_tableAlternatingCheck;

    // Mini Live Preview
    QTextBrowser* m_previewViewer;

    QPushButton* m_saveBtn;
    QPushButton* m_cancelBtn;
};
