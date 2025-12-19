#ifndef PLAYBACKCONTROLLER_H
#define PLAYBACKCONTROLLER_H

#include <QObject>
#include <QMediaPlayer>
#include "core/models/Track.h"

class PlaybackController : public QObject
{
    Q_OBJECT

public:
    explicit PlaybackController(QMediaPlayer* player, QObject *parent = nullptr);
    
    void playTrack(const TrackData& track);
    void pause();
    void resume();
    void stop();
    void seek(qint64 position);
    void setVolume(int volume);
    
    bool isPlaying() const;
    qint64 position() const;
    qint64 duration() const;
    int volume() const;

signals:
    void trackChanged(const TrackData& track);
    void stateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void volumeChanged(int volume);

private:
    QMediaPlayer* m_player;
    TrackData m_currentTrack;
};

#endif
