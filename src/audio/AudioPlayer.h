#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QMediaPlayer>
#include <QWidget>

class QSlider;
class QLabel;
class QToolButton;

class AudioPlayer : public QObject
{
    Q_OBJECT

public:
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();

    void setupPlayerControls(QWidget *parent);
    QWidget* getPlayerControls() const { return playerControls; }

    void playTrack(const QString& filePath);
    void pauseTrack();
    void setVolume(int volume);
    void seek(int position);

    bool isPlaying() const;
    QString formatTime(qint64 milliseconds);

public slots:
    void playSelectedTrack();
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void seekTrack(int position);
    void onPlaybackStateChanged();
    void updatePlaybackButtons();
    void updateTrackInfo(const QString& title, const QString& artist);

signals:
    void trackFinished();

private:
    QMediaPlayer *mediaPlayer;
    QWidget *playerControls;
    QLabel *nowPlayingLabel;
    QLabel *currentTimeLabel;
    QLabel *totalTimeLabel;
    QSlider *progressSlider;
    QToolButton *playPauseBtn;
    QToolButton *previousBtn;
    QToolButton *nextBtn;
    QSlider *volumeSlider;
};

#endif 