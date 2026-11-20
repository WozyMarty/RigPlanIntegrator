#pragma once

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QPushButton>
#include <QTabWidget>
#include "models/riggingplandata.h"
#include "models/formatpreset.h"

/**
 * @brief Diálogo moderno para edição de todos os campos editáveis (pontos vermelhos) do Plano de Rigging
 */
class RiggingPlanDataDialog : public QDialog {
    Q_OBJECT

public:
    explicit RiggingPlanDataDialog(const RiggingPlanData& data, QWidget* parent = nullptr);
    RiggingPlanData planData() const;

private slots:
    void onAddLingadaItem();
    void onRemoveLingadaItem();
    void onAddBalancimItem();
    void onRemoveBalancimItem();
    void recalculateTotals();

private:
    void setupUi();
    void populateFields(const RiggingPlanData& data);

    // 1. Identificação
    QLineEdit* m_clienteEdit;
    QLineEdit* m_localEdit;
    QLineEdit* m_dataEdit;
    QLineEdit* m_planoNumEdit;

    // 2. Carga
    QLineEdit* m_cargaDescEdit;
    QDoubleSpinBox* m_cargaCompSpin;
    QDoubleSpinBox* m_cargaLarguraSpin;
    QDoubleSpinBox* m_cargaAlturaSpin;
    QDoubleSpinBox* m_cargaPesoSpin;

    // 3. Lingada Table
    QTableWidget* m_lingadaTable;
    QLabel* m_lingadaTotalLabel;

    // 4. Balancim Table
    QTableWidget* m_balancimTable;
    QLabel* m_balancimTotalLabel;

    // 5. Guindaste
    QLineEdit* m_guindasteModeloEdit;
    QLineEdit* m_guindasteCaminhaoEdit;
    QLineEdit* m_guindastePlacasEdit;
    QLineEdit* m_guindasteChassiEdit;
    QLineEdit* m_guindasteAnoEdit;

    // 6. Configuração
    QDoubleSpinBox* m_capNominalSpin;
    QDoubleSpinBox* m_compLancaSpin;
    QDoubleSpinBox* m_raioOperacaoSpin;
    QDoubleSpinBox* m_capCargaNoRaioSpin;
    QLineEdit* m_quadranteInicialEdit;
    QLineEdit* m_quadranteFinalEdit;

    // 7. Extras & Assinaturas
    QDoubleSpinBox* m_cargasExtrasSpin;
    QLineEdit* m_artNumeroEdit;
    QLineEdit* m_observacoesEdit;
    QLineEdit* m_riggerNomeEdit;
    QLineEdit* m_supervisorNomeEdit;

    QLabel* m_cargaBrutaLabel;
};
