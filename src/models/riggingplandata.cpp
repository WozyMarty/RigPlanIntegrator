#include "riggingplandata.h"

RiggingPlanData::RiggingPlanData()
{
}

RiggingPlanData RiggingPlanData::createDefault()
{
    RiggingPlanData d;
    d.cliente = "Kaefer Rip Serviços Industriais Ltda.";
    d.local = "Curitiba";
    d.data = "24/07/2026";
    d.planoNumero = "2607036";

    d.cargaDescricao = "Chiller";
    d.cargaComp = 4.7;
    d.cargaLargura = 1.95;
    d.cargaAltura = 2.3;
    d.cargaPeso = 3450.0;

    // 4 itens de lingada conforme o template
    d.lingadaItems = {
        { 1, "Cinta plana 3 metros de comprimento - Capacidade 6t", "-", 4.0, 4.87 },
        { 2, "Manilha Curva 7/8\"com pino roscado - GRAU 6 - FS 6:1", "-", 6.0, 1.70 },
        { 3, "Cinta plana 4 metros de comprimento - Capacidade 6t", "-", 2.0, 5.97 },
        { 4, "Manilha Curva 1\"com pino roscado - GRAU 6 - FS 6:1", "-", 2.0, 2.50 }
    };

    // Item de balancim conforme o template
    d.balancimItems = {
        { 5, "Balancim", "-", 1.0, 250.0 }
    };

    d.guindasteModelo = "Palfinger MD 45.007";
    d.guindasteCaminhao = "Volvo/VM 270 6X2R";
    d.guindastePlacas = "IUL8A16";
    d.guindasteChassi = "93KP0R1C7DE141594";
    d.guindasteAno = "2013";

    d.capNominal = 45.0;
    d.compLanca = 5.1;
    d.raioOperacao = 5.0;
    d.capCargaNoRaio = 7.95;
    d.quadranteInicial = "LATERAL";
    d.quadranteFinal = "LATERAL";

    d.cargasExtras = 0.0;
    d.artNumero = "1720264496217";
    d.observacoes = "";
    d.riggerNome = "Rigger Giuliano Breda - RPP-23-1040";
    d.supervisorNome = "Supervisor Responsável";

    return d;
}

QJsonObject RiggingPlanData::toJson() const
{
    QJsonObject o;
    o["cliente"] = cliente;
    o["local"] = local;
    o["data"] = data;
    o["plano_numero"] = planoNumero;

    o["carga_descricao"] = cargaDescricao;
    o["carga_comp"] = cargaComp;
    o["carga_largura"] = cargaLargura;
    o["carga_altura"] = cargaAltura;
    o["carga_peso"] = cargaPeso;

    QJsonArray lingadaArr;
    for (const auto& it : lingadaItems) {
        lingadaArr.append(it.toJson());
    }
    o["lingada_items"] = lingadaArr;

    QJsonArray balancimArr;
    for (const auto& it : balancimItems) {
        balancimArr.append(it.toJson());
    }
    o["balancim_items"] = balancimArr;

    o["guindaste_modelo"] = guindasteModelo;
    o["guindaste_caminhao"] = guindasteCaminhao;
    o["guindaste_placas"] = guindastePlacas;
    o["guindaste_chassi"] = guindasteChassi;
    o["guindaste_ano"] = guindasteAno;

    o["cap_nominal"] = capNominal;
    o["comp_lanca"] = compLanca;
    o["raio_operacao"] = raioOperacao;
    o["cap_carga_no_raio"] = capCargaNoRaio;
    o["quadrante_inicial"] = quadranteInicial;
    o["quadrante_final"] = quadranteFinal;

    o["cargas_extras"] = cargasExtras;
    o["art_numero"] = artNumero;
    o["observacoes"] = observacoes;
    o["rigger_nome"] = riggerNome;
    o["supervisor_nome"] = supervisorNome;

    return o;
}

RiggingPlanData RiggingPlanData::fromJson(const QJsonObject& o)
{
    RiggingPlanData d = createDefault();

    if (o.contains("cliente")) d.cliente = o["cliente"].toString();
    if (o.contains("local")) d.local = o["local"].toString();
    if (o.contains("data")) d.data = o["data"].toString();
    if (o.contains("plano_numero")) d.planoNumero = o["plano_numero"].toString();

    if (o.contains("carga_descricao")) d.cargaDescricao = o["carga_descricao"].toString();
    if (o.contains("carga_comp")) d.cargaComp = o["carga_comp"].toDouble(d.cargaComp);
    if (o.contains("carga_largura")) d.cargaLargura = o["carga_largura"].toDouble(d.cargaLargura);
    if (o.contains("carga_altura")) d.cargaAltura = o["carga_altura"].toDouble(d.cargaAltura);
    if (o.contains("carga_peso")) d.cargaPeso = o["carga_peso"].toDouble(d.cargaPeso);

    if (o.contains("lingada_items")) {
        d.lingadaItems.clear();
        for (const auto& val : o["lingada_items"].toArray()) {
            d.lingadaItems.append(RiggingPlanItem::fromJson(val.toObject()));
        }
    }

    if (o.contains("balancim_items")) {
        d.balancimItems.clear();
        for (const auto& val : o["balancim_items"].toArray()) {
            d.balancimItems.append(RiggingPlanItem::fromJson(val.toObject()));
        }
    }

    if (o.contains("guindaste_modelo")) d.guindasteModelo = o["guindaste_modelo"].toString();
    if (o.contains("guindaste_caminhao")) d.guindasteCaminhao = o["guindaste_caminhao"].toString();
    if (o.contains("guindaste_placas")) d.guindastePlacas = o["guindaste_placas"].toString();
    if (o.contains("guindaste_chassi")) d.guindasteChassi = o["guindaste_chassi"].toString();
    if (o.contains("guindaste_ano")) d.guindasteAno = o["guindaste_ano"].toString();

    if (o.contains("cap_nominal")) d.capNominal = o["cap_nominal"].toDouble(d.capNominal);
    if (o.contains("comp_lanca")) d.compLanca = o["comp_lanca"].toDouble(d.compLanca);
    if (o.contains("raio_operacao")) d.raioOperacao = o["raio_operacao"].toDouble(d.raioOperacao);
    if (o.contains("cap_carga_no_raio")) d.capCargaNoRaio = o["cap_carga_no_raio"].toDouble(d.capCargaNoRaio);
    if (o.contains("quadrante_inicial")) d.quadranteInicial = o["quadrante_inicial"].toString();
    if (o.contains("quadrante_final")) d.quadranteFinal = o["quadrante_final"].toString();

    if (o.contains("cargas_extras")) d.cargasExtras = o["cargas_extras"].toDouble(d.cargasExtras);
    if (o.contains("art_numero")) d.artNumero = o["art_numero"].toString();
    if (o.contains("observacoes")) d.observacoes = o["observacoes"].toString();
    if (o.contains("rigger_nome")) d.riggerNome = o["rigger_nome"].toString();
    if (o.contains("supervisor_nome")) d.supervisorNome = o["supervisor_nome"].toString();

    return d;
}
