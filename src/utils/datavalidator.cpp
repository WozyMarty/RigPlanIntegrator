#include "datavalidator.h"
#include "constants.h"
#include <QTextStream>
#include <cmath>

DataValidator::DataValidator(QObject* parent)
    : QObject(parent)
{
}

bool DataValidator::validateRange(double value, double min, double max,
                                   const QString& fieldName) const
{
    if (std::isnan(value) || std::isinf(value)) {
        emit const_cast<DataValidator*>(this)->validationError(
            tr("Campo '%1': valor inválido (NaN/Inf).").arg(fieldName));
        return false;
    }
    if (value < min || value > max) {
        emit const_cast<DataValidator*>(this)->validationError(
            tr("Campo '%1': valor %2 fora do intervalo [%3, %4].")
                .arg(fieldName)
                .arg(value, 0, 'g', 6)
                .arg(min, 0, 'g', 6)
                .arg(max, 0, 'g', 6));
        return false;
    }
    return true;
}

bool DataValidator::validatePositive(double value, const QString& fieldName) const
{
    if (value <= 0.0) {
        emit const_cast<DataValidator*>(this)->validationError(
            tr("Campo '%1': valor deve ser positivo (recebido: %2).")
                .arg(fieldName)
                .arg(value, 0, 'g', 6));
        return false;
    }
    return validateRange(value, Constants::EPSILON, Constants::MAX_VALID_VALUE, fieldName);
}

bool DataValidator::validateNonZero(double value, const QString& fieldName) const
{
    if (std::abs(value) < Constants::EPSILON) {
        emit const_cast<DataValidator*>(this)->validationError(
            tr("Campo '%1': valor não pode ser zero.").arg(fieldName));
        return false;
    }
    return true;
}

QString DataValidator::computeChecksum(const QByteArray& data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}

QString DataValidator::computeChecksum(double value)
{
    QByteArray data;
    data.append(reinterpret_cast<const char*>(&value), sizeof(double));
    return computeChecksum(data);
}

QString DataValidator::computeChecksum(const QVector<double>& values)
{
    QByteArray data;
    for (double v : values) {
        data.append(reinterpret_cast<const char*>(&v), sizeof(double));
    }
    return computeChecksum(data);
}

bool DataValidator::detectInversion(double valueA, double valueB,
                                     double expectedMinA, double expectedMaxA,
                                     double expectedMinB, double expectedMaxB,
                                     const QString& nameA, const QString& nameB) const
{
    // Check if values seem swapped based on expected ranges
    bool aInRangeA = (valueA >= expectedMinA && valueA <= expectedMaxA);
    bool bInRangeB = (valueB >= expectedMinB && valueB <= expectedMaxB);
    bool aInRangeB = (valueA >= expectedMinB && valueA <= expectedMaxB);
    bool bInRangeA = (valueB >= expectedMinA && valueB <= expectedMaxA);

    if (!aInRangeA && !bInRangeB && aInRangeB && bInRangeA) {
        emit const_cast<DataValidator*>(this)->validationWarning(
            tr("ATENÇÃO: Possível inversão detectada entre '%1' (%2) e '%3' (%4). "
               "Verifique os valores!")
                .arg(nameA).arg(valueA, 0, 'g', 6)
                .arg(nameB).arg(valueB, 0, 'g', 6));
        return true; // Inversion detected
    }
    return false; // No inversion
}

ValidatedResult<double> DataValidator::validateAndCompute(
    const QVector<double>& inputs,
    const QVector<QPair<double, double>>& ranges,
    const QVector<QString>& fieldNames,
    std::function<double(const QVector<double>&)> computation,
    const QString& operationName)
{
    QString checksumBefore = computeChecksum(inputs);

    // Validate all inputs
    for (int i = 0; i < inputs.size(); ++i) {
        if (i < ranges.size() && i < fieldNames.size()) {
            if (!validateRange(inputs[i], ranges[i].first, ranges[i].second, fieldNames[i])) {
                addAuditEntry(operationName, "Validação falhou: " + fieldNames[i],
                              checksumBefore, "", false);
                return ValidatedResult<double>(0.0, false, "");
            }
        }
    }

    // Compute result
    double result = computation(inputs);

    // Validate result
    if (std::isnan(result) || std::isinf(result)) {
        addAuditEntry(operationName, "Resultado inválido (NaN/Inf)",
                      checksumBefore, "", false);
        emit validationError(
            tr("Cálculo '%1': resultado inválido.").arg(operationName));
        return ValidatedResult<double>(0.0, false, "");
    }

    // Verify input integrity (checksum unchanged)
    QString checksumAfter = computeChecksum(inputs);
    if (checksumBefore != checksumAfter) {
        addAuditEntry(operationName, "ALERTA CRÍTICO: Dados alterados durante cálculo!",
                      checksumBefore, checksumAfter, false);
        emit validationError(
            tr("ERRO CRÍTICO: Dados alterados durante o cálculo '%1'! Operação abortada.")
                .arg(operationName));
        return ValidatedResult<double>(0.0, false, "");
    }

    QString resultChecksum = computeChecksum(result);
    addAuditEntry(operationName,
                  QString("OK - Resultado: %1").arg(result, 0, 'g', 10),
                  checksumBefore, resultChecksum, true);

    return ValidatedResult<double>(result, true, resultChecksum);
}

void DataValidator::addAuditEntry(const QString& operation, const QString& details,
                                   const QString& checksumBefore, const QString& checksumAfter,
                                   bool valid)
{
    AuditEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.operation = operation;
    entry.details = details;
    entry.checksumBefore = checksumBefore;
    entry.checksumAfter = checksumAfter;
    entry.valid = valid;
    m_auditLog.append(entry);
    emit auditLogUpdated();
}

void DataValidator::clearAuditLog()
{
    m_auditLog.clear();
    emit auditLogUpdated();
}

QString DataValidator::exportAuditLog() const
{
    QString log;
    QTextStream stream(&log);
    stream << "=== LOG DE AUDITORIA - RigPlan Integrator ===\n";
    stream << "Exportado em: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
    stream << "Total de entradas: " << m_auditLog.size() << "\n";
    stream << "================================================\n\n";

    for (const auto& entry : m_auditLog) {
        stream << "[" << entry.timestamp.toString("yyyy-MM-dd HH:mm:ss.zzz") << "] ";
        stream << (entry.valid ? "✓" : "✗") << " ";
        stream << entry.operation << " - " << entry.details << "\n";
        if (!entry.checksumBefore.isEmpty()) {
            stream << "  Checksum IN:  " << entry.checksumBefore.left(16) << "...\n";
        }
        if (!entry.checksumAfter.isEmpty()) {
            stream << "  Checksum OUT: " << entry.checksumAfter.left(16) << "...\n";
        }
        stream << "\n";
    }

    return log;
}
