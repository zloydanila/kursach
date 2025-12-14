#ifndef LOCALFILEHANDLER_H
#define LOCALFILEHANDLER_H

#include <QObject>
#include <QString>
#include <QStringList>

class LocalFileHandler : public QObject
{
    Q_OBJECT

public:
    explicit LocalFileHandler(QObject *parent = nullptr);
    
    QStringList getSupportedFormats() const;
    bool addTrackToDatabase(int userId, const QString& filePath);
    bool scanDirectory(int userId, const QString& directoryPath);

private:
    struct TrackMetadata {
        QString title;
        QString artist;
        QString album;
        int duration;
        QString genre;
        int year;
    };
    
    TrackMetadata extractMetadata(const QString& filePath);
    bool isValidAudioFile(const QString& filePath);
    
    QStringList m_supportedFormats;
};

#endif
