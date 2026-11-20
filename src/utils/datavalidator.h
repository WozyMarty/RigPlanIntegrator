#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>
#include <QCryptographicHash>
#include <QPair>
#include <functional>

/**
 * @brief Audit log entry recording every data operation.
 */
struct AuditEntry {
    QDateTime timestamp;
    QString operation;
    QString details;
    QString checksumBefore;
    QString checksumAfter;
    bool valid;
};

/**
 * @brief Validated result wrapper ensuring data integrity.
 * 
 * Once created, the value and its checksum are immutable.
 * Any attempt to use a tampered value will be detected.
 */
template<typename T>
class ValidatedResult {
public:
    ValidatedResult() : m_value(T()), m_valid(false) {}

    ValidatedResult(T value, bool valid, const QString& checksum)
        : m_value(value), m_valid(valid), m_checksum(checksum) {}

    T value() const { return m_value; }
    bool isValid() const { return m_valid; }
    QString checksum() const { return m_checksum; }

private:
    T m_value;
    bool m_valid;
    QString m_checksum;
};

/**
 * @brief DataValidator provides safety-critical data validation.
 * 
 * CRITICAL: This class ensures that no data alteration or value
 * inversion occurs during processing, as such anomalies could
 * cause severe field accidents.
 */
class DataValidator : public QObject {
    Q_OBJECT

public:
    explicit DataValidator(QObject* parent = nullptr);

    // --- Range Validation ---
    bool validateRange(double value, double min, double max,
                       const QString& fieldName) const;
    bool validatePositive(double value, const QString& fieldName) const;
    bool validateNonZero(double value, const QString& fieldName) const;

    // --- Checksum ---
    static QString computeChecksum(const QByteArray& data);
    static QString computeChecksum(double value);
    static QString computeChecksum(const QVector<double>& values);

    // --- Value Inversion Detection ---
    bool detectInversion(double valueA, double valueB,
                         double expectedMinA, double expectedMaxA,
                         double expectedMinB, double expectedMaxB,
                         const QString& nameA, const QString& nameB) const;

    // --- Validated Computation ---
    ValidatedResult<double> validateAndCompute(
        const QVector<double>& inputs,
        const QVector<QPair<double, double>>& ranges,
        const QVector<QString>& fieldNames,
        std::function<double(const QVector<double>&)> computation,
        const QString& operationName);

    // --- Audit Log ---
    const QVector<AuditEntry>& auditLog() const { return m_auditLog; }
    void clearAuditLog();
    QString exportAuditLog() const;

signals:
    void validationError(const QString& message);
    void validationWarning(const QString& message);
    void auditLogUpdated();

private:
    void addAuditEntry(const QString& operation, const QString& details,
                       const QString& checksumBefore, const QString& checksumAfter,
                       bool valid);

    QVector<AuditEntry> m_auditLog;
};
