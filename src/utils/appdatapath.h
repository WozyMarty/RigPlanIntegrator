#pragma once

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

inline QString applicationDataLocation()
{
    const QString portableData = QDir(QCoreApplication::applicationDirPath())
                                     .filePath(QStringLiteral("portable_data"));
    if (QDir(portableData).exists()) {
        return portableData;
    }

    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}