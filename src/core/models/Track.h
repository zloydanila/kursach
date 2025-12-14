#ifndef TRACK_H
#define TRACK_H

#include <QString>
#include <QPixmap>
#include <QDateTime>

struct TrackData
{
    int id = -1;

    QString filePath;
    QString fileHash;

    QString title;
    QString artist;
    QString album;
    int duration = 0;
    int playCount = 0;
    QString addedAt;
    QString genre;
    int year = 0;
    int bitrate = 0;
    int sampleRate = 0;
    int position = 0;

    QString yandexId;
    QString streamUrl;
    QString previewUrl;

    QString lastFmUrl;
    QString mbid;

    QPixmap cover;
    QString lyrics;

    bool isFromCloud = false;
    QString source;
};

Q_DECLARE_METATYPE(TrackData)

#endif
