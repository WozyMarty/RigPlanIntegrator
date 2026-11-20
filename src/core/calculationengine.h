#pragma once

#include <QObject>
#include <QVector>
#include <QMap>
#include <QSet>
#include "models/riggingcalculation.h"
#include "models/calculationpreset.h"
#include "utils/datavalidator.h"

/**
 * @brief Engine that manages all calculations, custom additions, active selection, and presets.
 *
 * Registers calculations (16 built-in + user custom calculations),
 * manages active subset of calculations, handles presets, evaluates formulas,
 * propagates dependencies, and validates input data.
 */
class CalculationEngine : public QObject {
    Q_OBJECT

public:
    explicit CalculationEngine(QObject* parent = nullptr);
    ~CalculationEngine();

    // --- Initialization ---
    void initialize();
    void initializeDefaultCalculations();

    // --- Access ---
    // Returns currently active calculations (compatible with Preview & Exporters)
    QVector<RiggingCalculation*> calculations() const { return activeCalculations(); }
    QVector<RiggingCalculation*> activeCalculations() const;
    QVector<RiggingCalculation*> allCalculations() const { return m_calculations; }
    RiggingCalculation* calculationById(int id) const;
    int calculationCount() const { return activeCalculations().size(); }
    int totalAvailableCount() const { return m_calculations.size(); }
    DataValidator* validator() const { return m_validator; }

    // --- Active Calculations Selection ---
    QVector<int> activeCalculationIds() const;
    void setActiveCalculations(const QVector<int>& ids);
    bool isCalculationActive(int id) const;
    void setCalculationActive(int id, bool active);

    // --- Calculation Presets ---
    QVector<CalculationPreset> calculationPresets() const { return m_presets; }
    CalculationPreset presetById(const QString& id) const;
    CalculationPreset currentPreset() const;
    QString currentPresetId() const { return m_currentPresetId; }
    void applyPreset(const QString& presetId);
    void savePreset(const CalculationPreset& preset);
    void deletePreset(const QString& presetId);

    // --- Custom Calculations ---
    RiggingCalculation* addCustomCalculation(const QString& title,
                                            const QString& formula,
                                            const QVector<CalcParameter>& params,
                                            const QString& resultUnit,
                                            const QString& description = "");
    void removeCustomCalculation(int id);
    void setupCustomCalculationCompute(RiggingCalculation* calc);

    // --- Input ---
    void setParameterValue(int calcId, int paramIndex, double value);

    // --- Reorder ---
    void moveCalculation(int fromIndex, int toIndex);

    // --- State ---
    int completedCount() const;
    int activeCount() const { return activeCalculations().size(); }
    int totalCount() const { return activeCalculations().size(); }
    bool allCompleted() const { return completedCount() == activeCount() && activeCount() > 0; }

    // --- Bulk Operations ---
    void recalculateAll();
    void collapseAll();
    void expandAll();
    void clearAll();

    // --- Serialization ---
    QByteArray serializeAll() const;

signals:
    void calculationUpdated(int id, double result);
    void progressChanged(int completed, int total);
    void allCalculationsReady();
    void validationError(const QString& message);

    // Preset & Selection signals
    void activeCalculationsChanged();
    void calculationPresetsChanged();
    void presetApplied(const CalculationPreset& preset);
    void customCalculationAdded(RiggingCalculation* calc);
    void customCalculationRemoved(int id);

private slots:
    void onResultChanged(int calcId, double result);

private:
    void registerCalculation(RiggingCalculation* calc);
    void setupDependencies();
    void propagateDependency(int sourceCalcId, double result);

    // Disk persistence
    QString dataDirectory() const;
    void loadCustomCalculations();
    void saveCustomCalculations();
    void loadPresets();
    void savePresets();
    void ensureDefaultPresets();

    QVector<RiggingCalculation*> m_calculations;
    QMap<int, RiggingCalculation*> m_calcMap;
    QSet<int> m_activeIds;

    // Presets
    QVector<CalculationPreset> m_presets;
    QString m_currentPresetId;

    // Dependencies: key = source calc id, value = list of (target calc id, target param index)
    QMultiMap<int, QPair<int, int>> m_dependencies;

    DataValidator* m_validator;
};
