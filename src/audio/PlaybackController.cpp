#include "PlaybackController.h"
#include <QUrl>

PlaybackController::PlaybackController(QMediaPlayer* player, QObject *parent)
    : QObject(parent), m_player(player)
{
    connect(m_player, &QMediaPlayer::stateChanged, this, &PlaybackController::stateChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &PlaybackController::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &PlaybackController::durationChanged);
}

void PlaybackController::playTrack(const TrackData& track)
{
    m_currentTrack = track;
    
    if (track.filePath.startsWith("http://") || track.filePath.startsWith("https://")) {
        m_player->setMedia(QUrl(track.filePath));
    } else {
        m_player->setMedia(QUrl::fromLocalFile(track.filePath));
    }
    
    m_player->play();
    emit trackChanged(track);
}

void PlaybackController::pause()
{
    m_player->pause();
}

void PlaybackController::resume()
{
    m_player->play();
}

void PlaybackController::stop()
{
    m_player->stop();
}

void PlaybackController::seek(qint64 position)
{
    m_player->setPosition(position);
}

void PlaybackController::setVolume(int volume)
{
    m_player->setVolume(volume);
    emit volumeChanged(volume);
}

bool PlaybackController::isPlaying() const
{
    return m_player->state() == QMediaPlayer::PlayingState;
}

qint64 PlaybackController::position() const
{
    return m_player->position();
}

qint64 PlaybackController::duration() const
{
    return m_player->duration();
}

int PlaybackController::volume() const
{
    return m_player->volume();
}
