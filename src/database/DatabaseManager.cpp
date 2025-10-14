#include <DatabaseManager.h>
#include <QCryptographicHash>

DatabaseManager& DatabaseManager::instance(){
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initializeDatabase(){
    m_database = QSqlDatabase::addDatabase("QPSQL");
    
    m_database.setHostName("metro.proxy.rlwy.net");
    m_database.setPort(34314);
    m_database.setDatabaseName("railway");
    m_database.setUserName("postgres");
    m_database.setPassword("vhUEaNIeiKoyrcVzGyIXPwgOemrTcrOw");
    

    m_database.setConnectOptions("requiressl=1;sslmode=require");
    
    if(!m_database.open()){
        qDebug() << "Ошибка подключения к Railway:" << m_database.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Успешно подключились к Railway!";
    
    
    if(!m_database.open()){
        qDebug() << "Ошибка подключения к Railway:" << m_database.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Успешно подключились к Railway!";
    qDebug() << "БД успешно подключена";

    qDebug() << "Создание таблиц...";
    
    if(!createUsersTable()) {
        qDebug() << "Ошибка создания таблицы users:" << m_database.lastError().text();
        return false;
    }
    qDebug() << "Таблица users создана";
    

    if(!createTracksMetadataTable()){
        qDebug() << "Ошибка создания таблицы track_metadata: " << m_database.lastError().text();
        return false;
    }
    qDebug() << "Таблица track_metadata создана";


    if(!createTracksTable()) {
        qDebug() << "Ошибка создания таблицы tracks:" << m_database.lastError().text();
        return false;
    }
    qDebug() << "Таблица tracks создана";
    
    if(!createPlaylistsTable()) {
        qDebug() << "Ошибка создания таблицы playlists:" << m_database.lastError().text();
        return false;
    }
    qDebug() << "Таблица playlists создана";
    
    if(!createPlaylistTracksTable()) {
        qDebug() << "Ошибка создания таблицы playlist_tracks:" << m_database.lastError().text();
        return false;
    }
    qDebug() << "Таблица playlist_tracks создана";
    
    if(!createFriendsTable()) {
        qDebug() << "Ошибка создания таблицы friends:" << m_database.lastError().text();
        return false;
    }
    qDebug() << "Таблица friends создана";
    
    qDebug() << "Все таблицы успешно созданы!";

    QSqlQuery query("SELECT version();");
    if (query.exec() && query.next()) {
        qDebug() << "✅ PostgreSQL version:" << query.value(0).toString();
    }
    return true;
}

bool DatabaseManager::isDatabaseOpen() const{
    return m_database.isOpen();
}

bool DatabaseManager::createUsersTable(){
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id SERIAL PRIMARY KEY, "
        "username TEXT UNIQUE NOT NULL, "
        "password_hash TEXT NOT NULL, "
        "avatar_path TEXT, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"                                    
    );
}

bool DatabaseManager::createTracksMetadataTable(){
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS track_metadata("
        "file_hash TEXT PRIMARY KEY, "
        "title TEXT NOT NULL, "
        "artist TEXT NOT NULL, "
        "album TEXT, "
        "duration INTEGER NOT NULL, "
        "genre TEXT, "
        "year INTEGER, "
        "bitrate INTEGER, "
        "sample_rate INTEGER)"
    );
}

bool DatabaseManager::createTracksTable(){
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS tracks("
        "id SERIAL PRIMARY KEY, "
        "file_path TEXT NOT NULL, "
        "file_hash TEXT NOT NULL, "
        "user_id INTEGER, "
        "added_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "play_count INTEGER DEFAULT 0, "
        "title TEXT, "
        "artist TEXT, "
        "album TEXT, "
        "duration INTEGER, "
        "FOREIGN KEY (file_hash) REFERENCES track_metadata(file_hash), "
        "FOREIGN KEY (user_id) REFERENCES users(id))"
    );
}

bool DatabaseManager::createPlaylistsTable(){
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS playlists("
        "id SERIAL PRIMARY KEY, "
        "user_id INTEGER, "
        "name TEXT NOT NULL, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY (user_id) REFERENCES users(id))"
    );
}

bool DatabaseManager::createPlaylistTracksTable(){
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS playlist_tracks("
        "playlist_id INTEGER, "
        "track_id INTEGER, "
        "position INTEGER, "
        "PRIMARY KEY (playlist_id, track_id), "
        "FOREIGN KEY (playlist_id) REFERENCES playlists(id), "
        "FOREIGN KEY (track_id) REFERENCES tracks(id))"
    );
}

bool DatabaseManager::createFriendsTable(){
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS friends("
        "user_id INTEGER, "
        "friend_id INTEGER, "
        "status TEXT DEFAULT 'pending', "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "PRIMARY KEY (user_id, friend_id), "
        "FOREIGN KEY (user_id) REFERENCES users(id), "
        "FOREIGN KEY (friend_id) REFERENCES users(id))"
    );
}

QString DatabaseManager::hashPassword(const QString& password){
    QByteArray passwordData = password.toUtf8();
    return QString(QCryptographicHash::hash(passwordData, QCryptographicHash::Sha256).toHex());
}

bool DatabaseManager::registerUser(const QString& username, const QString& password){
    if (username.isEmpty() || password.isEmpty()) {
        qDebug() << "Пароль не может быть пустым";
        return false;
    }
    
    if (username.length() < 3) {
        qDebug() << "Имя пользователя должно быть больше 3 символов";
        return false;
    }
    
    if (password.length() < 6) {
        qDebug() << "Длина пароля должна быть больше 6 символов";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password_hash) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashPassword(password));
    
    if (!query.exec()) {
        QSqlError error = query.lastError();
        qDebug() << "Регистрация для пользователя" << username << "провалена:" << error.text();
        return false;
    }

    qDebug() << "Пользователь" << username << "успешно зарегистрирован";
    return true;
}

bool DatabaseManager::authenticateUser(const QString& username, const QString& password){
    if(username.isEmpty() || password.isEmpty()){
        qDebug() << "Имя пользователя или пароль не могут быть пустыми";
        return false;
    }

    QSqlQuery query;
    query.prepare("SELECT password_hash FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if(!query.exec()){
        qDebug() << "Ошибка аутентификации: " << query.lastError().text();
        return false;
    }

    if(query.next()){
        QString storedHash = query.value(0).toString();
        QString inputHash = hashPassword(password);

        if(storedHash == inputHash){
            qDebug() << "Пользователь" << username << "успешно аутентифицирован";
            return true;
        }else{
            qDebug() << "Неверный пароль";
            return false;
        }
    }else{
        qDebug() << "Пользователь" << username << "не найден";
        return false;
    }
}

bool DatabaseManager::addTrackMetadata(const QString& fileHash, const QString& title, 
                                     const QString& artist, const QString& album, 
                                     int duration, const QString& genre, int year,
                                     int bitrate, int sampleRate) {
    QSqlQuery query;
    query.prepare(
        "INSERT INTO track_metadata "
        "(file_hash, title, artist, album, duration, genre, year, bitrate, sample_rate) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?) "
        "ON CONFLICT (file_hash) DO NOTHING"
    );
    
    query.addBindValue(fileHash);
    query.addBindValue(title);
    query.addBindValue(artist);
    query.addBindValue(album);
    query.addBindValue(duration);
    query.addBindValue(genre);
    query.addBindValue(year);
    query.addBindValue(bitrate);
    query.addBindValue(sampleRate);
    
    if (!query.exec()) {
        qDebug() << "Ошибка добавления метаданных трека:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::addTrackToUser(const QString& filePath, const QString& fileHash,
                                   int userId, const QString& title, const QString& artist,
                                   const QString& album, int duration) {
    QSqlQuery query;
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
    
    if (!query.exec()) {
        qDebug() << "Ошибка добавления трека пользователю:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<Track> DatabaseManager::getUserTracks(int userId) {
    QList<Track> tracks;
    QSqlQuery query;
    
    query.prepare(
        "SELECT t.id, t.file_path, t.file_hash, t.title, t.artist, t.album, t.duration, "
        "t.play_count, t.added_at, tm.genre, tm.year, tm.bitrate, tm.sample_rate "
        "FROM tracks t "
        "LEFT JOIN track_metadata tm ON t.file_hash = tm.file_hash "
        "WHERE t.user_id = ? "
        "ORDER BY t.artist, t.title"
    );
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qDebug() << "Ошибка получения треков пользователя:" << query.lastError().text();
        return tracks;
    }
    
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
        track.genre = query.value("genre").toString();
        track.year = query.value("year").toInt();
        track.bitrate = query.value("bitrate").toInt();
        track.sampleRate = query.value("sample_rate").toInt();
        
        tracks.append(track);
    }
    
    return tracks;
}

int DatabaseManager::createPlaylist(int userId, const QString& name) {
    QSqlQuery query;
    query.prepare(
        "INSERT INTO playlists (user_id, name) VALUES (?, ?)"
    );
    query.addBindValue(userId);
    query.addBindValue(name);
    
    if (!query.exec()) {
        qDebug() << "Ошибка создания плейлиста:" << query.lastError().text();
        return -1;
    }
    
    return query.lastInsertId().toInt();
}

bool DatabaseManager::addTrackToPlaylist(int playlistId, int trackId, int position) {
    QSqlQuery query;
    query.prepare(
        "INSERT INTO playlist_tracks (playlist_id, track_id, position) VALUES (?, ?, ?)"
    );
    query.addBindValue(playlistId);
    query.addBindValue(trackId);
    query.addBindValue(position);
    
    if (!query.exec()) {
        qDebug() << "Ошибка добавления трека в плейлист:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<Playlist> DatabaseManager::getUserPlaylists(int userId) {
    QList<Playlist> playlists;
    QSqlQuery query;
    
    query.prepare(
        "SELECT id, name, created_at FROM playlists WHERE user_id = ? ORDER BY created_at DESC"
    );
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qDebug() << "Ошибка получения плейлистов:" << query.lastError().text();
        return playlists;
    }
    
    while (query.next()) {
        Playlist playlist;
        playlist.id = query.value("id").toInt();
        playlist.userId = userId;
        playlist.name = query.value("name").toString();
        playlist.createdAt = query.value("created_at").toString();
        
        playlists.append(playlist);
    }
    
    return playlists;
}

QList<Track> DatabaseManager::getPlaylistTracks(int playlistId) {
    QList<Track> tracks;
    QSqlQuery query;
    
    query.prepare(
        "SELECT t.id, t.file_path, t.file_hash, t.title, t.artist, t.album, t.duration, "
        "t.play_count, pt.position "
        "FROM tracks t "
        "JOIN playlist_tracks pt ON t.id = pt.track_id "
        "WHERE pt.playlist_id = ? "
        "ORDER BY pt.position"
    );
    query.addBindValue(playlistId);
    
    if (!query.exec()) {
        qDebug() << "Ошибка получения треков плейлиста:" << query.lastError().text();
        return tracks;
    }
    
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
        track.position = query.value("position").toInt();
        
        tracks.append(track);
    }
    
    return tracks;
}

bool DatabaseManager::incrementPlayCount(int trackId) {
    QSqlQuery query;
    query.prepare(
        "UPDATE tracks SET play_count = play_count + 1 WHERE id = ?"
    );
    query.addBindValue(trackId);
    
    if (!query.exec()) {
        qDebug() << "Ошибка увеличения счетчика прослушиваний:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::userExists(const QString& username) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (!query.exec()) {
        qDebug() << "Ошибка проверки пользователя:" << query.lastError().text();
        return false;
    }
    
    return query.next();
}

int DatabaseManager::getUserId(const QString& username) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (!query.exec() || !query.next()) {
        return -1;
    }
    
    return query.value("id").toInt();
}