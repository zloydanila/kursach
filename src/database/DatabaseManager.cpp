#include "DatabaseManager.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDateTime>

QMutex DatabaseManager::m_mutex;

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject *parent) 
    : QObject(parent), m_initialized(false) {}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::initialize() {
    QMutexLocker locker(&m_mutex);
    
    if (m_initialized && m_db.isOpen()) {
        return true;
    }
    
    return openConnection();
}

bool DatabaseManager::openConnection() {
    if (m_db.isOpen()) {
        m_db.close();
    }
    
    m_db = QSqlDatabase::addDatabase("QPSQL", "soundspace_connection");
    
    if (!m_db.isValid()) {
        qDebug() << "PostgreSQL driver unavailable";
        return false;
    }
    
    m_db.setHostName("turntable.proxy.rlwy.net");
    m_db.setPort(26533);
    m_db.setDatabaseName("railway");
    m_db.setUserName("postgres");
    m_db.setPassword("VAIWhuipzWQGYBXtLfvtIqXfKyfkxXQS");
    m_db.setConnectOptions("connect_timeout=10");
    
    if (!m_db.open()) {
        qDebug() << "Connection error:" << m_db.lastError().text();
        return false;
    }
    
    if (!createTables()) {
        qDebug() << "Table creation error";
        return false;
    }
    
    m_initialized = true;
    return true;
}

bool DatabaseManager::createTables() {
    if (!m_db.isOpen()) {
        return false;
    }
    
    bool success = true;
    
    QString createUsers = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id SERIAL PRIMARY KEY, "
        "username VARCHAR(50) UNIQUE NOT NULL, "
        "password_hash VARCHAR(64) NOT NULL, "
        "email VARCHAR(100), "
        "avatar_path TEXT, "
        "status INTEGER DEFAULT 0, "
        "bio TEXT, "
        "last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
    
    QSqlQuery query(m_db);
    if (!query.exec(createUsers)) {
        qDebug() << "users error:" << query.lastError().text();
        success = false;
    }
    
    QString createTracks = 
        "CREATE TABLE IF NOT EXISTS tracks ("
        "id SERIAL PRIMARY KEY, "
        "file_path TEXT NOT NULL, "
        "file_hash VARCHAR(64) NOT NULL, "
        "user_id INTEGER NOT NULL, "
        "title TEXT NOT NULL, "
        "artist TEXT NOT NULL, "
        "album TEXT, "
        "duration INTEGER DEFAULT 0, "
        "play_count INTEGER DEFAULT 0, "
        "added_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "cover_url TEXT, "
        "genre TEXT, "
        "year INTEGER, "
        "bitrate INTEGER, "
        "sample_rate INTEGER)";

    if (!query.exec(createTracks)) {
        qDebug() << "tracks error:" << query.lastError().text();
        success = false;
    }
    
    QString createRadioStations = 
        "CREATE TABLE IF NOT EXISTS radio_stations ("
        "id SERIAL PRIMARY KEY, "
        "user_id INTEGER NOT NULL, "
        "name VARCHAR(255) NOT NULL, "
        "country VARCHAR(100), "
        "stream_url TEXT NOT NULL, "
        "genre VARCHAR(100), "
        "bitrate INTEGER DEFAULT 128, "
        "favicon TEXT, "
        "play_count INTEGER DEFAULT 0, "
        "is_favorite BOOLEAN DEFAULT false, "
        "added_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
    
    if (!query.exec(createRadioStations)) {
        qDebug() << "radio_stations error:" << query.lastError().text();
        success = false;
    }
    
    QString createRadioShares = 
        "CREATE TABLE IF NOT EXISTS radio_shares ("
        "id SERIAL PRIMARY KEY, "
        "station_id INTEGER NOT NULL, "
        "shared_by INTEGER NOT NULL, "
        "shared_with INTEGER NOT NULL, "
        "shared_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
    
    if (!query.exec(createRadioShares)) {
        qDebug() << "radio_shares error:" << query.lastError().text();
        success = false;
    }
    
    QString createRadioStatus = 
        "CREATE TABLE IF NOT EXISTS radio_status ("
        "id SERIAL PRIMARY KEY, "
        "user_id INTEGER NOT NULL, "
        "station_id INTEGER NOT NULL, "
        "status_text TEXT, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
    
    if (!query.exec(createRadioStatus)) {
        qDebug() << "radio_status error:" << query.lastError().text();
        success = false;
    }
    
    QString createPlaylists = 
        "CREATE TABLE IF NOT EXISTS playlists ("
        "id SERIAL PRIMARY KEY, "
        "user_id INTEGER NOT NULL, "
        "name VARCHAR(100) NOT NULL, "
        "description TEXT, "
        "cover_url TEXT, "
        "is_public BOOLEAN DEFAULT false, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
    
    if (!query.exec(createPlaylists)) {
        qDebug() << "playlists error:" << query.lastError().text();
        success = false;
    }
    
    QString createPlaylistTracks = 
        "CREATE TABLE IF NOT EXISTS playlist_tracks ("
        "playlist_id INTEGER NOT NULL, "
        "track_id INTEGER NOT NULL, "
        "position INTEGER DEFAULT 0, "
        "added_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "PRIMARY KEY (playlist_id, track_id))";
    
    if (!query.exec(createPlaylistTracks)) {
        qDebug() << "playlist_tracks error:" << query.lastError().text();
        success = false;
    }
    
    QString createFriendRequests = 
        "CREATE TABLE IF NOT EXISTS friend_requests ("
        "id SERIAL PRIMARY KEY, "
        "sender_id INTEGER NOT NULL, "
        "receiver_id INTEGER NOT NULL, "
        "status VARCHAR(20) DEFAULT 'pending', "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "UNIQUE(sender_id, receiver_id))";
    
    if (!query.exec(createFriendRequests)) {
        qDebug() << "friend_requests error:" << query.lastError().text();
        success = false;
    }
    
    QString createFriendships = 
        "CREATE TABLE IF NOT EXISTS friendships ("
        "user_id INTEGER NOT NULL, "
        "friend_id INTEGER NOT NULL, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "PRIMARY KEY (user_id, friend_id))";
    
    if (!query.exec(createFriendships)) {
        qDebug() << "friendships error:" << query.lastError().text();
        success = false;
    }
    
    QString createMessages = 
        "CREATE TABLE IF NOT EXISTS messages ("
        "id SERIAL PRIMARY KEY, "
        "sender_id INTEGER NOT NULL, "
        "receiver_id INTEGER NOT NULL, "
        "content TEXT, "
        "type INTEGER DEFAULT 0, "
        "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "is_read BOOLEAN DEFAULT false, "
        "shared_track_id INTEGER, "
        "shared_station_id INTEGER)";
    
    if (!query.exec(createMessages)) {
        qDebug() << "messages error:" << query.lastError().text();
        success = false;
    }
    
    QString createRooms = 
        "CREATE TABLE IF NOT EXISTS rooms ("
        "id SERIAL PRIMARY KEY, "
        "name VARCHAR(100) NOT NULL, "
        "description TEXT, "
        "genre VARCHAR(50), "
        "host_id INTEGER NOT NULL, "
        "current_track_id INTEGER, "
        "current_station_id INTEGER, "
        "current_position BIGINT DEFAULT 0, "
        "is_playing BOOLEAN DEFAULT false, "
        "max_members INTEGER DEFAULT 10, "
        "is_private BOOLEAN DEFAULT false, "
        "password VARCHAR(64), "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
    
    if (!query.exec(createRooms)) {
        qDebug() << "rooms error:" << query.lastError().text();
        success = false;
    }
    
    QString createRoomMembers = 
        "CREATE TABLE IF NOT EXISTS room_members ("
        "room_id INTEGER NOT NULL, "
        "user_id INTEGER NOT NULL, "
        "is_host BOOLEAN DEFAULT false, "
        "can_control BOOLEAN DEFAULT false, "
        "joined_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "PRIMARY KEY (room_id, user_id))";
    
    if (!query.exec(createRoomMembers)) {
        qDebug() << "room_members error:" << query.lastError().text();
        success = false;
    }
    
    query.exec("CREATE INDEX IF NOT EXISTS idx_tracks_user ON tracks(user_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_radio_stations_user ON radio_stations(user_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_messages_receiver ON messages(receiver_id, is_read)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_friendships ON friendships(user_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_room_members ON room_members(room_id)");
    
    if (success) {
        QString testPass = hashPassword("test123");
        query.prepare("INSERT INTO users (username, password_hash) "
                     "SELECT 'testuser', ? WHERE NOT EXISTS "
                     "(SELECT 1 FROM users WHERE username = 'testuser')");
        query.addBindValue(testPass);
        query.exec();
    }
    
    return success;
}

bool DatabaseManager::isConnected() const {
    return m_db.isOpen();
}

QString DatabaseManager::hashPassword(const QString& password) {
    return QCryptographicHash::hash(password.toUtf8(), 
                                   QCryptographicHash::Sha256).toHex();
}

QString DatabaseManager::generateFileHash(const QString& filePath) {
    QString uniqueString = QString("%1_%2").arg(filePath).arg(QDateTime::currentMSecsSinceEpoch());
    return QCryptographicHash::hash(uniqueString.toUtf8(), 
                                   QCryptographicHash::Md5).toHex();
}

bool DatabaseManager::registerUser(const QString& username, const QString& password) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return false;
    }
    
    if (username.isEmpty() || password.isEmpty() || username.length() < 3 || password.length() < 6) {
        return false;
    }
    
    QSqlQuery checkQuery(m_db);
    checkQuery.prepare("SELECT id FROM users WHERE username = ?");
    checkQuery.addBindValue(username);
    
    if (checkQuery.exec() && checkQuery.next()) {
        return false;
    }
    
    QSqlQuery insertQuery(m_db);
    insertQuery.prepare("INSERT INTO users (username, password_hash) VALUES (?, ?)");
    insertQuery.addBindValue(username);
    insertQuery.addBindValue(hashPassword(password));
    
    return insertQuery.exec();
}

bool DatabaseManager::authenticateUser(const QString& username, const QString& password) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return false;
    }
    
    QSqlQuery query(m_db);
    query.prepare("SELECT id, password_hash FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    QString storedHash = query.value("password_hash").toString();
    QString inputHash = hashPassword(password);
    
    return (storedHash == inputHash);
}

int DatabaseManager::getUserId(const QString& username) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return -1;
    }
    
    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        return query.value("id").toInt();
    }
    
    return -1;
}

bool DatabaseManager::addTrack(const QString& filePath, const QString& title, 
                              const QString& artist, const QString& album, 
                              int duration, int userId) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return false;
    }
    
    QString fileHash = generateFileHash(filePath);
    
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO tracks (file_path, file_hash, user_id, title, artist, album, duration) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)"
    );
    
    query.addBindValue(filePath);
    query.addBindValue(fileHash);
    query.addBindValue(userId);
    query.addBindValue(title);
    query.addBindValue(artist);
    query.addBindValue(album);
    query.addBindValue(duration);
    
    return query.exec();
}

QList<TrackData> DatabaseManager::getUserTracks(int userId) {
    QList<TrackData> tracks;
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return tracks;
    }
    
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, file_path, file_hash, title, artist, album, duration, "
        "play_count, added_at "
        "FROM tracks WHERE user_id = ? ORDER BY added_at DESC"
    );
    query.addBindValue(userId);
    
    if (query.exec()) {
        while (query.next()) {
            TrackData track;
            track.id = query.value("id").toInt();
            track.filePath = query.value("file_path").toString();
            track.fileHash = query.value("file_hash").toString();
            track.title = query.value("title").toString();
            track.artist = query.value("artist").toString();
            track.album = query.value("album").toString();
            track.duration = query.value("duration").toInt();
            track.playCount = query.value("play_count").toInt();
            track.addedAt = query.value("added_at").toString();
            
            tracks.append(track);
        }
    }
    
    return tracks;
}

bool DatabaseManager::incrementPlayCount(int trackId) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return false;
    }
    
    QSqlQuery query(m_db);
    query.prepare("UPDATE tracks SET play_count = play_count + 1 WHERE id = ?");
    query.addBindValue(trackId);
    
    return query.exec();
}

bool DatabaseManager::deleteTrack(int trackId) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return false;
    }
    
    QSqlQuery deleteFromPlaylists(m_db);
    deleteFromPlaylists.prepare("DELETE FROM playlist_tracks WHERE track_id = ?");
    deleteFromPlaylists.addBindValue(trackId);
    deleteFromPlaylists.exec();
    
    QSqlQuery deleteTrackQuery(m_db);
    deleteTrackQuery.prepare("DELETE FROM tracks WHERE id = ?");
    deleteTrackQuery.addBindValue(trackId);
    
    return deleteTrackQuery.exec();
}

bool DatabaseManager::addRadioStation(int userId, const QString& name, const QString& country,
                                      const QString& streamUrl, const QString& genre, int bitrate)
{
    QMutexLocker locker(&m_mutex);

    if (!m_db.isOpen()) {
        if (!initialize()) return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO radio_stations (user_id, name, country, stream_url, genre, bitrate, added_at) "
        "VALUES (?, ?, ?, ?, ?, ?, CURRENT_TIMESTAMP)"
    );
    
    query.addBindValue(userId);
    query.addBindValue(name);
    query.addBindValue(country);
    query.addBindValue(streamUrl);
    query.addBindValue(genre);
    query.addBindValue(bitrate);

    if (!query.exec()) {
        qWarning() << "Add radio station error:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<TrackData> DatabaseManager::getUserRadioStations(int userId) {
    QList<TrackData> stations;
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return stations;
    }
    
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, name, country, stream_url, genre, bitrate, favicon, play_count, added_at "
        "FROM radio_stations WHERE user_id = ? ORDER BY added_at DESC"
    );
    query.addBindValue(userId);
    
    if (query.exec()) {
        while (query.next()) {
            TrackData station;
            station.id = query.value("id").toInt();
            station.title = query.value("name").toString();
            station.artist = query.value("country").toString();
            station.streamUrl = query.value("stream_url").toString();
            station.album = query.value("genre").toString();
            station.bitrate = query.value("bitrate").toInt();
            station.cover = query.value("favicon").toString();
            station.playCount = query.value("play_count").toInt();
            station.addedAt = query.value("added_at").toString();
            
            stations.append(station);
        }
    }
    
    return stations;
}

bool DatabaseManager::deleteRadioStation(int stationId) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return false;
    }
    
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM radio_stations WHERE id = ?");
    query.addBindValue(stationId);
    
    return query.exec();
}

bool DatabaseManager::incrementRadioPlayCount(int stationId) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return false;
    }
    
    QSqlQuery query(m_db);
    query.prepare("UPDATE radio_stations SET play_count = play_count + 1 WHERE id = ?");
    query.addBindValue(stationId);
    
    return query.exec();
}

bool DatabaseManager::shareRadioStation(int userId, int stationId, const QStringList& friendIds) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return false;
    }
    
    QSqlQuery query(m_db);
    
    for (const QString& friendId : friendIds) {
        query.prepare(
            "INSERT INTO radio_shares (station_id, shared_by, shared_with, shared_at) "
            "VALUES (?, ?, ?, CURRENT_TIMESTAMP)"
        );
        query.addBindValue(stationId);
        query.addBindValue(userId);
        query.addBindValue(friendId.toInt());
        
        if (!query.exec()) {
            qWarning() << "Share radio station error:" << query.lastError().text();
            return false;
        }
    }
    
    return true;
}

bool DatabaseManager::addRadioToStatus(int userId, int stationId) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return false;
    }
    
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO radio_status (user_id, station_id, created_at) "
        "VALUES (?, ?, CURRENT_TIMESTAMP)"
    );
    query.addBindValue(userId);
    query.addBindValue(stationId);
    
    return query.exec();
}

QList<TrackData> DatabaseManager::getSharedRadioStations(int userId) {
    QList<TrackData> stations;
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return stations;
    }
    
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT DISTINCT rs.id, rs.name, rs.country, rs.stream_url, rs.genre, rs.bitrate, rs.favicon, rs.play_count, rs.added_at "
        "FROM radio_stations rs "
        "INNER JOIN radio_shares share ON rs.id = share.station_id "
        "WHERE share.shared_with = ? "
        "ORDER BY rs.added_at DESC"
    );
    query.addBindValue(userId);
    
    if (query.exec()) {
        while (query.next()) {
            TrackData station;
            station.id = query.value("id").toInt();
            station.title = query.value("name").toString();
            station.artist = query.value("country").toString();
            station.streamUrl = query.value("stream_url").toString();
            station.album = query.value("genre").toString();
            station.bitrate = query.value("bitrate").toInt();
            station.cover = query.value("favicon").toString();
            station.playCount = query.value("play_count").toInt();
            station.addedAt = query.value("added_at").toString();
            
            stations.append(station);
        }
    }
    
    return stations;
}
