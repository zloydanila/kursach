#include "AudioPlayer.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent)
{
    m_audioOutput = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), QAudioFormat(), this);
    m_audioOutput->setVolume(0.5);

    m_player = new QMediaPlayer(this);
    
    m_player->setVolume(50); 

    connect(m_player, &QMediaPlayer::positionChanged, this, &AudioPlayer::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &AudioPlayer::durationChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &AudioPlayer::onMediaStatusChanged);
    connect(m_player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &AudioPlayer::onPlayerError);

    qDebug() << "Аудио-плеер инициализирован";
}

void AudioPlayer::play()
{
    if(m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        m_player->play();
        emit playbackStateChanged(true);
        qDebug() << "Воспроизведение начато";
    } else {
        qDebug() << "Нет загруженного трека";
    }
}

void AudioPlayer::pause()
{
    m_player->pause();
    emit playbackStateChanged(false);
    qDebug() << "Воспроизведение поставлено на паузу";
}

void AudioPlayer::stop()
{
    m_player->stop();
    emit playbackStateChanged(false);
    qDebug() << "Воспроизведение остановлено";
}

void AudioPlayer::setVolume(int volume)
{
    volume = qBound(0, volume, 100);
    m_player->setVolume(volume);
    emit volumeChanged(volume);
    qDebug() << "Громкость установлена: " << volume << "%";
}

void AudioPlayer::loadTrack(const QString& filePath)
{
    if(filePath.isEmpty()) {
        qDebug() << "Путь не должен быть пустым";
        emit errorOccurred("Путь не должен быть пустым");
        return;
    }

    if(!QFile::exists(filePath)) {
        qDebug() << "Файл не существует" << filePath;
        emit errorOccurred("Файл не существует");
        return;
    }
    m_player->setMedia(QUrl::fromLocalFile(filePath));
    m_currentTrack = filePath;

    qDebug() << "Загружен трек: " << filePath;
    emit trackChanged(QFileInfo(filePath).fileName());
}

bool AudioPlayer::isPlaying() const
{
    return m_player->state() == QMediaPlayer::PlayingState;
}

int AudioPlayer::volume() const
{
    return m_player->volume();
}

qint64 AudioPlayer::duration() const
{
    return m_player->duration();
}

qint64 AudioPlayer::position() const
{
    return m_player->position();
}

QString AudioPlayer::currentTrack() const
{
    return m_currentTrack;
}

void AudioPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch(status) {
        case QMediaPlayer::LoadedMedia:
            qDebug() << "Медиа загружена";
            emit durationChanged(m_player->duration());
            break;
        case QMediaPlayer::EndOfMedia:
            qDebug() << "Конец трека";
            emit playbackStateChanged(false);
            break;
        case QMediaPlayer::InvalidMedia:
            qDebug() << "Невалидное медиа";
            emit errorOccurred("Невалидное медиа");
            break;
        case QMediaPlayer::NoMedia:
            qDebug() << "Нет загруженного медиа";
            break;
        default:
            qDebug() << "Статус медиа изменён:" << status;
            break;
    }
}

void AudioPlayer::onPlayerError(QMediaPlayer::Error error)
{
    QString errorMessage;
    switch(error) {
        case QMediaPlayer::NoError:
            return;
        case QMediaPlayer::ResourceError:
            errorMessage = "Ошибка ресурса";
            break;
        case QMediaPlayer::FormatError:
            errorMessage = "Неверный формат файла";
            break;
        case QMediaPlayer::NetworkError:
            errorMessage = "Ошибка сети";
            break;
        case QMediaPlayer::AccessDeniedError:
            errorMessage = "Доступ запрещен";
            break;
        default:
            errorMessage = "Неизвестная ошибка";
            break;
    }
    qDebug() << "Ошибка плеера: " << errorMessage;
    emit errorOccurred(errorMessage);
}

AudioPlayer::~AudioPlayer()
{
    stop();
    qDebug() << "Аудио-плеер уничтожен";
}