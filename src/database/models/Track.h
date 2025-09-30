#ifndef TRACk_H
#define TRACK_H

#include <QString>

struct Track{

    int id;
    QString filePath;
    QString title;
    QString artist;
    QString album;
    int duration; //в секундах
    QString fileHash; //для избежания дубликатов

    Track() : id(0), duration(0){}
};



#endif