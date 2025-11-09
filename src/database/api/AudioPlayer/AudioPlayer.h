#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QMediaPlayer>

class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();

    void loadTrack(const QString &filePath);
    void play();
    void pause();
    void stop();
    void setVolume(int volume);
    int volume() const;
    bool isPlaying() const;
    qint64 duration() const;
    qint64 position() const;
    QString currentTrack() const;

signals:
    void trackChanged(const QString &trackName);
    void playbackStateChanged(bool playing);
    void volumeChanged(int volume);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void errorOccurred(const QString &message);

private slots:
    void onStateChanged(QMediaPlayer::State state);
    void onPlayerError(QMediaPlayer::Error error);

private:
    QMediaPlayer *m_player;
    QString m_currentTrack;
};

#endif // AUDIOPLAYER_H
