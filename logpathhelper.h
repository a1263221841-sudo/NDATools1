#ifndef LOGPATHHELPER_H
#define LOGPATHHELPER_H

#include <QDir>
#include <QFileInfo>
#include <QString>

inline QString projectRootPath()
{
    return QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();
}

inline QString projectLogDirPath()
{
    return QDir(projectRootPath()).filePath(QStringLiteral("logs"));
}

inline QString projectLogDirPath(const QString &subDirName)
{
    if (subDirName.trimmed().isEmpty()) {
        return projectLogDirPath();
    }
    return QDir(projectLogDirPath()).filePath(subDirName);
}

inline QString ensureProjectLogDirPath(const QString &subDirName = QString())
{
    const QString logDir = projectLogDirPath(subDirName);
    QDir().mkpath(logDir);
    return logDir;
}

inline QString projectLogFilePath(const QString &fileName, const QString &subDirName = QString())
{
    return QDir(ensureProjectLogDirPath(subDirName)).filePath(fileName);
}

#endif // LOGPATHHELPER_H
