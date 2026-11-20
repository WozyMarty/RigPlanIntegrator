#pragma once

#include <QString>
#include <QJsonObject>
#include <QVector>

/**
 * @brief Perfil de dados e configurações de um Guindaste para o Plano de Rigging
 */
struct CraneProfile {
    QString id;
    QString modelo;             // Ex: "Madal Palfinger MD 300", "Palfinger MD 45.007"
    QString fabricante;         // Ex: "Palfinger", "Liebherr", "Madal", "Sany"
    QString caminhao;           // Ex: "Mercedes-Benz 2423 6x2", "Volvo/VM 270 6X2R"
    QString placas;             // Ex: "BCK-3412", "IUL8A16"
    QString chassi;             // Ex: "9BM9580436B123456", "93KP0R1C7DE141594"
    QString ano;                // Ex: "2015", "2013"

    // Configuração Operacional (Configuração do Guindaste no Plano)
    double capNominal = 30.0;    // t (Capacidade nominal)
    double compLanca = 5.1;      // m (Comprimento da lança)
    double raioOperacao = 5.0;   // m (Raio de operação)
    double capCargaNoRaio = 8.0; // t (Capacidade de carga no raio)
    QString quadranteInicial = "LATERAL";
    QString quadranteFinal = "LATERAL";

    QString categoria = "Rodoviário / Articulado";
    QString observacoes;

    QJsonObject toJson() const;
    static CraneProfile fromJson(const QJsonObject& json);
    static QVector<CraneProfile> createDefaultCatalog();
};
