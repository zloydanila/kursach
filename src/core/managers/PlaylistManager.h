#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QVector>
#include "core/models/Playlist.h"
#include "core/models/Track.h"

class PlaylistManager : public QObject
{
    Q_OBJECT

public:
    explicit PlaylistManager(int userId, QObject *parent = nullptr);
    
    int createPlaylist(const QString& name, const QString& description = QString());
    bool deletePlaylist(int playlistId);
    bool renamePlaylist(int playlistId, const QString& newName);
    bool addTrackToPlaylist(int playlistId, int trackId);
    bool removeTrackFromPlaylist(int playlistId, int trackId);
    bool reorderTracks(int playlistId, const QVector<int>& trackIds);
    
    QVector<Playlist> getUserPlaylists();
    QVector<TrackData> getPlaylistTracks(int playlistId);
    Playlist getPlaylist(int playlistId);

signals:
    void playlistCreated(int playlistId);
    void playlistDeleted(int playlistId);
    void playlistUpdated(int playlistId);
    void trackAdded(int playlistId, int trackId);
    void trackRemoved(int playlistId, int trackId);

private:
    int m_userId;
};

#endif
