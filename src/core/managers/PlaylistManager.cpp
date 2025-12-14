#include "PlaylistManager.h"
#include "database/DatabaseManager.h"

PlaylistManager::PlaylistManager(int userId, QObject *parent)
    : QObject(parent), m_userId(userId)
{
}

QVector<Playlist> PlaylistManager::getUserPlaylists()
{
    return QVector<Playlist>();
}

Playlist PlaylistManager::getPlaylist(int playlistId)
{
    Playlist playlist;
    playlist.id = playlistId;
    return playlist;
}

int PlaylistManager::createPlaylist(const QString& name, const QString& description)
{
    Q_UNUSED(description);
    Q_UNUSED(name);
    return -1;
}

QVector<TrackData> PlaylistManager::getPlaylistTracks(int playlistId)
{
    Q_UNUSED(playlistId);
    return QVector<TrackData>();
}

bool PlaylistManager::removeTrackFromPlaylist(int playlistId, int trackId)
{
    Q_UNUSED(playlistId);
    Q_UNUSED(trackId);
    return false;
}
