#ifndef TRACK_H
#define TRACK_H

#include <QString>

struct TrackData {  // ← измените имя
    int id;
    QString filePath;
    QString fileHash;
    QString title;
    QString artist;
    QString album;
    int duration;
    int playCount;
    QString addedAt;
    QString genre;
    int year;
    int bitrate;
    int sampleRate;
    int position;
};

#endif // TRACK_H