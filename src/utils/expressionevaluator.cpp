#include "expressionevaluator.h"
#include <cmath>
#include <limits>
#include <QRegularExpression>
#include <QSet>

namespace {

const QSet<QString> KNOWN_FUNCS = {
    "sqrt", "cbrt", "abs", "sin", "cos", "tan", "asin", "acos", "atan",
    "log", "ln", "exp", "floor", "ceil", "round"
};

const QSet<QString> KNOWN_CONSTS = {
    "pi", "π", "e"
};

double getConstantValue(const QString& name) {
    if (name.compare("pi", Qt::CaseInsensitive) == 0 || name == "π") {
        return 3.14159265358979323846;
    }
    if (name.compare("e", Qt::CaseInsensitive) == 0) {
        return 2.71828182845904523536;
    }
    return 0.0;
}

enum class TokenType {
    Number,
    Identifier,
    Plus,
    Minus,
    Multiply,
    Divide,
    Power,
    Modulo,
    OpenParen,
    CloseParen,
    Comma,
    End
};

struct Token {
    TokenType type = TokenType::End;
    double numberValue = 0.0;
    QString stringValue;
};

class Tokenizer {
public:
    Tokenizer(const QString& input) : m_text(input), m_pos(0) {}

    Token nextToken(QString* errorMsg = nullptr) {
        skipWhitespace();

        if (m_pos >= m_text.length()) {
            return {TokenType::End, 0.0, ""};
        }

        QChar c = m_text[m_pos];

        // Operators
        if (c == '+') { m_pos++; return {TokenType::Plus, 0.0, "+"}; }
        if (c == '-') { m_pos++; return {TokenType::Minus, 0.0, "-"}; }
        if (c == '*' || c == QChar(0x00D7)) { m_pos++; return {TokenType::Multiply, 0.0, "*"}; }
        if (c == '/' || c == QChar(0x00F7)) { m_pos++; return {TokenType::Divide, 0.0, "/"}; }
        if (c == '^') { m_pos++; return {TokenType::Power, 0.0, "^"}; }
        if (c == '%') { m_pos++; return {TokenType::Modulo, 0.0, "%"}; }
        if (c == '(') { m_pos++; return {TokenType::OpenParen, 0.0, "("}; }
        if (c == ')') { m_pos++; return {TokenType::CloseParen, 0.0, ")"}; }
        if (c == ',') { m_pos++; return {TokenType::Comma, 0.0, ","}; }

        // Numbers (digits or decimal point)
        if (c.isDigit() || (c == '.' && peekDigit())) {
            int start = m_pos;
            bool hasDot = (c == '.');
            m_pos++;
            while (m_pos < m_text.length()) {
                QChar cur = m_text[m_pos];
                if (cur.isDigit()) {
                    m_pos++;
                } else if (cur == '.' && !hasDot) {
                    hasDot = true;
                    m_pos++;
                } else if ((cur == 'e' || cur == 'E') && m_pos + 1 < m_text.length()) {
                    QChar next = m_text[m_pos + 1];
                    if (next.isDigit() || next == '+' || next == '-') {
                        m_pos += 2;
                        while (m_pos < m_text.length() && m_text[m_pos].isDigit()) {
                            m_pos++;
                        }
                    }
                    break;
                } else {
                    break;
                }
            }
            QString numStr = m_text.mid(start, m_pos - start);
            bool ok = false;
            double val = numStr.toDouble(&ok);
            if (!ok) {
                if (errorMsg) *errorMsg = QString("Número inválido: '%1'").arg(numStr);
                return {TokenType::End, 0.0, ""};
            }
            return {TokenType::Number, val, numStr};
        }

        // Identifier (letters, unicode symbols, underscores)
        if (isIdentifierStart(c)) {
            int start = m_pos;
            m_pos++;
            while (m_pos < m_text.length() && isIdentifierPart(m_text[m_pos])) {
                m_pos++;
            }
            QString id = m_text.mid(start, m_pos - start);
            return {TokenType::Identifier, 0.0, id};
        }

        if (errorMsg) {
            *errorMsg = QString("Caractere desconhecido: '%1'").arg(c);
        }
        m_pos++;
        return {TokenType::End, 0.0, ""};
    }

private:
    void skipWhitespace() {
        while (m_pos < m_text.length() && m_text[m_pos].isSpace()) {
            m_pos++;
        }
    }

    bool peekDigit() const {
        return (m_pos + 1 < m_text.length()) && m_text[m_pos + 1].isDigit();
    }

    static bool isIdentifierStart(QChar c) {
        return c.isLetter() || c == '_' || c.unicode() >= 128;
    }

    static bool isIdentifierPart(QChar c) {
        return c.isLetterOrNumber() || c == '_' || c.unicode() >= 128;
    }

    QString m_text;
    int m_pos;
};

class Parser {
public:
    Parser(Tokenizer& tokenizer, const QMap<QString, double>& vars, QString* error)
        : m_tokenizer(tokenizer), m_vars(vars), m_error(error)
    {
        advance();
    }

    double parse() {
        double result = parseExpression();
        if (m_current.type != TokenType::End && !hasError()) {
            setError(QString("Token inesperado no final da expressão: '%1'").arg(m_current.stringValue));
        }
        return result;
    }

private:
    void advance() {
        m_current = m_tokenizer.nextToken(m_error);
    }

    bool hasError() const {
        return m_error && !m_error->isEmpty();
    }

    void setError(const QString& msg) {
        if (m_error && m_error->isEmpty()) {
            *m_error = msg;
        }
    }

    // Expression: Addition and Subtraction (+, -)
    double parseExpression() {
        double left = parseTerm();
        if (hasError()) return std::numeric_limits<double>::quiet_NaN();

        while (m_current.type == TokenType::Plus || m_current.type == TokenType::Minus) {
            TokenType op = m_current.type;
            advance();
            double right = parseTerm();
            if (hasError()) return std::numeric_limits<double>::quiet_NaN();

            if (op == TokenType::Plus) {
                left += right;
            } else {
                left -= right;
            }
        }
        return left;
    }

    // Term: Multiplication, Division, Modulo (*, /, %)
    double parseTerm() {
        double left = parsePower();
        if (hasError()) return std::numeric_limits<double>::quiet_NaN();

        while (m_current.type == TokenType::Multiply ||
               m_current.type == TokenType::Divide ||
               m_current.type == TokenType::Modulo)
        {
            TokenType op = m_current.type;
            advance();
            double right = parsePower();
            if (hasError()) return std::numeric_limits<double>::quiet_NaN();

            if (op == TokenType::Multiply) {
                left *= right;
            } else if (op == TokenType::Divide) {
                if (std::abs(right) < 1e-15) {
                    setError("Divisão por zero.");
                    return std::numeric_limits<double>::quiet_NaN();
                }
                left /= right;
            } else if (op == TokenType::Modulo) {
                if (std::abs(right) < 1e-15) {
                    setError("Módulo por zero.");
                    return std::numeric_limits<double>::quiet_NaN();
                }
                left = std::fmod(left, right);
            }
        }
        return left;
    }

    // Power: exponentiation (^)
    double parsePower() {
        double left = parseFactor();
        if (hasError()) return std::numeric_limits<double>::quiet_NaN();

        if (m_current.type == TokenType::Power) {
            advance();
            double right = parsePower(); // Right-associative
            if (hasError()) return std::numeric_limits<double>::quiet_NaN();
            return std::pow(left, right);
        }
        return left;
    }

    // Factor: Unary operators, numbers, identifiers, function calls, parentheses
    double parseFactor() {
        if (hasError()) return std::numeric_limits<double>::quiet_NaN();

        // Unary +
        if (m_current.type == TokenType::Plus) {
            advance();
            return parseFactor();
        }

        // Unary -
        if (m_current.type == TokenType::Minus) {
            advance();
            return -parseFactor();
        }

        // Parentheses (expr)
        if (m_current.type == TokenType::OpenParen) {
            advance();
            double val = parseExpression();
            if (m_current.type != TokenType::CloseParen) {
                setError("Parêntese de fechamento ')' ausente.");
                return std::numeric_limits<double>::quiet_NaN();
            }
            advance();
            return val;
        }

        // Numbers
        if (m_current.type == TokenType::Number) {
            double val = m_current.numberValue;
            advance();
            return val;
        }

        // Identifiers (Variables, Functions, Constants)
        if (m_current.type == TokenType::Identifier) {
            QString name = m_current.stringValue;
            advance();

            // Function call: func(arg)
            if (m_current.type == TokenType::OpenParen) {
                advance();
                double arg = parseExpression();
                if (m_current.type != TokenType::CloseParen) {
                    setError(QString("Parêntese ')' ausente na função '%1'").arg(name));
                    return std::numeric_limits<double>::quiet_NaN();
                }
                advance();
                return evaluateFunction(name, arg);
            }

            // Constants (pi, e)
            if (KNOWN_CONSTS.contains(name.toLower()) || name == "π") {
                return getConstantValue(name);
            }

            // Variables
            if (m_vars.contains(name)) {
                return m_vars.value(name);
            }

            // Try case-insensitive lookup
            for (auto it = m_vars.cbegin(); it != m_vars.cend(); ++it) {
                if (it.key().compare(name, Qt::CaseInsensitive) == 0) {
                    return it.value();
                }
            }

            setError(QString("Variável não definida: '%1'").arg(name));
            return std::numeric_limits<double>::quiet_NaN();
        }

        setError(QString("Expressão inválida próxima a '%1'").arg(m_current.stringValue));
        return std::numeric_limits<double>::quiet_NaN();
    }

    double evaluateFunction(const QString& name, double arg) {
        QString lower = name.toLower();
        if (lower == "sqrt") {
            if (arg < 0.0) {
                setError("Raiz quadrada de número negativo.");
                return std::numeric_limits<double>::quiet_NaN();
            }
            return std::sqrt(arg);
        }
        if (lower == "cbrt") return std::cbrt(arg);
        if (lower == "abs") return std::abs(arg);
        if (lower == "sin") return std::sin(arg);
        if (lower == "cos") return std::cos(arg);
        if (lower == "tan") return std::tan(arg);
        if (lower == "asin") {
            if (arg < -1.0 || arg > 1.0) {
                setError("Argumento inválido para asin (deve estar entre -1 e 1).");
                return std::numeric_limits<double>::quiet_NaN();
            }
            return std::asin(arg);
        }
        if (lower == "acos") {
            if (arg < -1.0 || arg > 1.0) {
                setError("Argumento inválido para acos (deve estar entre -1 e 1).");
                return std::numeric_limits<double>::quiet_NaN();
            }
            return std::acos(arg);
        }
        if (lower == "atan") return std::atan(arg);
        if (lower == "log" || lower == "ln") {
            if (arg <= 0.0) {
                setError("Logaritmo de número menor ou igual a zero.");
                return std::numeric_limits<double>::quiet_NaN();
            }
            return (lower == "log") ? std::log10(arg) : std::log(arg);
        }
        if (lower == "exp") return std::exp(arg);
        if (lower == "floor") return std::floor(arg);
        if (lower == "ceil") return std::ceil(arg);
        if (lower == "round") return std::round(arg);

        setError(QString("Função desconhecida: '%1'").arg(name));
        return std::numeric_limits<double>::quiet_NaN();
    }

    Tokenizer& m_tokenizer;
    const QMap<QString, double>& m_vars;
    QString* m_error;
    Token m_current;
};

} // namespace

QString ExpressionEvaluator::extractResultVariable(const QString& fullFormula)
{
    int eqIndex = fullFormula.indexOf('=');
    if (eqIndex > 0) {
        QString left = fullFormula.left(eqIndex).trimmed();
        if (!left.isEmpty()) {
            return left;
        }
    }
    return "R";
}

QString ExpressionEvaluator::extractFormulaExpression(const QString& fullFormula)
{
    int eqIndex = fullFormula.indexOf('=');
    if (eqIndex >= 0 && eqIndex < fullFormula.length() - 1) {
        return fullFormula.mid(eqIndex + 1).trimmed();
    }
    return fullFormula.trimmed();
}

double ExpressionEvaluator::evaluate(const QString& expression,
                                     const QMap<QString, double>& variables,
                                     QString* errorMsg)
{
    QString expr = extractFormulaExpression(expression);
    if (expr.isEmpty()) {
        if (errorMsg) *errorMsg = "Expressão vazia.";
        return std::numeric_limits<double>::quiet_NaN();
    }

    QString internalError;
    Tokenizer tokenizer(expr);
    Parser parser(tokenizer, variables, &internalError);
    double result = parser.parse();

    if (!internalError.isEmpty()) {
        if (errorMsg) *errorMsg = internalError;
        return std::numeric_limits<double>::quiet_NaN();
    }

    if (std::isnan(result) || std::isinf(result)) {
        if (errorMsg && errorMsg->isEmpty()) {
            *errorMsg = "Resultado inválido (NaN ou Infinito).";
        }
        return std::numeric_limits<double>::quiet_NaN();
    }

    return result;
}

bool ExpressionEvaluator::validate(const QString& expression,
                                   const QStringList& definedVariables,
                                   QString* errorMsg)
{
    QMap<QString, double> dummyVars;
    for (const QString& v : definedVariables) {
        dummyVars[v] = 1.0; // Use non-zero dummy value to avoid div by zero during validation
    }

    QString err;
    double res = evaluate(expression, dummyVars, &err);
    Q_UNUSED(res);

    if (!err.isEmpty()) {
        if (errorMsg) *errorMsg = err;
        return false;
    }
    return true;
}

QStringList ExpressionEvaluator::extractVariables(const QString& expression)
{
    QString expr = extractFormulaExpression(expression);
    Tokenizer tokenizer(expr);

    QStringList vars;
    QSet<QString> seen;

    Token token = tokenizer.nextToken();
    while (token.type != TokenType::End) {
        if (token.type == TokenType::Identifier) {
            QString name = token.stringValue;
            QString lower = name.toLower();

            // Check if next token is '(' -> then it's a function call, not a variable
            Tokenizer lookahead = tokenizer;
            Token next = lookahead.nextToken();

            if (next.type != TokenType::OpenParen) {
                if (!KNOWN_CONSTS.contains(lower) && name != "π" && !seen.contains(name)) {
                    vars.append(name);
                    seen.insert(name);
                }
            }
        }
        token = tokenizer.nextToken();
    }

    return vars;
}
