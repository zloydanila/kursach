#ifndef PLAYLISTTRACKS_H
#define PLAYLISTTRACKS_H

#include <QString>

struct PlaylistTrack{

    int playlistId;
    int trackId;
    int position; 

    PlaylistTrack(): playlistId(0), trackId(0), position(0) {}

};



#endif