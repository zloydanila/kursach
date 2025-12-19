#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QString>

struct Playlist {
    int id = -1;
    int userId = -1;
    QString name;
    QString createdAt;
};

#endif
