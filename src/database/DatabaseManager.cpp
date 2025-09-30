#include <DatabaseManager.h>
#include <QCryptographicHash>

DatabaseManager& DatabaseManager::instance(){
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initializeDatabase(){
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("soundspace.db");
    if(!m_database.open()){
        qDebug() << "Ошибка открытия БД: " << m_database.lastError().text();
        return false;
    }

    qDebug() << "БД успешно подключена";

    qDebug() << "Создание таблиц...";
    
    if(!createUsersTable()) {
        qDebug() << "Ошибка создания таблицы users:" << m_database.lastError().text();
        return false;
    }
    qDebug() << "Таблица users создана";
    
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
    return true;
}

bool DatabaseManager::isDatabaseOpen() const{
    return m_database.isOpen();
}

bool DatabaseManager::createUsersTable(){
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
        "file_hash TEXT)"
    );
}

bool DatabaseManager::createPlaylistsTable(){
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS playlists("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER, "
        "name TEXT NOT NULL, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
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
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
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
    query.prepare("SELECT password_hash FROM users WHERE username =?");
    query.addBindValue(username);
    
    if(!query.exec()){
        qDebug() << "Ошибка аутентифекации: " << query.lastError().text();
        return false;
    }

    if(query.next()){
        QString storedHash = query.value(0).toString();
        QString inputHash = hashPassword(password);

        if(storedHash == inputHash){
            qDebug() << "Пользователь " << username << "успешно аутентифицирован";
            return true;
        }else{
            qDebug() << "Неверный пароль";
            return false;
        }
    }else{
            qDebug() << "Пользователь " << username << "не найден";
            return false;
    }
}

