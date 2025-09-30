#include <DatabaseManager.h>
#include <QCryptographicHash>

DatabaseManager& DatabaseManager::instance(){
    static DatabaseManager instanse;
    return instance;
}

bool DatabaseManager::initializeDatabase(){

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("soundspace.db");
    if(!m_database.open()){
        qDebug() << "Ошибка открытия БД: " << m_database.lastError().text();
        return false;
    }

    qDebug() << "БД успешно подключена"
    if(!createUsersTable() || !createTrackTable() || !createPlaylistTable() ||
    !createPlaylistTrackTable || !createFriendTable()){
        qDebug() << "Ошибка создания таблицы";
        return false;
    }
    qDebug() << "Все таблицы успешно созданы";
    return true;
}

bool DatabaseManager::isDatabaseOpen(){
    return m_database.isOpen();
}

bool DatabaseManager::createUserTable(){
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password_hash TEXT NOT NULL, "
        "avatar_path TEXT, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)"                                    
    );
}

bool DatabaseManager::createTracksTable(){
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS tracks("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "file_path TEXT UNIQUE NOT NULL, "
        "title TEXT, "
        "artist TEXT, "
        "album TEXT, "
        "duration INTEGER, "
        "fileHash TEXT)"
    );
}

bool DatabaseManager::createPlaylistTable(){
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS playlist("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "userId INTEGER, "
        "name TEXT NOT NULL, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY (user_id) REFERENCES users(id))"
    );
}

bool DatabaseManager::createPlaylistTracksTable(){
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS playlist_tracks("
        "playlistId INTEGER, "
        "trackId INTEGER, "
        "position INTEGER,"
        "PRIMARY KEY (playlist_id, track_id), "
        "FOREIGN KEY (playlist_id) REFERENCES playlists(id), "
        "FOREIGN KEY (track_id) REFERENCES tracks(id))"
    );

}