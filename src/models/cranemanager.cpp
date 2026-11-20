#include "cranemanager.h"
#include "../utils/appdatapath.h"
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

CraneManager* CraneManager::s_instance = nullptr;

CraneManager* CraneManager::instance()
{
    if (!s_instance) {
        s_instance = new CraneManager();
        s_instance->initialize();
    }
    return s_instance;
}

CraneManager::CraneManager(QObject* parent)
    : QObject(parent)
{
}

void CraneManager::initialize()
{
    loadAll();
    ensureDefaults();
}

CraneProfile CraneManager::craneById(const QString& id) const
{
    for (const auto& c : m_cranes) {
        if (c.id == id) return c;
    }
    if (!m_cranes.isEmpty()) return m_cranes.first();
    return CraneProfile();
}

void CraneManager::addCrane(const CraneProfile& crane)
{
    CraneProfile c = crane;
    if (c.id.isEmpty()) {
        c.id = QString("crane_%1").arg(QDateTime::currentMSecsSinceEpoch());
    }
    m_cranes.append(c);
    saveAll();
    emit cranesChanged();
}

void CraneManager::updateCrane(const CraneProfile& crane)
{
    for (int i = 0; i < m_cranes.size(); ++i) {
        if (m_cranes[i].id == crane.id) {
            m_cranes[i] = crane;
            saveAll();
            emit cranesChanged();
            return;
        }
    }
    // If not found, add it
    addCrane(crane);
}

void CraneManager::removeCrane(const QString& id)
{
    for (int i = 0; i < m_cranes.size(); ++i) {
        if (m_cranes[i].id == id) {
            m_cranes.removeAt(i);
            saveAll();
            emit cranesChanged();
            return;
        }
    }
}

QString CraneManager::dataDirectory() const
{
    QString appData = applicationDataLocation();
    return appData + "/cranes";
}

QString CraneManager::catalogFilePath() const
{
    return dataDirectory() + "/catalog.json";
}

void CraneManager::saveAll()
{
    QDir dir(dataDirectory());
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QJsonArray arr;
    for (const auto& c : m_cranes) {
        arr.append(c.toJson());
    }

    QJsonObject root;
    root["cranes"] = arr;

    QFile file(catalogFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        file.close();
    }
}

void CraneManager::loadAll()
{
    m_cranes.clear();
    QFile file(catalogFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        if (doc.isObject()) {
            QJsonArray arr = doc.object()["cranes"].toArray();
            for (const auto& val : arr) {
                if (val.isObject()) {
                    m_cranes.append(CraneProfile::fromJson(val.toObject()));
                }
            }
        }
    }
}

void CraneManager::ensureDefaults()
{
    if (m_cranes.isEmpty()) {
        m_cranes = CraneProfile::createDefaultCatalog();
        saveAll();
    }
}
