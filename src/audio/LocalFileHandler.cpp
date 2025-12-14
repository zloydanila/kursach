#include "LocalFileHandler.h"
#include "database/DatabaseManager.h"
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QDebug>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>


LocalFileHandler::LocalFileHandler(QObject *parent)
    : QObject(parent)
{
    m_supportedFormats << "mp3" << "wav" << "flac" << "ogg" << "m4a" << "aac";
}


QStringList LocalFileHandler::getSupportedFormats() const
{
    return m_supportedFormats;
}


bool LocalFileHandler::addTrackToDatabase(int userId, const QString& filePath)
{
    if (!isValidAudioFile(filePath)) {
        return false;
    }
    
    TrackMetadata metadata = extractMetadata(filePath);
    
    return DatabaseManager::instance().addTrack(
        filePath,
        metadata.title,
        metadata.artist,
        metadata.album,
        metadata.duration,
        userId
    );
}


bool LocalFileHandler::scanDirectory(int userId, const QString& directoryPath)
{
    QDir dir(directoryPath);
    if (!dir.exists()) {
        return false;
    }
    
    QStringList filters;
    for (const QString& format : m_supportedFormats) {
        filters << "*." + format;
    }
    
    QDirIterator it(directoryPath, filters, QDir::Files, QDirIterator::Subdirectories);
    
    int addedCount = 0;
    while (it.hasNext()) {
        QString filePath = it.next();
        if (addTrackToDatabase(userId, filePath)) {
            addedCount++;
        }
    }
    
    return addedCount > 0;
}


LocalFileHandler::TrackMetadata LocalFileHandler::extractMetadata(const QString& filePath)
{
    TrackMetadata metadata;
    QFileInfo fileInfo(filePath);
    
    metadata.title = fileInfo.completeBaseName();
    metadata.artist = "Unknown Artist";
    metadata.album = "Unknown Album";
    metadata.duration = 0;
    metadata.genre = "";
    metadata.year = 0;
    
    TagLib::FileRef file(filePath.toStdString().c_str());
    
    if (!file.isNull() && file.tag()) {
        TagLib::Tag *tag = file.tag();
        
        if (!tag->title().isEmpty()) {
            metadata.title = QString::fromStdString(tag->title().to8Bit(true));
        }
        if (!tag->artist().isEmpty()) {
            metadata.artist = QString::fromStdString(tag->artist().to8Bit(true));
        }
        if (!tag->album().isEmpty()) {
            metadata.album = QString::fromStdString(tag->album().to8Bit(true));
        }
        if (!tag->genre().isEmpty()) {
            metadata.genre = QString::fromStdString(tag->genre().to8Bit(true));
        }
        metadata.year = tag->year();
        
        if (file.audioProperties()) {
            metadata.duration = file.audioProperties()->lengthInSeconds();
        }
    }
    
    return metadata;
}


bool LocalFileHandler::isValidAudioFile(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    
    if (!fileInfo.exists()) {
        return false;
    }
    
    QString extension = fileInfo.suffix().toLower();
    return m_supportedFormats.contains(extension);
}
