#include "DatabaseManager.h"
#include <QDebug>
#include "utils/Config.h"
#include <QCryptographicHash>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>
#include <QMutexLocker>

QMutex DatabaseManager::m_mutex;

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent), m_initialized(false) {}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) m_db.close();
}

bool DatabaseManager::initialize() {
    QMutexLocker locker(&m_mutex);
    if (m_initialized && m_db.isOpen()) return true;
    return openConnection();
}

bool DatabaseManager::openConnection() {
    if (QSqlDatabase::contains("soundspace_connection")) {
        {
            QSqlDatabase oldDb = QSqlDatabase::database("soundspace_connection", false);
            if (oldDb.isOpen()) oldDb.close();
        }
        QSqlDatabase::removeDatabase("soundspace_connection");
    }

    m_db = QSqlDatabase::addDatabase("QPSQL", "soundspace_connection");
    if (!m_db.isValid()) return false;

    const Config& cfg = Config::instance();
    m_db.setHostName(cfg.getDatabaseHost());
    m_db.setPort(cfg.getDatabasePort());
    m_db.setDatabaseName(cfg.getDatabaseName());
    m_db.setUserName(cfg.getDatabaseUser());
    m_db.setPassword(cfg.getDatabasePassword());
    m_db.setConnectOptions("connect_timeout=10");

    if (!m_db.open()) { qWarning() << "DB open failed:" << m_db.lastError().text(); return false; }

    if (!createTables()) return false;

    m_initialized = true;
    return true;
}

bool DatabaseManager::createTables() {
    if (!m_db.isOpen()) return false;

    QSqlQuery query(m_db);
    bool ok = true;
    auto execOrLog = [&](const QString& sql){
        if (!query.exec(sql)) {
            qWarning() << "SQL failed:" << query.lastError().text() << "SQL:" << sql;
            return false;
        }
        return true;
    };

    ok = ok && execOrLog(
        "CREATE TABLE IF NOT EXISTS users ("
        "id SERIAL PRIMARY KEY, "
        "username VARCHAR(50) UNIQUE NOT NULL, "
        "password_hash VARCHAR(128) NOT NULL, "
        "email VARCHAR(100), "
        "avatar_path TEXT, "
        "status INTEGER DEFAULT 4, "
        "bio TEXT, "
        "last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"
    );

    query.exec("ALTER TABLE users ADD COLUMN IF NOT EXISTS email VARCHAR(100)");
    query.exec("ALTER TABLE users ADD COLUMN IF NOT EXISTS avatar_path TEXT");
    query.exec("ALTER TABLE users ADD COLUMN IF NOT EXISTS status INTEGER DEFAULT 4");
    query.exec("ALTER TABLE users ADD COLUMN IF NOT EXISTS bio TEXT");
    query.exec("ALTER TABLE users ADD COLUMN IF NOT EXISTS last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP");
    query.exec("ALTER TABLE users ADD COLUMN IF NOT EXISTS created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP");
    query.exec("ALTER TABLE users ADD COLUMN IF NOT EXISTS avatar_data BYTEA");

    ok = ok && query.exec(
        "CREATE TABLE IF NOT EXISTS tracks ("
        "id SERIAL PRIMARY KEY, "
        "file_path TEXT NOT NULL, "
        "file_hash VARCHAR(64) NOT NULL, "
        "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
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
        "sample_rate INTEGER)"
    );

    ok = ok && query.exec(
        "CREATE TABLE IF NOT EXISTS radio_stations ("
        "id SERIAL PRIMARY KEY, "
        "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "name TEXT NOT NULL, "
        "country TEXT, "
        "stream_url TEXT NOT NULL, "
        "genre TEXT, "
        "bitrate INTEGER DEFAULT 128, "
        "favicon TEXT, "
        "play_count INTEGER DEFAULT 0, "
        "is_favorite BOOLEAN DEFAULT false, "
        "added_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"
    );

    ok = ok && query.exec(
        "CREATE TABLE IF NOT EXISTS radio_shares ("
        "id SERIAL PRIMARY KEY, "
        "station_id INTEGER NOT NULL REFERENCES radio_stations(id) ON DELETE CASCADE, "
        "shared_by INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "shared_with INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "shared_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"
    );

    ok = ok && query.exec(
        "CREATE TABLE IF NOT EXISTS radio_status ("
        "id SERIAL PRIMARY KEY, "
        "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "station_id INTEGER NOT NULL REFERENCES radio_stations(id) ON DELETE CASCADE, "
        "status_text TEXT, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"
    );

    ok = ok && query.exec(
        "CREATE TABLE IF NOT EXISTS playlists ("
        "id SERIAL PRIMARY KEY, "
        "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "name VARCHAR(100) NOT NULL, "
        "description TEXT, "
        "cover_url TEXT, "
        "is_public BOOLEAN DEFAULT false, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"
    );

    ok = ok && query.exec(
        "CREATE TABLE IF NOT EXISTS playlist_tracks ("
        "playlist_id INTEGER NOT NULL REFERENCES playlists(id) ON DELETE CASCADE, "
        "track_id INTEGER NOT NULL REFERENCES tracks(id) ON DELETE CASCADE, "
        "position INTEGER DEFAULT 0, "
        "added_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "PRIMARY KEY (playlist_id, track_id))"
    );

    ok = ok && query.exec(
        "CREATE TABLE IF NOT EXISTS friend_requests ("
        "id SERIAL PRIMARY KEY, "
        "sender_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "receiver_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "status VARCHAR(20) DEFAULT 'pending', "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "UNIQUE(sender_id, receiver_id))"
    );

    ok = ok && query.exec(
        "CREATE TABLE IF NOT EXISTS friendships ("
        "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "friend_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "PRIMARY KEY (user_id, friend_id))"
    );

    ok = ok && query.exec(
        "CREATE TABLE IF NOT EXISTS messages ("
        "id SERIAL PRIMARY KEY, "
        "sender_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "receiver_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "content TEXT, "
        "type INTEGER DEFAULT 0, "
        "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "is_read BOOLEAN DEFAULT false, "
        "shared_track_id INTEGER REFERENCES tracks(id) ON DELETE SET NULL, "
        "shared_station_id INTEGER REFERENCES radio_stations(id) ON DELETE SET NULL)"
    );

    ok = ok && query.exec(
        "CREATE TABLE IF NOT EXISTS rooms ("
        "id SERIAL PRIMARY KEY, "
        "name VARCHAR(100) NOT NULL, "
        "description TEXT, "
        "genre VARCHAR(50), "
        "host_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "current_track_id INTEGER REFERENCES tracks(id) ON DELETE SET NULL, "
        "current_station_id INTEGER REFERENCES radio_stations(id) ON DELETE SET NULL, "
        "current_position BIGINT DEFAULT 0, "
        "is_playing BOOLEAN DEFAULT false, "
        "max_members INTEGER DEFAULT 10, "
        "is_private BOOLEAN DEFAULT false, "
        "password VARCHAR(128), "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"
    );

    ok = ok && query.exec(
        "CREATE TABLE IF NOT EXISTS room_members ("
        "room_id INTEGER NOT NULL REFERENCES rooms(id) ON DELETE CASCADE, "
        "user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE, "
        "is_host BOOLEAN DEFAULT false, "
        "can_control BOOLEAN DEFAULT false, "
        "joined_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "PRIMARY KEY (room_id, user_id))"
    );

    query.exec("CREATE INDEX IF NOT EXISTS idx_tracks_user ON tracks(user_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_radio_stations_user ON radio_stations(user_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_messages_receiver ON messages(receiver_id, is_read)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_friendships ON friendships(user_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_room_members ON room_members(room_id)");

    return ok;
}

bool DatabaseManager::isConnected() const {
    return m_db.isOpen();
}

QSqlDatabase DatabaseManager::database() {
    if (!m_db.isOpen()) initialize();
    return m_db;
}

QString DatabaseManager::hashPassword(const QString& password) {
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
}

QString DatabaseManager::generateFileHash(const QString& filePath) {
    QByteArray data = (filePath + "|" + QString::number(QDateTime::currentMSecsSinceEpoch())).toUtf8();
    return QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
}

bool DatabaseManager::registerUser(const QString& username, const QString& password) {
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return false;
    if (username.trimmed().length() < 3 || password.length() < 6) return false;

    QSqlQuery insert(m_db);
    insert.prepare("INSERT INTO users (username, password_hash, status, last_seen) VALUES (?, ?, ?, NOW())");
    insert.addBindValue(username.trimmed());
    insert.addBindValue(hashPassword(password));
    insert.addBindValue(static_cast<int>(UserStatus::Offline));
    return insert.exec();
}

bool DatabaseManager::authenticateUser(const QString& username, const QString& password) {
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return false;

    QSqlQuery query(m_db);
    query.prepare("SELECT password_hash FROM users WHERE username = ?");
    query.addBindValue(username.trimmed());
    if (!query.exec() || !query.next()) return false;

    return query.value(0).toString() == hashPassword(password);
}

int DatabaseManager::getUserId(const QString& username) {
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return -1;

    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM users WHERE username = ?");
    query.addBindValue(username.trimmed());
    if (query.exec() && query.next()) return query.value(0).toInt();
    return -1;
}

bool DatabaseManager::setUserStatus(int userId, UserStatus status)
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return false;

    QSqlQuery q(m_db);
    q.prepare("UPDATE users SET status = ?, last_seen = NOW() WHERE id = ?");
    q.addBindValue(static_cast<int>(status));
    q.addBindValue(userId);
    return q.exec();
}

UserStatus DatabaseManager::getUserStatus(int userId)
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return UserStatus::Offline;

    QSqlQuery q(m_db);
    q.prepare("SELECT status FROM users WHERE id = ?");
    q.addBindValue(userId);
    if (!q.exec() || !q.next()) return UserStatus::Offline;
    return static_cast<UserStatus>(q.value(0).toInt());
}

bool DatabaseManager::addTrack(const QString& filePath, const QString& title,
                              const QString& artist, const QString& album,
                              int duration, int userId) {
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return false;

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO tracks (file_path, file_hash, user_id, title, artist, album, duration) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)"
    );
    query.addBindValue(filePath);
    query.addBindValue(generateFileHash(filePath));
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
    if (!m_db.isOpen() && !initialize()) return tracks;

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, file_path, file_hash, title, artist, album, duration, play_count, added_at "
        "FROM tracks WHERE user_id = ? ORDER BY added_at DESC"
    );
    query.addBindValue(userId);

    if (query.exec()) {
        while (query.next()) {
            TrackData t;
            t.id = query.value(0).toInt();
            t.filePath = query.value(1).toString();
            t.fileHash = query.value(2).toString();
            t.title = query.value(3).toString();
            t.artist = query.value(4).toString();
            t.album = query.value(5).toString();
            t.duration = query.value(6).toInt();
            t.playCount = query.value(7).toInt();
            t.addedAt = query.value(8).toString();
            tracks.append(t);
        }
    }
    return tracks;
}

bool DatabaseManager::incrementPlayCount(int trackId) {
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return false;

    QSqlQuery query(m_db);
    query.prepare("UPDATE tracks SET play_count = play_count + 1 WHERE id = ?");
    query.addBindValue(trackId);
    return query.exec();
}

bool DatabaseManager::deleteTrack(int trackId) {
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return false;

    QSqlQuery deleteFromPlaylists(m_db);
    deleteFromPlaylists.prepare("DELETE FROM playlist_tracks WHERE track_id = ?");
    deleteFromPlaylists.addBindValue(trackId);
    deleteFromPlaylists.exec();

    QSqlQuery del(m_db);
    del.prepare("DELETE FROM tracks WHERE id = ?");
    del.addBindValue(trackId);
    return del.exec();
}

bool DatabaseManager::addRadioStation(int userId, const QString& name, const QString& country,
                                     const QString& streamUrl, const QString& genre, int bitrate) {
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return false;

    QString n = name.trimmed();
    QString c = country.trimmed();
    QString g = genre.trimmed();
    QString u = streamUrl.trimmed();

    if (n.isEmpty() || u.isEmpty()) return false;

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO radio_stations (user_id, name, country, stream_url, genre, bitrate, added_at) "
        "VALUES (?, ?, ?, ?, ?, ?, CURRENT_TIMESTAMP)"
    );
    query.addBindValue(userId);
    query.addBindValue(n);
    query.addBindValue(c);
    query.addBindValue(u);
    query.addBindValue(g);
    query.addBindValue(bitrate > 0 ? bitrate : 128);
    return query.exec();
}

QList<TrackData> DatabaseManager::getUserRadioStations(int userId) {
    QList<TrackData> stations;
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return stations;

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, name, country, stream_url, genre, bitrate, favicon, play_count, added_at "
        "FROM radio_stations WHERE user_id = ? ORDER BY added_at DESC"
    );
    query.addBindValue(userId);

    if (query.exec()) {
        while (query.next()) {
            TrackData s;
            s.id = query.value(0).toInt();
            s.title = query.value(1).toString();
            s.artist = query.value(2).toString();
            s.streamUrl = query.value(3).toString();
            s.album = query.value(4).toString();
            s.bitrate = query.value(5).toInt();
            s.source = query.value(6).toString();
            s.playCount = query.value(7).toInt();
            s.addedAt = query.value(8).toString();
            stations.append(s);
        }
    }
    return stations;
}

bool DatabaseManager::deleteRadioStation(int stationId) {
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return false;

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM radio_stations WHERE id = ?");
    query.addBindValue(stationId);
    return query.exec();
}

bool DatabaseManager::incrementRadioPlayCount(int stationId) {
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return false;

    QSqlQuery query(m_db);
    query.prepare("UPDATE radio_stations SET play_count = play_count + 1 WHERE id = ?");
    query.addBindValue(stationId);
    return query.exec();
}

bool DatabaseManager::shareRadioStation(int userId, int stationId, const QStringList& friendIds) {
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return false;

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO radio_shares (station_id, shared_by, shared_with, shared_at) "
        "VALUES (?, ?, ?, CURRENT_TIMESTAMP)"
    );

    for (const QString& fid : friendIds) {
        bool ok = false;
        int toId = fid.toInt(&ok);
        if (!ok) continue;
        query.bindValue(0, stationId);
        query.bindValue(1, userId);
        query.bindValue(2, toId);
        if (!query.exec()) return false;
    }
    return true;
}

bool DatabaseManager::addRadioToStatus(int userId, int stationId) {
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return false;

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
    if (!m_db.isOpen() && !initialize()) return stations;

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
            TrackData s;
            s.id = query.value(0).toInt();
            s.title = query.value(1).toString();
            s.artist = query.value(2).toString();
            s.streamUrl = query.value(3).toString();
            s.album = query.value(4).toString();
            s.bitrate = query.value(5).toInt();
            s.source = query.value(6).toString();
            s.playCount = query.value(7).toInt();
            s.addedAt = query.value(8).toString();
            stations.append(s);
        }
    }
    return stations;
}

QList<QPair<int, QString>> DatabaseManager::searchUsers(const QString& queryText, int excludeUserId)
{
    QList<QPair<int, QString>> out;
    QMutexLocker locker(&m_mutex);
    if (!m_db.isOpen() && !initialize()) return out;

    QString q = queryText.trimmed().toLower();
    if (q.isEmpty()) return out;

    QSqlQuery query(m_db);
    query.prepare("SELECT id, username FROM users WHERE LOWER(username) LIKE ? AND id <> ? ORDER BY username LIMIT 50");
    query.addBindValue("%" + q + "%");
    query.addBindValue(excludeUserId);

    if (query.exec()) {
        while (query.next()) {
            out.append({query.value(0).toInt(), query.value(1).toString()});
        }
    }
    return out;
}
