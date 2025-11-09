#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "models/User.h"
#include "models/Track.h"
#include "models/Playlist.h"
#include "models/PlaylistTracks.h"

class DatabaseManager
{
private:
    QSqlDatabase m_database;
    static DatabaseManager* m_instance;

    DatabaseManager() = default;
    ~DatabaseManager() = default;

public:
    static DatabaseManager& instance();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool initializeDatabase();
    bool isDatabaseOpen() const;

    // Пользователи
    bool registerUser(const QString& username, const QString& password);
    bool authenticateUser(const QString& username, const QString& password);
    bool userExists(const QString& username);
    int getUserId(const QString& username);

    // Треки
    bool addTrackMetadata(const QString& fileHash, const QString& title, 
                         const QString& artist, const QString& album, 
                         int duration, const QString& genre, int year,
                         int bitrate, int sampleRate);
    
    bool addTrackToUser(const QString& filePath, const QString& fileHash,
                       int userId, const QString& title, const QString& artist,
                       const QString& album, int duration);
    
    bool addTrackFromAPI(int userId, const QString& title, const QString& artist, 
                        const QString& album, int duration, 
                        const QString& genre, const QString& coverUrl);
    
    bool addTrackToUserLibrary(int userId, const QString& title, const QString& artist, 
                              const QString& album, int duration, const QString& coverUrl);
    
    QList<Track> getUserTracks(int userId); // ОДНО объявление

    // Плейлисты
    int createPlaylist(int userId, const QString& name);
    bool addTrackToPlaylist(int playlistId, int trackId, int position);
    QList<Playlist> getUserPlaylists(int userId);
    QList<Track> getPlaylistTracks(int playlistId);

    // Статистика
    bool incrementPlayCount(int trackId);

private:
    bool createUsersTable();
    bool createTracksMetadataTable();
    bool createTracksTable();
    bool createPlaylistsTable();
    bool createPlaylistTracksTable();
    bool createFriendsTable();
    bool createMessagesTable();
    bool updateFriendsTable();
    QString hashPassword(const QString& password);
};

#endif // DATABASEMANAGER_H