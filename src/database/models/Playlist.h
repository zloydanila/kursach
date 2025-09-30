#ifndef PLAYLIST_H
#define PLAYLUST_H

#include <QString>

struct Playlist{

    int id;
    int userId;
    QString name;
    QString createdAt;

    Playlist(): id(0), userId(0){}

};



#endif