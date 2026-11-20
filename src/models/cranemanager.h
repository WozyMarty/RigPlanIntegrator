#pragma once

#include <QObject>
#include <QVector>
#include "craneprofile.h"

/**
 * @brief Gerenciador persistente de Guindastes registrados / utilizados
 */
class CraneManager : public QObject {
    Q_OBJECT

public:
    static CraneManager* instance();

    explicit CraneManager(QObject* parent = nullptr);

    void initialize();

    QVector<CraneProfile> allCranes() const { return m_cranes; }
    CraneProfile craneById(const QString& id) const;

    void addCrane(const CraneProfile& crane);
    void updateCrane(const CraneProfile& crane);
    void removeCrane(const QString& id);

    void saveAll();
    void loadAll();

signals:
    void cranesChanged();

private:
    QString dataDirectory() const;
    QString catalogFilePath() const;
    void ensureDefaults();

    static CraneManager* s_instance;
    QVector<CraneProfile> m_cranes;
};
