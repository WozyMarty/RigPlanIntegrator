#pragma once

#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>

/**
 * @brief Item individual de lingada, amarra ou acessório no Plano de Rigging
 */
struct RiggingPlanItem {
    int item = 1;
    QString descricao;
    QString refDesenho = "-";
    double qtd = 1.0;
    double unitario = 0.0; // kg

    // Multiplicação (linha azul): Qtd x Unitário = Total (ponto verde)
    double total() const {
        return qtd * unitario;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["item"] = item;
        o["descricao"] = descricao;
        o["ref_desenho"] = refDesenho;
        o["qtd"] = qtd;
        o["unitario"] = unitario;
        return o;
    }

    static RiggingPlanItem fromJson(const QJsonObject& o) {
        RiggingPlanItem it;
        it.item = o["item"].toInt(1);
        it.descricao = o["descricao"].toString();
        it.refDesenho = o["ref_desenho"].toString("-");
        it.qtd = o["qtd"].toDouble(1.0);
        it.unitario = o["unitario"].toDouble(0.0);
        return it;
    }
};

/**
 * @brief Dados estruturados do documento completo de Plano de Rigging
 */
class RiggingPlanData {
public:
    RiggingPlanData();

    // 1. Cabeçalho (Pontos vermelhos)
    QString cliente = "Kaefer Rip Serviços Industriais Ltda.";
    QString local = "Curitiba";
    QString data = "24/07/2026";
    QString planoNumero = "2607036";

    // 2. Dados da Carga (Pontos vermelhos)
    QString cargaDescricao = "Chiller";
    double cargaComp = 4.7;
    double cargaLargura = 1.95;
    double cargaAltura = 2.3;
    double cargaPeso = 3450.0; // kg

    // 3. Lingada / Amarras (Itens com Qtd e Unitário editáveis)
    QVector<RiggingPlanItem> lingadaItems;

    // 4. Dispositivos / Balancim
    QVector<RiggingPlanItem> balancimItems;

    // 5. Informações do Guindaste (Pontos vermelhos)
    QString guindasteModelo = "Palfinger MD 45.007";
    QString guindasteCaminhao = "Volvo/VM 270 6X2R";
    QString guindastePlacas = "IUL8A16";
    QString guindasteChassi = "93KP0R1C7DE141594";
    QString guindasteAno = "2013";

    // 6. Configuração do Guindaste (Pontos vermelhos)
    double capNominal = 45.0; // t
    double compLanca = 5.1;   // m
    double raioOperacao = 5.0; // m
    double capCargaNoRaio = 7.9; // t (capacidade de carga no raio de operação)
    QString quadranteInicial = "LATERAL";
    QString quadranteFinal = "LATERAL";

    // 7. Cargas Extras (Ponto vermelho)
    double cargasExtras = 0.0; // kg

    // 8. Observações, ART e Assinaturas (Pontos vermelhos)
    QString artNumero = "1720264496217";
    QString observacoes = "";
    QString riggerNome = "Rigger Giuliano Breda - RPP-23-1040";
    QString supervisorNome = "Supervisor Responsável";

    // --- Cálculos automáticos ---
    // Soma dos blocos verdes da Lingada = Ponto amarelo
    double pesoTotalLingada() const {
        double sum = 0.0;
        for (const auto& it : lingadaItems) {
            sum += it.total();
        }
        return sum;
    }

    // Soma dos blocos verdes do Balancim = Ponto amarelo
    double pesoTotalBalancim() const {
        double sum = 0.0;
        for (const auto& it : balancimItems) {
            sum += it.total();
        }
        return sum;
    }

    // Carga bruta total = Carga líquida + Lingada + Balancim + Cargas extras
    double cargaBruta() const {
        return cargaPeso + pesoTotalLingada() + pesoTotalBalancim() + cargasExtras;
    }

    // Carga bruta em toneladas
    double cargaBrutaTon() const {
        return cargaBruta() / 1000.0;
    }

    // Taxa de utilização do guindaste (%)
    double taxaUtilizacaoGuindaste() const {
        if (capCargaNoRaio <= 0.0) return 0.0;
        return (cargaBrutaTon() / capCargaNoRaio) * 100.0;
    }

    // Serialization
    QJsonObject toJson() const;
    static RiggingPlanData fromJson(const QJsonObject& json);
    static RiggingPlanData createDefault();
};
