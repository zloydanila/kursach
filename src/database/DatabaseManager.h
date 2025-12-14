#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QMutex>
#include <QObject>
#include "core/models/Track.h"
#include "core/models/Playlist.h"

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance();
    
    bool initialize();
    bool isConnected() const;
    QSqlDatabase database() { return m_db; }
    
    bool registerUser(const QString& username, const QString& password);
    bool authenticateUser(const QString& username, const QString& password);
    int getUserId(const QString& username);
    
    bool addTrack(const QString& filePath, const QString& title, 
                 const QString& artist, const QString& album, 
                 int duration, int userId);
    QList<TrackData> getUserTracks(int userId);
    bool incrementPlayCount(int trackId);
    bool deleteTrack(int trackId);
    
    bool addRadioStation(int userId, const QString& name, const QString& country,
                     const QString& streamUrl, const QString& genre, int bitrate);
    QList<TrackData> getUserRadioStations(int userId);
    bool deleteRadioStation(int stationId);
    bool incrementRadioPlayCount(int stationId);
    bool shareRadioStation(int userId, int stationId, const QStringList& friendIds);
    bool addRadioToStatus(int userId, int stationId);
    QList<TrackData> getSharedRadioStations(int userId);

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

#endif
