#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

class AudioPlayer : public QObject
{
    Q_OBJECT

public: 
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();

    void play();
    void pause();
    void stop();
    void setVolume(int volume);

    void loadTrack(const QString& filePath);
    void setPosition(qint64 position);

    bool isPlaying() const;
    int volume() const;
    qint64 duration() const;
    qint64 position() const;
    QString currentTrack() const;

signals:
    void playbackStateChanged(bool playing);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void volumeChanged(int volume);
    void trackChanged(const QString& trackTitle);
    void errorOccurred(const QString& errorMessage);

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPlayerError(QMediaPlayer::Error error);

private:
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    QString m_currentTrack;
};

#endif