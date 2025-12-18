#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include <QStringList>
#include <QMutex>
#include "core/models/Track.h"
#include "core/models/User.h"

class DatabaseManager : public QObject {
    Q_OBJECT
signals:
    void userRadioChanged(int userId);

public:
    static DatabaseManager& instance();

    bool initialize();
    bool isConnected() const;

    QSqlDatabase database();

    bool registerUser(const QString& username, const QString& password);
    bool authenticateUser(const QString& username, const QString& password);
    int getUserId(const QString& username);
    QList<QPair<int, QString>> searchUsers(const QString& query, int excludeUserId);

    bool setUserStatus(int userId, UserStatus status);
    UserStatus getUserStatus(int userId);

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
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool openConnection();
    bool createTables();

    static QString hashPassword(const QString& password);
    static QString generateFileHash(const QString& filePath);

    QSqlDatabase m_db;
    bool m_initialized;
    static QMutex m_mutex;
};

#endif
