#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>

#include "models/Track.h"
#include "models/Playlist.h"

class DatabaseManager
{
public:

    static DatabaseManager& instance();
    
    bool initializeDatabase();
    bool isDatabaseOpen() const;

    QString hashPassword(const QString& password);
    bool registerUser(const QString& username, const QString& password);
    bool authenticateUser(const QString& username, const QString& password);
    bool userExists(const QString& username);
    int getUserId(const QString& username);
    

    bool addTrackMetadata(const QString& fileHash, const QString& title, 
                         const QString& artist, const QString& album, 
                         int duration, const QString& genre = "", int year = 0,
                         int bitrate = 0, int sampleRate = 0);
    bool addTrackToUser(const QString& filePath, const QString& fileHash,
                       int userId, const QString& title, const QString& artist,
                       const QString& album, int duration);
    QList<Track> getUserTracks(int userId);
    bool incrementPlayCount(int trackId);

    int createPlaylist(int userId, const QString& name);
    bool addTrackToPlaylist(int playlistId, int trackId, int position = -1);
    QList<Playlist> getUserPlaylists(int userId);
    QList<Track> getPlaylistTracks(int playlistId);
    bool addTrackFromAPI(int userId, const QString& title, const QString& artist, 
                        const QString& album = "", int duration = 0, 
                        const QString& genre = "", const QString& coverUrl = "");

private:

    DatabaseManager() = default;
    ~DatabaseManager() = default;

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    QSqlDatabase m_database;

    bool createUsersTable();
    bool createTracksMetadataTable();
    bool createTracksTable();
    bool createPlaylistsTable();
    bool createPlaylistTracksTable();
    bool createFriendsTable();

};

#endif 