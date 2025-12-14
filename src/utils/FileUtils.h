#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>
#include <QStringList>

class FileUtils
{
public:
    static bool fileExists(const QString& path);
    static qint64 fileSize(const QString& path);
    static QString fileExtension(const QString& path);
    static QString fileName(const QString& path);
    static QString fileBaseName(const QString& path);
    
    static bool createDirectory(const QString& path);
    static bool deleteFile(const QString& path);
    static bool copyFile(const QString& source, const QString& destination);
    static bool moveFile(const QString& source, const QString& destination);
    
    static QStringList listFiles(const QString& directory, const QStringList& filters = QStringList());
    static QString readTextFile(const QString& path);
    static bool writeTextFile(const QString& path, const QString& content);
    
    static QString generateUniqueFileName(const QString& baseName, const QString& extension);
    static QString sanitizeFileName(const QString& name);
    
    static qint64 directorySize(const QString& path);
    static bool cleanDirectory(const QString& path);
};

#endif
