#include "craneprofile.h"
#include <QUuid>

QJsonObject CraneProfile::toJson() const
{
    QJsonObject o;
    o["id"] = id.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : id;
    o["modelo"] = modelo;
    o["fabricante"] = fabricante;
    o["caminhao"] = caminhao;
    o["placas"] = placas;
    o["chassi"] = chassi;
    o["ano"] = ano;
    o["cap_nominal"] = capNominal;
    o["comp_lanca"] = compLanca;
    o["raio_operacao"] = raioOperacao;
    o["cap_carga_no_raio"] = capCargaNoRaio;
    o["quadrante_inicial"] = quadranteInicial;
    o["quadrante_final"] = quadranteFinal;
    o["categoria"] = categoria;
    o["observacoes"] = observacoes;
    return o;
}

CraneProfile CraneProfile::fromJson(const QJsonObject& json)
{
    CraneProfile c;
    c.id = json["id"].toString(QUuid::createUuid().toString(QUuid::WithoutBraces));
    c.modelo = json["modelo"].toString();
    c.fabricante = json["fabricante"].toString();
    c.caminhao = json["caminhao"].toString();
    c.placas = json["placas"].toString();
    c.chassi = json["chassi"].toString();
    c.ano = json["ano"].toString();
    c.capNominal = json["cap_nominal"].toDouble(30.0);
    c.compLanca = json["comp_lanca"].toDouble(5.1);
    c.raioOperacao = json["raio_operacao"].toDouble(5.0);
    c.capCargaNoRaio = json["cap_carga_no_raio"].toDouble(8.0);
    c.quadranteInicial = json["quadrante_inicial"].toString("LATERAL");
    c.quadranteFinal = json["quadrante_final"].toString("LATERAL");
    c.categoria = json["categoria"].toString("Rodoviário / Articulado");
    c.observacoes = json["observacoes"].toString();
    return c;
}

QVector<CraneProfile> CraneProfile::createDefaultCatalog()
{
    QVector<CraneProfile> catalog;

    // 1. Madal Palfinger MD 300 (Dados exatos da imagem do usuário: 30,0t | 5,1m | 5,0m | 8,0t | LATERAL)
    CraneProfile c1;
    c1.id = "crane_madal_md300";
    c1.modelo = "Madal Palfinger MD 300";
    c1.fabricante = "Madal Palfinger";
    c1.caminhao = "Mercedes-Benz 2423 6x2";
    c1.placas = "BCK-3412";
    c1.chassi = "9BM9580436B123456";
    c1.ano = "2015";
    c1.capNominal = 30.0;
    c1.compLanca = 5.1;
    c1.raioOperacao = 5.0;
    c1.capCargaNoRaio = 8.0;
    c1.quadranteInicial = "LATERAL";
    c1.quadranteFinal = "LATERAL";
    c1.categoria = "Articulado / Munck";
    c1.observacoes = "Guindaste articulado sobre caminhão MB 2423, patolamento 100%.";
    catalog.append(c1);

    // 2. Palfinger MD 45.007 (Padrão do sistema)
    CraneProfile c2;
    c2.id = "crane_palfinger_md45007";
    c2.modelo = "Palfinger MD 45.007";
    c2.fabricante = "Palfinger";
    c2.caminhao = "Volvo/VM 270 6X2R";
    c2.placas = "IUL8A16";
    c2.chassi = "93KP0R1C7DE141594";
    c2.ano = "2013";
    c2.capNominal = 45.0;
    c2.compLanca = 5.1;
    c2.raioOperacao = 5.0;
    c2.capCargaNoRaio = 7.9;
    c2.quadranteInicial = "LATERAL";
    c2.quadranteFinal = "LATERAL";
    c2.categoria = "Articulado / Munck";
    c2.observacoes = "Guindaste articulado com guincho de cabo 4,5t e controle remoto.";
    catalog.append(c2);

    // 3. Liebherr LTM 1070-4.1 (70t AT)
    CraneProfile c3;
    c3.id = "crane_liebherr_ltm1070";
    c3.modelo = "Liebherr LTM 1070-4.1";
    c3.fabricante = "Liebherr";
    c3.caminhao = "Liebherr Carrier 8x6x8";
    c3.placas = "RHO-4B70";
    c3.chassi = "W097041009L129481";
    c3.ano = "2019";
    c3.capNominal = 70.0;
    c3.compLanca = 26.5;
    c3.raioOperacao = 8.0;
    c3.capCargaNoRaio = 14.2;
    c3.quadranteInicial = "360°";
    c3.quadranteFinal = "360°";
    c3.categoria = "Telescópico Todo-Terreno (AT)";
    c3.observacoes = "Contrapeso completo 14,5t. Patolas totalmente estendidas 6,3m.";
    catalog.append(c3);

    // 4. Tadano ATF 70G-4 (70t AT)
    CraneProfile c4;
    c4.id = "crane_tadano_atf70";
    c4.modelo = "Tadano ATF 70G-4";
    c4.fabricante = "Tadano";
    c4.caminhao = "Tadano Faun 4-Eixos";
    c4.placas = "PLO-9C33";
    c4.chassi = "W09ATF7049E004123";
    c4.ano = "2018";
    c4.capNominal = 70.0;
    c4.compLanca = 32.1;
    c4.raioOperacao = 10.0;
    c4.capCargaNoRaio = 11.5;
    c4.quadranteInicial = "360°";
    c4.quadranteFinal = "360°";
    c4.categoria = "Telescópico Todo-Terreno (AT)";
    c4.observacoes = "Lança telescópica de 5 seções, patolas 6,4m.";
    catalog.append(c4);

    // 5. Sany STC 800S (80t Rodoviário)
    CraneProfile c5;
    c5.id = "crane_sany_stc800s";
    c5.modelo = "Sany STC 800S";
    c5.fabricante = "Sany";
    c5.caminhao = "Sany Heavy 4-Eixos";
    c5.placas = "SNY-8A00";
    c5.chassi = "LANY800S9K0098712";
    c5.ano = "2021";
    c5.capNominal = 80.0;
    c5.compLanca = 35.0;
    c5.raioOperacao = 9.0;
    c5.capCargaNoRaio = 16.0;
    c5.quadranteInicial = "360°";
    c5.quadranteFinal = "360°";
    c5.categoria = "Telescópico Rodoviário (Truck Crane)";
    c5.observacoes = "Lança em formato U de 5 seções, contrapeso 16t.";
    catalog.append(c5);

    // 6. Grove GMK 4100L-1 (100t AT)
    CraneProfile c6;
    c6.id = "crane_grove_gmk4100";
    c6.modelo = "Grove GMK 4100L-1";
    c6.fabricante = "Grove / Manitowoc";
    c6.caminhao = "Grove 4-Eixos Carrier";
    c6.placas = "GRV-4100";
    c6.chassi = "W094100L9M1049281";
    c6.ano = "2020";
    c6.capNominal = 100.0;
    c6.compLanca = 42.0;
    c6.raioOperacao = 12.0;
    c6.capCargaNoRaio = 13.8;
    c6.quadranteInicial = "360°";
    c6.quadranteFinal = "360°";
    c6.categoria = "Telescópico Todo-Terreno (AT)";
    c6.observacoes = "Lança Megaform Twin-Lock 60m, sistema MAXbase de patolamento.";
    catalog.append(c6);

    return catalog;
}
