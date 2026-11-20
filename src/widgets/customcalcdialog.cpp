#include "customcalcdialog.h"
#include "utils/expressionevaluator.h"
#include <QMessageBox>
#include <QGroupBox>

CustomCalcDialog::CustomCalcDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Adicionar Novo Cálculo Personalizado");
    resize(700, 680);
    setupUi();

    // Default example to guide the user
    m_titleEdit->setText("Velocidade Média");
    m_formulaEdit->setText("Vm = ΔS / Δt");
    m_resultUnitEdit->setText("m/s");
    m_descEdit->setText("Cálculo da velocidade média a partir do deslocamento e tempo");

    addParameterRow("ΔS", "Variação do Espaço (Deslocamento)", "m", 100.0);
    addParameterRow("Δt", "Intervalo de Tempo", "s", 5.0);

    updateLiveTest();
}

void CustomCalcDialog::setupUi()
{
    setStyleSheet(
        "QDialog {"
        "   background-color: #0D1117;"
        "   color: #E6EDF3;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        "QLabel {"
        "   color: #C9D1D9;"
        "   font-size: 12px;"
        "}"
        "QLabel#sectionHeader {"
        "   color: #58A6FF;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QLineEdit, QDoubleSpinBox {"
        "   background-color: #161B22;"
        "   color: #FFFFFF;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 6px 10px;"
        "   font-size: 12px;"
        "}"
        "QLineEdit:focus, QDoubleSpinBox:focus {"
        "   border: 1px solid #58A6FF;"
        "}"
        "QPushButton#symBtn {"
        "   background-color: #21262D;"
        "   color: #58A6FF;"
        "   border: 1px solid #30363D;"
        "   border-radius: 4px;"
        "   padding: 4px 8px;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "}"
        "QPushButton#symBtn:hover {"
        "   background-color: #30363D;"
        "   color: #FFFFFF;"
        "}"
        "QPushButton#actionBtn {"
        "   background-color: #21262D;"
        "   color: #C9D1D9;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   padding: 6px 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#actionBtn:hover {"
        "   background-color: #30363D;"
        "   color: #FFFFFF;"
        "}"
        "QPushButton#saveBtn {"
        "   background-color: #238636;"
        "   color: #FFFFFF;"
        "   border: 1px solid rgba(240, 246, 252, 0.1);"
        "   border-radius: 6px;"
        "   padding: 8px 18px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton#saveBtn:hover {"
        "   background-color: #2EA043;"
        "}"
        "QScrollArea {"
        "   border: 1px solid #21262D;"
        "   background-color: #12161F;"
        "   border-radius: 6px;"
        "}"
    );

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(14);

    // --- Title & Description ---
    QHBoxLayout* titleRow = new QHBoxLayout();
    QLabel* titleLbl = new QLabel("Título do Cálculo:", this);
    titleLbl->setFixedWidth(130);
    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("ex: Velocidade Média, Pressão Hidráulica...");
    titleRow->addWidget(titleLbl);
    titleRow->addWidget(m_titleEdit);
    rootLayout->addLayout(titleRow);

    // --- Formula Row ---
    QHBoxLayout* formulaRow = new QHBoxLayout();
    QLabel* formulaLbl = new QLabel("Fórmula Completa:", this);
    formulaLbl->setFixedWidth(130);
    m_formulaEdit = new QLineEdit(this);
    m_formulaEdit->setPlaceholderText("ex: Vm = ΔS / Δt  ou  F = m * a  ou  R = A * B + C");
    m_detectBtn = new QPushButton("⚡ Detectar Símbolos", this);
    m_detectBtn->setObjectName("actionBtn");
    m_detectBtn->setToolTip("Lê a fórmula e gera automaticamente as caixas de definição de símbolos");

    formulaRow->addWidget(formulaLbl);
    formulaRow->addWidget(m_formulaEdit, 1);
    formulaRow->addWidget(m_detectBtn);
    rootLayout->addLayout(formulaRow);

    // --- Math Symbols Bar ---
    QHBoxLayout* symBarLayout = new QHBoxLayout();
    QLabel* symLbl = new QLabel("Inserir Símbolo:", this);
    symLbl->setStyleSheet("color: #8B949E; font-size: 11px;");
    symBarLayout->addWidget(symLbl);

    QStringList quickSymbols = {"+", "-", "*", "/", "^", "√", "(", ")", "Δ", "π", "θ", "α", "β", "μ", "σ"};
    for (const QString& sym : quickSymbols) {
        QPushButton* btn = new QPushButton(sym, this);
        btn->setObjectName("symBtn");
        btn->setFixedWidth(32);
        connect(btn, &QPushButton::clicked, this, [this, sym]() {
            insertSymbolIntoFormula(sym == "√" ? "sqrt(" : sym);
        });
        symBarLayout->addWidget(btn);
    }
    symBarLayout->addStretch(1);
    rootLayout->addLayout(symBarLayout);

    // --- Parameters Definition Section ---
    QHBoxLayout* paramHeaderLayout = new QHBoxLayout();
    QLabel* paramHeaderLbl = new QLabel("Definição dos Símbolos / Variáveis (O que cada letra significa):", this);
    paramHeaderLbl->setObjectName("sectionHeader");
    m_addParamBtn = new QPushButton("➕ Adicionar Símbolo Manualmente", this);
    m_addParamBtn->setObjectName("actionBtn");

    paramHeaderLayout->addWidget(paramHeaderLbl);
    paramHeaderLayout->addStretch(1);
    paramHeaderLayout->addWidget(m_addParamBtn);
    rootLayout->addLayout(paramHeaderLayout);

    // Table Header indicator
    QHBoxLayout* thLayout = new QHBoxLayout();
    thLayout->setContentsMargins(10, 0, 10, 0);
    QLabel* thSym = new QLabel("Símbolo (na fórmula)", this); thSym->setFixedWidth(130);
    QLabel* thName = new QLabel("Significado / Nome do Campo", this);
    QLabel* thUnit = new QLabel("Unidade", this); thUnit->setFixedWidth(80);
    QLabel* thDef = new QLabel("Valor Padrão", this); thDef->setFixedWidth(90);
    QLabel* thAct = new QLabel("", this); thAct->setFixedWidth(30);

    thSym->setStyleSheet("color: #8B949E; font-weight: bold; font-size: 11px;");
    thName->setStyleSheet("color: #8B949E; font-weight: bold; font-size: 11px;");
    thUnit->setStyleSheet("color: #8B949E; font-weight: bold; font-size: 11px;");
    thDef->setStyleSheet("color: #8B949E; font-weight: bold; font-size: 11px;");

    thLayout->addWidget(thSym);
    thLayout->addWidget(thName, 1);
    thLayout->addWidget(thUnit);
    thLayout->addWidget(thDef);
    thLayout->addWidget(thAct);
    rootLayout->addLayout(thLayout);

    // Scroll Area for dynamic parameter cards
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumHeight(140);
    scrollArea->setMaximumHeight(200);

    m_paramsContainer = new QWidget(scrollArea);
    m_paramsContainer->setStyleSheet("background: transparent;");
    m_paramsLayout = new QVBoxLayout(m_paramsContainer);
    m_paramsLayout->setContentsMargins(6, 6, 6, 6);
    m_paramsLayout->setSpacing(6);
    m_paramsLayout->addStretch(1);
    scrollArea->setWidget(m_paramsContainer);

    rootLayout->addWidget(scrollArea);

    // --- Result Unit & Description Row ---
    QHBoxLayout* resultRow = new QHBoxLayout();
    QLabel* resUnitLbl = new QLabel("Unidade do Resultado:", this);
    resUnitLbl->setFixedWidth(130);
    m_resultUnitEdit = new QLineEdit(this);
    m_resultUnitEdit->setFixedWidth(100);
    m_resultUnitEdit->setPlaceholderText("ex: m/s, kg, Pa");

    QLabel* descLbl = new QLabel("Descrição / Nota:", this);
    m_descEdit = new QLineEdit(this);
    m_descEdit->setPlaceholderText("Explicação breve do cálculo (opcional)");

    resultRow->addWidget(resUnitLbl);
    resultRow->addWidget(m_resultUnitEdit);
    resultRow->addSpacing(16);
    resultRow->addWidget(descLbl);
    resultRow->addWidget(m_descEdit, 1);
    rootLayout->addLayout(resultRow);

    // --- Live Test Area ---
    QGroupBox* testGroup = new QGroupBox("🔬 Teste em Tempo Real (Validação da Fórmula)", this);
    testGroup->setStyleSheet(
        "QGroupBox {"
        "   background-color: #161B22;"
        "   border: 1px solid #30363D;"
        "   border-radius: 6px;"
        "   margin-top: 10px;"
        "   padding-top: 12px;"
        "   font-weight: bold;"
        "   color: #58A6FF;"
        "}"
        "QGroupBox::title { left: 12px; }"
    );

    QVBoxLayout* testGroupLayout = new QVBoxLayout(testGroup);
    testGroupLayout->setContentsMargins(12, 12, 12, 12);

    m_testContainer = new QWidget(testGroup);
    m_testInputsLayout = new QVBoxLayout(m_testContainer);
    m_testInputsLayout->setContentsMargins(0, 0, 0, 0);
    m_testInputsLayout->setSpacing(4);
    testGroupLayout->addWidget(m_testContainer);

    QHBoxLayout* resultDisplayLayout = new QHBoxLayout();
    QLabel* rLbl = new QLabel("Resultado Calculado:", testGroup);
    rLbl->setStyleSheet("color: #C9D1D9; font-weight: bold;");

    m_testResultLabel = new QLabel("—", testGroup);
    m_testResultLabel->setStyleSheet("color: #3FB950; font-size: 14px; font-weight: bold;");

    m_statusMessageLabel = new QLabel("", testGroup);
    m_statusMessageLabel->setStyleSheet("color: #F85149; font-size: 11px;");

    resultDisplayLayout->addWidget(rLbl);
    resultDisplayLayout->addWidget(m_testResultLabel);
    resultDisplayLayout->addStretch(1);
    resultDisplayLayout->addWidget(m_statusMessageLabel);
    testGroupLayout->addLayout(resultDisplayLayout);

    rootLayout->addWidget(testGroup);

    // --- Footer Action Buttons ---
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);

    m_cancelBtn = new QPushButton("Cancelar", this);
    m_cancelBtn->setObjectName("actionBtn");

    m_saveBtn = new QPushButton("💾 Salvar Cálculo", this);
    m_saveBtn->setObjectName("saveBtn");

    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);
    rootLayout->addLayout(btnLayout);

    // Connections
    connect(m_formulaEdit, &QLineEdit::textChanged, this, &CustomCalcDialog::onFormulaChanged);
    connect(m_detectBtn, &QPushButton::clicked, this, &CustomCalcDialog::detectSymbolsFromFormula);
    connect(m_addParamBtn, &QPushButton::clicked, this, [this]() { addParameterRow(); });
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveBtn, &QPushButton::clicked, this, &CustomCalcDialog::onSaveClicked);
}

void CustomCalcDialog::insertSymbolIntoFormula(const QString& symbol)
{
    m_formulaEdit->insert(symbol);
    m_formulaEdit->setFocus();
}

void CustomCalcDialog::onFormulaChanged(const QString& text)
{
    Q_UNUSED(text);
    updateLiveTest();
}

void CustomCalcDialog::detectSymbolsFromFormula()
{
    QString formulaText = m_formulaEdit->text().trimmed();
    QStringList vars = ExpressionEvaluator::extractVariables(formulaText);

    if (vars.isEmpty()) {
        QMessageBox::information(this, "Detectar Símbolos",
                                 "Nenhum símbolo/variável foi detectado na fórmula.\n"
                                 "Exemplo de fórmula: Vm = ΔS / Δt");
        return;
    }

    // Keep existing rows that match detected variables, add new ones
    QSet<QString> existingSymbols;
    for (const auto& row : m_paramRows) {
        existingSymbols.insert(row.symbolEdit->text().trimmed());
    }

    for (const QString& v : vars) {
        if (!existingSymbols.contains(v)) {
            addParameterRow(v, QString("Valor de %1").arg(v), "", 1.0);
        }
    }

    updateLiveTest();
}

void CustomCalcDialog::addParameterRow(const QString& symbol, const QString& name,
                                       const QString& unit, double defaultValue)
{
    QWidget* rowWidget = new QWidget(m_paramsContainer);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(6);

    QLineEdit* symEdit = new QLineEdit(symbol, rowWidget);
    symEdit->setFixedWidth(130);
    symEdit->setPlaceholderText("ex: ΔS");

    QLineEdit* nameEdit = new QLineEdit(name, rowWidget);
    nameEdit->setPlaceholderText("ex: Variação de Espaço");

    QLineEdit* unitEdit = new QLineEdit(unit, rowWidget);
    unitEdit->setFixedWidth(80);
    unitEdit->setPlaceholderText("ex: m");

    QDoubleSpinBox* defSpin = new QDoubleSpinBox(rowWidget);
    defSpin->setFixedWidth(90);
    defSpin->setRange(-1e9, 1e9);
    defSpin->setValue(defaultValue);

    QPushButton* removeBtn = new QPushButton("✕", rowWidget);
    removeBtn->setFixedWidth(30);
    removeBtn->setObjectName("actionBtn");
    removeBtn->setStyleSheet("color: #F85149; font-weight: bold;");

    rowLayout->addWidget(symEdit);
    rowLayout->addWidget(nameEdit, 1);
    rowLayout->addWidget(unitEdit);
    rowLayout->addWidget(defSpin);
    rowLayout->addWidget(removeBtn);

    m_paramsLayout->insertWidget(m_paramsLayout->count() - 1, rowWidget);

    ParamRowWidget item;
    item.container = rowWidget;
    item.symbolEdit = symEdit;
    item.nameEdit = nameEdit;
    item.unitEdit = unitEdit;
    item.defaultSpin = defSpin;
    item.removeBtn = removeBtn;
    m_paramRows.append(item);

    connect(symEdit, &QLineEdit::textChanged, this, &CustomCalcDialog::updateLiveTest);
    connect(defSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CustomCalcDialog::updateLiveTest);
    connect(removeBtn, &QPushButton::clicked, this, [this, rowWidget]() {
        for (int i = 0; i < m_paramRows.size(); ++i) {
            if (m_paramRows[i].container == rowWidget) {
                removeParameterRow(i);
                break;
            }
        }
    });

    updateLiveTest();
}

void CustomCalcDialog::removeParameterRow(int index)
{
    if (index >= 0 && index < m_paramRows.size()) {
        auto row = m_paramRows.takeAt(index);
        row.container->deleteLater();
        updateLiveTest();
    }
}

void CustomCalcDialog::updateLiveTest()
{
    // Rebuild test inputs according to defined parameters
    // Preserve existing values where possible
    QMap<QString, double> oldValues;
    for (auto it = m_testSpinBoxes.cbegin(); it != m_testSpinBoxes.cend(); ++it) {
        oldValues[it.key()] = it.value()->value();
    }

    // Clear old test layout widgets
    QLayoutItem* item;
    while ((item = m_testInputsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    m_testSpinBoxes.clear();

    QMap<QString, double> currentVars;

    for (const auto& row : m_paramRows) {
        QString sym = row.symbolEdit->text().trimmed();
        if (sym.isEmpty()) continue;

        QWidget* inputRow = new QWidget(m_testContainer);
        QHBoxLayout* inLayout = new QHBoxLayout(inputRow);
        inLayout->setContentsMargins(0, 0, 0, 0);

        QString labelText = QString("%1 (%2):").arg(sym).arg(row.nameEdit->text().trimmed().isEmpty()
                                                                 ? "Parâmetro"
                                                                 : row.nameEdit->text().trimmed());
        QLabel* inLbl = new QLabel(labelText, inputRow);
        inLbl->setFixedWidth(200);

        QDoubleSpinBox* spin = new QDoubleSpinBox(inputRow);
        spin->setRange(-1e9, 1e9);
        spin->setDecimals(3);
        double val = oldValues.contains(sym) ? oldValues[sym] : row.defaultSpin->value();
        if (val == 0.0) val = 1.0; // convenient non-zero test value
        spin->setValue(val);

        QLabel* unitLbl = new QLabel(row.unitEdit->text(), inputRow);
        unitLbl->setFixedWidth(50);

        inLayout->addWidget(inLbl);
        inLayout->addWidget(spin, 1);
        inLayout->addWidget(unitLbl);

        m_testInputsLayout->addWidget(inputRow);
        m_testSpinBoxes[sym] = spin;
        currentVars[sym] = spin->value();

        connect(spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, [this]() {
            // Re-evaluate without recreating inputs
            QMap<QString, double> vars;
            for (auto it = m_testSpinBoxes.cbegin(); it != m_testSpinBoxes.cend(); ++it) {
                vars[it.key()] = it.value()->value();
            }
            QString err;
            double res = ExpressionEvaluator::evaluate(m_formulaEdit->text(), vars, &err);
            if (!err.isEmpty()) {
                m_testResultLabel->setText("—");
                m_statusMessageLabel->setText("⚠️ " + err);
            } else {
                m_testResultLabel->setText(QString("%1 %2").arg(res, 0, 'g', 6).arg(m_resultUnitEdit->text()));
                m_statusMessageLabel->setText("✓ Fórmula Válida");
                m_statusMessageLabel->setStyleSheet("color: #3FB950; font-size: 11px;");
            }
        });
    }

    // Evaluate formula with current variables
    QString err;
    double result = ExpressionEvaluator::evaluate(m_formulaEdit->text(), currentVars, &err);

    if (!err.isEmpty()) {
        m_testResultLabel->setText("—");
        m_statusMessageLabel->setText("⚠️ " + err);
        m_statusMessageLabel->setStyleSheet("color: #F85149; font-size: 11px;");
    } else {
        m_testResultLabel->setText(QString("%1 %2").arg(result, 0, 'g', 6).arg(m_resultUnitEdit->text()));
        m_statusMessageLabel->setText("✓ Fórmula Válida");
        m_statusMessageLabel->setStyleSheet("color: #3FB950; font-size: 11px;");
    }
}

void CustomCalcDialog::onSaveClicked()
{
    QString t = m_titleEdit->text().trimmed();
    if (t.isEmpty()) {
        QMessageBox::warning(this, "Validação", "Por favor, informe o título do cálculo.");
        m_titleEdit->setFocus();
        return;
    }

    QString f = m_formulaEdit->text().trimmed();
    if (f.isEmpty()) {
        QMessageBox::warning(this, "Validação", "Por favor, informe a fórmula do cálculo.");
        m_formulaEdit->setFocus();
        return;
    }

    if (m_paramRows.isEmpty()) {
        QMessageBox::warning(this, "Validação", "Adicione pelo menos um símbolo/parâmetro para o cálculo.");
        return;
    }

    QStringList definedSymbols;
    for (const auto& row : m_paramRows) {
        QString s = row.symbolEdit->text().trimmed();
        if (s.isEmpty()) {
            QMessageBox::warning(this, "Validação", "Há parâmetros sem símbolo preenchido.");
            row.symbolEdit->setFocus();
            return;
        }
        definedSymbols.append(s);
    }

    QString err;
    if (!ExpressionEvaluator::validate(f, definedSymbols, &err)) {
        QMessageBox::warning(this, "Erro na Fórmula",
                             "A fórmula não pôde ser validada:\n" + err);
        return;
    }

    accept();
}

QString CustomCalcDialog::title() const
{
    return m_titleEdit->text().trimmed();
}

QString CustomCalcDialog::formula() const
{
    return m_formulaEdit->text().trimmed();
}

QString CustomCalcDialog::resultUnit() const
{
    return m_resultUnitEdit->text().trimmed();
}

QString CustomCalcDialog::description() const
{
    return m_descEdit->text().trimmed();
}

QVector<CalcParameter> CustomCalcDialog::parameters() const
{
    QVector<CalcParameter> params;
    for (const auto& row : m_paramRows) {
        CalcParameter p;
        p.symbol = row.symbolEdit->text().trimmed();
        p.name = row.nameEdit->text().trimmed().isEmpty() ? p.symbol : row.nameEdit->text().trimmed();
        p.unit = row.unitEdit->text().trimmed();
        p.defaultValue = row.defaultSpin->value();
        p.tooltip = QString("%1 (%2)").arg(p.name).arg(p.symbol);
        params.append(p);
    }
    return params;
}
