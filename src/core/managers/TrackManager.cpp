#include "TrackManager.h"
#include "database/DatabaseManager.h"

TrackManager::TrackManager(int userId, QObject *parent)
    : QObject(parent), m_userId(userId)
{
}

QVector<TrackData> TrackManager::getUserTracks()
{
    QList<TrackData> list = DatabaseManager::instance().getUserTracks(m_userId);
    return QVector<TrackData>(list.begin(), list.end());
}

TrackData TrackManager::getTrack(int trackId)
{
    TrackData track;
    track.id = trackId;
    return track;
}

bool TrackManager::deleteTrack(int trackId)
{
    return DatabaseManager::instance().deleteTrack(trackId);
}
