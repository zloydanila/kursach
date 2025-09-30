#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DatabaseManager
{
public:
    static DatabaseManager& instance();
    
    // Инициализация БД
    bool initializeDatabase();
    bool isDatabaseOpen() const;
    
    // Работа с пользователями
    bool registerUser(const QString& username, const QString& password);
    bool authenticateUser(const QString& username, const QString& password);  // 🔽 ИСПРАВЛЕНО название
    
private:
    DatabaseManager() = default;
    
    // Создание таблиц
    bool createUsersTable();
    bool createTracksTable();
    bool createPlaylistsTable();
    bool createPlaylistTracksTable();
    bool createFriendsTable();
    
    // Безопасность
    QString hashPassword(const QString& password);

    QSqlDatabase m_database;
};

#endif