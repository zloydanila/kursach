#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QMediaPlayer>
#include <QObject>
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

    void playRadio(const QString& streamUrl, const QString& title, const QString& artist);
    void stopRadio();
    void playTrack(const QString& filePath);
    void pauseTrack();
    void setVolume(int volume);
    void seek(int position);

    bool isPlaying() const;
    QString formatTime(qint64 milliseconds);
    QString currentRadioTitle() const { return m_currentRadioTitle; }

public slots:
    void playSelectedTrack();
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void seekTrack(int position);
    void onPlaybackStateChanged();
    void updatePlaybackButtons();
    void updateTrackInfo(const QString& title, const QString& artist);
    void onVolumeIconClicked();

signals:
    void trackFinished();
    void radioStateChanged(bool isPlaying, const QString& title);

private:
    void updateNowPlayingLabel();
    void updateVolumeIcon();

    QMediaPlayer *mediaPlayer;
    QWidget *playerControls;
    QLabel *nowPlayingLabel;
    QToolButton *previousBtn;
    QToolButton *nextBtn;
    QToolButton *volumeIcon;
    QSlider *volumeSlider;
    QToolButton *stopBtn;

    QString m_currentRadioTitle;
    QString m_currentRadioArtist;
    bool m_isRadioPlaying = false;
};

#endif
