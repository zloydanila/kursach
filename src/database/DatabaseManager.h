#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QMutex>
#include <QObject>

struct Track {
    int id;
    QString filePath;
    QString fileHash;
    QString title;
    QString artist;
    QString album;
    int duration;
    int playCount;
    QString addedAt;
};

struct Playlist {
    int id;
    int userId;
    QString name;
    QString createdAt;
};

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance();
    
    // Подключение
    bool initialize();
    bool isConnected() const;
    
    // Пользователи
    bool registerUser(const QString& username, const QString& password);
    bool authenticateUser(const QString& username, const QString& password);
    int getUserId(const QString& username);
    
    // Треки
    bool addTrack(const QString& filePath, const QString& title, 
                 const QString& artist, const QString& album, 
                 int duration, int userId);
    QList<Track> getUserTracks(int userId);
    bool incrementPlayCount(int trackId);
    bool deleteTrack(int trackId);
    
    // API интеграция
 bool addTrackFromAPI(int userId, const QString& title, 
                    const QString& artist, const QString& coverUrl,
                    const QString& album = "", int duration = 0,
                    const QString& genre = "");
    
private:
    DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    bool openConnection();
    bool createTables();
    QString hashPassword(const QString& password);
    QString generateFileHash(const QString& filePath);
    
    QSqlDatabase m_db;
    static QMutex m_mutex;
    bool m_initialized;
};

#endif // DATABASEMANAGER_H