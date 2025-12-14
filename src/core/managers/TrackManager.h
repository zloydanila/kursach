#ifndef TRACKMANAGER_H
#define TRACKMANAGER_H

#include <QObject>
#include <QVector>
#include "core/models/Track.h"

class TrackManager : public QObject
{
    Q_OBJECT

public:
    explicit TrackManager(int userId, QObject *parent = nullptr);
    
    bool addTrack(const TrackData& track);
    bool deleteTrack(int trackId);
    bool updateTrack(int trackId, const TrackData& track);
    bool incrementPlayCount(int trackId);
    
    QVector<TrackData> getUserTracks();
    TrackData getTrack(int trackId);
    QVector<TrackData> searchTracks(const QString& query);
    QVector<TrackData> getRecentTracks(int limit = 20);
    QVector<TrackData> getMostPlayedTracks(int limit = 20);

signals:
    void trackAdded(int trackId);
    void trackDeleted(int trackId);
    void trackUpdated(int trackId);

private:
    int m_userId;
};

#endif
