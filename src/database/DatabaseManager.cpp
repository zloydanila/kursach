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
    // Закрываем старое соединение
    if (m_db.isOpen()) {
        m_db.close();
    }
    
    // Создаем новое подключение
    m_db = QSqlDatabase::addDatabase("QPSQL", "soundspace_connection");
    
    if (!m_db.isValid()) {
        qDebug() << "❌ Драйвер PostgreSQL недоступен";
        return false;
    }
    
    // Railway PostgreSQL настройки
    m_db.setHostName("turntable.proxy.rlwy.net");
    m_db.setPort(26533);
    m_db.setDatabaseName("railway");
    m_db.setUserName("postgres");
    m_db.setPassword("VAIWhuipzWQGYBXtLfvtIqXfKyfkxXQS");
    m_db.setConnectOptions("connect_timeout=10");
    
    if (!m_db.open()) {
        qDebug() << "❌ Ошибка подключения:" << m_db.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Подключено к PostgreSQL";
    
    // Проверяем таблицы
    if (!createTables()) {
        qDebug() << "❌ Ошибка создания таблиц";
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
    
    // Таблица пользователей (упрощенная)
    QString createUsers = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id SERIAL PRIMARY KEY, "
        "username VARCHAR(50) UNIQUE NOT NULL, "
        "password_hash VARCHAR(64) NOT NULL, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
    
    QSqlQuery query(m_db);
    if (!query.exec(createUsers)) {
        qDebug() << "❌ Ошибка users:" << query.lastError().text();
        success = false;
    }
    
    // Таблица треков (упрощенная)
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
        "cover_url TEXT)";
    
    if (!query.exec(createTracks)) {
        qDebug() << "❌ Ошибка tracks:" << query.lastError().text();
        success = false;
    }
    
    // Таблица плейлистов
    QString createPlaylists = 
        "CREATE TABLE IF NOT EXISTS playlists ("
        "id SERIAL PRIMARY KEY, "
        "user_id INTEGER NOT NULL, "
        "name VARCHAR(100) NOT NULL, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
    
    if (!query.exec(createPlaylists)) {
        qDebug() << "❌ Ошибка playlists:" << query.lastError().text();
        success = false;
    }
    
    // Таблица плейлист-треки
    QString createPlaylistTracks = 
        "CREATE TABLE IF NOT EXISTS playlist_tracks ("
        "playlist_id INTEGER NOT NULL, "
        "track_id INTEGER NOT NULL, "
        "position INTEGER DEFAULT 0, "
        "PRIMARY KEY (playlist_id, track_id))";
    
    if (!query.exec(createPlaylistTracks)) {
        qDebug() << "❌ Ошибка playlist_tracks:" << query.lastError().text();
        success = false;
    }
    
    // Создаем тестового пользователя
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
    
    // Проверяем существование пользователя
    QSqlQuery checkQuery(m_db);
    checkQuery.prepare("SELECT id FROM users WHERE username = ?");
    checkQuery.addBindValue(username);
    
    if (checkQuery.exec() && checkQuery.next()) {
        qDebug() << "Пользователь уже существует:" << username;
        return false;
    }
    
    // Регистрируем нового пользователя
    QSqlQuery insertQuery(m_db);
    insertQuery.prepare("INSERT INTO users (username, password_hash) VALUES (?, ?)");
    insertQuery.addBindValue(username);
    insertQuery.addBindValue(hashPassword(password));
    
    bool success = insertQuery.exec();
    if (success) {
        qDebug() << "✅ Пользователь зарегистрирован:" << username;
    }
    
    return success;
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
    
    bool success = query.exec();
    if (success) {
        qDebug() << "✅ Трек добавлен:" << title << "-" << artist;
    }
    
    return success;
}

QList<Track> DatabaseManager::getUserTracks(int userId) {
    QList<Track> tracks;
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
            Track track;
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
    
    // Удаляем сначала из плейлистов
    QSqlQuery deleteFromPlaylists(m_db);
    deleteFromPlaylists.prepare("DELETE FROM playlist_tracks WHERE track_id = ?");
    deleteFromPlaylists.addBindValue(trackId);
    deleteFromPlaylists.exec();
    
    // Удаляем сам трек
    QSqlQuery deleteTrackQuery(m_db);
    deleteTrackQuery.prepare("DELETE FROM tracks WHERE id = ?");
    deleteTrackQuery.addBindValue(trackId);
    
    bool success = deleteTrackQuery.exec();
    if (success) {
        qDebug() << "✅ Трек удален, ID:" << trackId;
    }
    
    return success;
}

bool DatabaseManager::addTrackFromAPI(int userId, const QString& title, 
                                     const QString& artist, const QString& coverUrl,
                                     const QString& album, int duration,
                                     const QString& genre) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        if (!initialize()) return false;
    }
    
    QString virtualPath = QString("lastfm://%1/%2").arg(artist).arg(title);
    QString fileHash = generateFileHash(virtualPath);
    
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO tracks (file_path, file_hash, user_id, title, artist, "
        "album, duration, cover_url) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
    );
    
    query.addBindValue(virtualPath);
    query.addBindValue(fileHash);
    query.addBindValue(userId);
    query.addBindValue(title);
    query.addBindValue(artist);
    query.addBindValue(album);
    query.addBindValue(duration);
    query.addBindValue(coverUrl);
    
    bool success = query.exec();
    if (success) {
        qDebug() << "✅ API трек добавлен:" << title << "-" << artist;
    }
    
    return success;
}