#include "FileUtils.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QDirIterator>

bool FileUtils::fileExists(const QString& path)
{
    return QFile::exists(path);
}

qint64 FileUtils::fileSize(const QString& path)
{
    QFileInfo info(path);
    return info.size();
}

QString FileUtils::fileExtension(const QString& path)
{
    QFileInfo info(path);
    return info.suffix();
}

QString FileUtils::fileName(const QString& path)
{
    QFileInfo info(path);
    return info.fileName();
}

QString FileUtils::fileBaseName(const QString& path)
{
    QFileInfo info(path);
    return info.baseName();
}

bool FileUtils::createDirectory(const QString& path)
{
    QDir dir;
    return dir.mkpath(path);
}

bool FileUtils::deleteFile(const QString& path)
{
    return QFile::remove(path);
}

bool FileUtils::copyFile(const QString& source, const QString& destination)
{
    if (fileExists(destination)) {
        QFile::remove(destination);
    }
    return QFile::copy(source, destination);
}

bool FileUtils::moveFile(const QString& source, const QString& destination)
{
    if (copyFile(source, destination)) {
        return deleteFile(source);
    }
    return false;
}

QStringList FileUtils::listFiles(const QString& directory, const QStringList& filters)
{
    QStringList files;
    QDir dir(directory);
    
    if (!dir.exists()) {
        return files;
    }
    
    QFileInfoList fileList;
    if (filters.isEmpty()) {
        fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    } else {
        fileList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);
    }
    
    for (const QFileInfo& info : fileList) {
        files.append(info.absoluteFilePath());
    }
    
    return files;
}

QString FileUtils::readTextFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    
    return content;
}

bool FileUtils::writeTextFile(const QString& path, const QString& content)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out << content;
    file.close();
    
    return true;
}

QString FileUtils::generateUniqueFileName(const QString& baseName, const QString& extension)
{
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    return QString("%1_%2.%3").arg(baseName, timestamp, extension);
}

QString FileUtils::sanitizeFileName(const QString& name)
{
    QString sanitized = name;
    sanitized.replace(QRegExp("[<>:\"/\\|?*]"), "_");
    return sanitized;
}

qint64 FileUtils::directorySize(const QString& path)
{
    qint64 totalSize = 0;
    QDirIterator it(path, QDir::Files, QDirIterator::Subdirectories);
    
    while (it.hasNext()) {
        it.next();
        totalSize += it.fileInfo().size();
    }
    
    return totalSize;
}

bool FileUtils::cleanDirectory(const QString& path)
{
    QDir dir(path);
    
    if (!dir.exists()) {
        return false;
    }
    
    bool success = true;
    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    
    for (const QFileInfo& file : files) {
        if (!QFile::remove(file.absoluteFilePath())) {
            success = false;
        }
    }
    
    return success;
}
