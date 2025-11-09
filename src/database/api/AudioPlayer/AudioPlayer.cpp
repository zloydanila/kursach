#include "AudioPlayer.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QUrl>

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , m_player(new QMediaPlayer(this))
{
    m_player->setVolume(50);

    connect(m_player, &QMediaPlayer::positionChanged, this, &AudioPlayer::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &AudioPlayer::durationChanged);
    connect(m_player, &QMediaPlayer::stateChanged, this, &AudioPlayer::onStateChanged);
    connect(m_player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onPlayerError(QMediaPlayer::Error)));

    qDebug() << "🎵 QMediaPlayer инициализирован (Qt5)";
}

AudioPlayer::~AudioPlayer()
{
    stop();
    qDebug() << "🧹 Аудиоплеер уничтожен";
}

void AudioPlayer::loadTrack(const QString &filePath)
{
    if (filePath.isEmpty()) {
        emit errorOccurred("Путь не должен быть пустым");
        return;
    }

    m_player->setMedia(QUrl::fromLocalFile(filePath));
    m_currentTrack = filePath;

    qDebug() << "✅ Загружен трек:" << filePath;
    emit trackChanged(QFileInfo(filePath).fileName());
}

void AudioPlayer::play()
{
    if (!m_player->media().isNull()) {
        m_player->play();
        emit playbackStateChanged(true);
        qDebug() << "▶ Воспроизведение начато";
    } else {
        emit errorOccurred("Нет загруженного трека");
    }
}

void AudioPlayer::pause()
{
    m_player->pause();
    emit playbackStateChanged(false);
    qDebug() << "⏸ Пауза";
}

void AudioPlayer::stop()
{
    m_player->stop();
    emit playbackStateChanged(false);
    qDebug() << "⏹ Остановлено";
}

void AudioPlayer::setVolume(int volume)
{
    volume = qBound(0, volume, 100);
    m_player->setVolume(volume);
    emit volumeChanged(volume);
}

int AudioPlayer::volume() const { return m_player->volume(); }
bool AudioPlayer::isPlaying() const { return m_player->state() == QMediaPlayer::PlayingState; }
qint64 AudioPlayer::duration() const { return m_player->duration(); }
qint64 AudioPlayer::position() const { return m_player->position(); }
QString AudioPlayer::currentTrack() const { return m_currentTrack; }

void AudioPlayer::onStateChanged(QMediaPlayer::State state)
{
    switch (state) {
        case QMediaPlayer::PlayingState:
            qDebug() << "Состояние: Playing";
            break;
        case QMediaPlayer::PausedState:
            qDebug() << "Состояние: Paused";
            break;
        case QMediaPlayer::StoppedState:
            qDebug() << "Состояние: Stopped";
            break;
    }
}

void AudioPlayer::onPlayerError(QMediaPlayer::Error error)
{
    QString msg;
    switch (error) {
        case QMediaPlayer::NoError: return;
        case QMediaPlayer::ResourceError: msg = "Ошибка ресурса"; break;
        case QMediaPlayer::FormatError: msg = "Неверный формат"; break;
        case QMediaPlayer::NetworkError: msg = "Ошибка сети"; break;
        case QMediaPlayer::AccessDeniedError: msg = "Доступ запрещен"; break;
        default: msg = "Неизвестная ошибка"; break;
    }
    emit errorOccurred(msg);
    qDebug() << "⚠️ Ошибка плеера:" << msg;
}
