#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QMediaPlayer>
#include <QObject>
#include <QWidget>
#include <QVector>

class QSlider;
class QLabel;
class QToolButton;

struct RadioItem {
    QString url;
    QString title;
    QString artist;
};

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
    void pauseTrack();
    void setVolume(int volume);
    void seek(int position);

    bool isPlaying() const;
    QString currentRadioTitle() const { return m_currentRadioTitle; }

    void setRadioPlaylist(const QVector<RadioItem>& radios);
    void setCurrentRadioIndex(int index);

    void updateTrackInfo(const QString& title, const QString& artist);
    void playTrack(const QString& filePath);

signals:
    void trackFinished();
    void radioStateChanged(bool isPlaying, const QString& title);

private slots:
    void onPlaybackStateChanged();

private:
    void updateNowPlayingLabel();
    void updateVolumeIcon();
    void playCurrentFromPlaylist();
    void playPrev();
    void playNext();
    void togglePlayPause();

    QMediaPlayer *mediaPlayer = nullptr;
    QWidget *playerControls = nullptr;
    QLabel *nowPlayingLabel = nullptr;

    QToolButton *previousBtn = nullptr;
    QToolButton *playPauseBtn = nullptr;
    QToolButton *stopBtn = nullptr;
    QToolButton *nextBtn = nullptr;

    QToolButton *volumeIcon = nullptr;
    QSlider *volumeSlider = nullptr;

    QVector<RadioItem> m_radios;
    int m_currentIndex = -1;

    QString m_currentRadioTitle;
    QString m_currentRadioArtist;
};

#endif
