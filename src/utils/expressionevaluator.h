#pragma once

#include <QString>
#include <QStringList>
#include <QMap>

/**
 * @brief Mathematical expression parser and evaluator.
 *
 * Supports arithmetic (+, -, *, /, ^, %), parentheses, common functions
 * (sqrt, sin, cos, tan, abs, log, ln, exp), constants (pi, e), and arbitrary
 * variable identifiers including unicode symbols (such as ΔS, Δt, θ, etc.).
 */
class ExpressionEvaluator {
public:
    /**
     * @brief Evaluates an expression with the given variable substitutions.
     * @param expression The mathematical expression (e.g. "dS / dt" or "Vm = dS / dt").
     * @param variables Map of variable names/symbols to their numeric values.
     * @param errorMsg Optional pointer to receive error description if evaluation fails.
     * @return Calculated value, or NaN on error.
     */
    static double evaluate(const QString& expression,
                           const QMap<QString, double>& variables,
                           QString* errorMsg = nullptr);

    /**
     * @brief Checks if a formula is syntactically valid and all variables are accounted for.
     */
    static bool validate(const QString& expression,
                         const QStringList& definedVariables,
                         QString* errorMsg = nullptr);

    /**
     * @brief Extracts variable names from an expression, excluding functions and constants.
     */
    static QStringList extractVariables(const QString& expression);

    /**
     * @brief If formula has an assignment (e.g. "Vm = ΔS / Δt"), returns "Vm".
     * Otherwise returns "R".
     */
    static QString extractResultVariable(const QString& fullFormula);

    /**
     * @brief If formula has an assignment (e.g. "Vm = ΔS / Δt"), returns the right-hand side "ΔS / Δt".
     * Otherwise returns the full formula trimmed.
     */
    static QString extractFormulaExpression(const QString& fullFormula);
};
