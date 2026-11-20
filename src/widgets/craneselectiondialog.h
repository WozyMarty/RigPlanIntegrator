#pragma once

#include <QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include "models/craneprofile.h"
#include "models/riggingplandata.h"

/**
 * @brief Diálogo visual para listar, selecionar, cadastrar e gerenciar guindastes,
 * permitindo aplicar dinamicamente a configuração operacional ao Plano de Rigging.
 */
class CraneSelectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit CraneSelectionDialog(const RiggingPlanData& currentPlanData, QWidget* parent = nullptr);

    CraneProfile selectedCrane() const;

private slots:
    void onSearchTextChanged(const QString& text);
    void onCraneListItemSelected(int row);
    void onAddNewCrane();
    void onDuplicateCrane();
    void onDeleteCrane();
    void onUsePlanData();
    void onSaveCraneToCatalog();
    void onFieldChanged();
    void onApplyAndClose();

private:
    void setupUi();
    void refreshCraneList(const QString& filter = QString());
    void loadCraneIntoForm(const CraneProfile& crane);
    CraneProfile formToCrane() const;
    void updateLiveTablePreview();

    RiggingPlanData m_planData;
    CraneProfile m_currentCrane;
    bool m_isCreatingNew = false;
    bool m_updatingForm = false;

    // Controls - Left List
    QLineEdit* m_searchEdit;
    QListWidget* m_craneList;
    QPushButton* m_addNewBtn;
    QPushButton* m_usePlanDataBtn;
    QPushButton* m_duplicateBtn;
    QPushButton* m_deleteBtn;

    // Controls - Equipment Info
    QLineEdit* m_modeloEdit;
    QLineEdit* m_fabricanteEdit;
    QLineEdit* m_caminhaoEdit;
    QLineEdit* m_placasEdit;
    QLineEdit* m_chassiEdit;
    QLineEdit* m_anoEdit;
    QLineEdit* m_categoriaEdit;

    // Controls - Operational Config
    QDoubleSpinBox* m_capNominalSpin;
    QDoubleSpinBox* m_compLancaSpin;
    QDoubleSpinBox* m_raioOperacaoSpin;
    QDoubleSpinBox* m_capCargaNoRaioSpin;
    QComboBox* m_quadranteInicialCombo;
    QComboBox* m_quadranteFinalCombo;

    // Preview Labels (matching the exact table from user's image)
    QLabel* m_prevCapNominal;
    QLabel* m_prevCompLanca;
    QLabel* m_prevRaioOperacao;
    QLabel* m_prevCapCarga;
    QLabel* m_prevQuadInicial;
    QLabel* m_prevQuadFinal;

    QLabel* m_prevCargaLiquida;
    QLabel* m_prevLingada;
    QLabel* m_prevBalancim;
    QLabel* m_prevCargasExtras;
    QLabel* m_prevCargaBruta;
    QLabel* m_prevTaxaUtilizacao;

    QPushButton* m_saveCatalogBtn;
    QPushButton* m_applyBtn;
    QPushButton* m_cancelBtn;
};
