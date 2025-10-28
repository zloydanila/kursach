#include "audio/AudioPlayer.h"
#include <QSlider>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>
#include <QTime>
#include <QApplication>

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , mediaPlayer(new QMediaPlayer(this))
    , playerControls(nullptr)
{
    mediaPlayer->setVolume(50);
}

AudioPlayer::~AudioPlayer()
{
}

void AudioPlayer::setupPlayerControls(QWidget *parent)
{
    playerControls = new QWidget(parent);
    playerControls->setFixedHeight(80);
    playerControls->setStyleSheet(
        "QWidget {"
        "   background-color: #000000;"
        "   border-top: 1px solid #2A2A2A;"
        "}"
    );
    
    QHBoxLayout *layout = new QHBoxLayout(playerControls);
    layout->setContentsMargins(20, 10, 20, 10);
    
    QWidget *trackInfoWidget = new QWidget();
    QVBoxLayout *trackInfoLayout = new QVBoxLayout(trackInfoWidget);
    trackInfoLayout->setContentsMargins(0, 0, 0, 0);
    
    nowPlayingLabel = new QLabel("Не воспроизводится");
    nowPlayingLabel->setStyleSheet("QLabel { color: white; font-size: 14px; font-weight: bold; }");
    
    QLabel *artistLabel = new QLabel("SoundSpace");
    artistLabel->setStyleSheet("QLabel { color: #BBBBBB; font-size: 12px; }");
    
    trackInfoLayout->addWidget(nowPlayingLabel);
    trackInfoLayout->addWidget(artistLabel);
    
    QWidget *controlsWidget = new QWidget();
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setAlignment(Qt::AlignCenter);
    
    QHBoxLayout *playbackLayout = new QHBoxLayout();
    playbackLayout->setAlignment(Qt::AlignCenter);
    
    previousBtn = new QToolButton();
    previousBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipBackward));
    previousBtn->setStyleSheet(
        "QToolButton {"
        "   background-color: transparent;"
        "   color: white;"
        "   border: none;"
        "   padding: 5px;"
        "}"
        "QToolButton:hover {"
        "   color: #8A2BE2;"
        "}"
    );
    
    playPauseBtn = new QToolButton();
    playPauseBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
    playPauseBtn->setStyleSheet(
        "QToolButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 15px;"
        "   padding: 10px;"
        "   margin: 0 10px;"
        "}"
        "QToolButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );
    
    nextBtn = new QToolButton();
    nextBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipForward));
    nextBtn->setStyleSheet(previousBtn->styleSheet());
    
    playbackLayout->addWidget(previousBtn);
    playbackLayout->addWidget(playPauseBtn);
    playbackLayout->addWidget(nextBtn);
    
    QHBoxLayout *progressLayout = new QHBoxLayout();
    
    currentTimeLabel = new QLabel("0:00");
    currentTimeLabel->setStyleSheet("QLabel { color: #BBBBBB; font-size: 11px; min-width: 40px; }");
    
    progressSlider = new QSlider(Qt::Horizontal);
    progressSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   background-color: #2A2A2A;"
        "   height: 4px;"
        "   border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background-color: #8A2BE2;"
        "   width: 12px;"
        "   height: 12px;"
        "   border-radius: 6px;"
        "   margin: -4px 0;"
        "}"
        "QSlider::sub-page:horizontal {"
        "   background-color: #8A2BE2;"
        "   border-radius: 2px;"
        "}"
    );
    
    totalTimeLabel = new QLabel("0:00");
    totalTimeLabel->setStyleSheet("QLabel { color: #BBBBBB; font-size: 11px; min-width: 40px; }");
    
    progressLayout->addWidget(currentTimeLabel);
    progressLayout->addWidget(progressSlider);
    progressLayout->addWidget(totalTimeLabel);
    
    controlsLayout->addLayout(playbackLayout);
    controlsLayout->addLayout(progressLayout);
    
    QWidget *volumeWidget = new QWidget();
    QHBoxLayout *volumeLayout = new QHBoxLayout(volumeWidget);
    volumeLayout->setContentsMargins(0, 0, 0, 0);
    
    QToolButton *volumeIcon = new QToolButton();
    volumeIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaVolume));
    volumeIcon->setStyleSheet("QToolButton { background-color: transparent; border: none; color: white; }");
    
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setFixedWidth(80);
    volumeSlider->setStyleSheet(progressSlider->styleSheet());
    
    volumeLayout->addWidget(volumeIcon);
    volumeLayout->addWidget(volumeSlider);
    
    layout->addWidget(trackInfoWidget);
    layout->addStretch();
    layout->addWidget(controlsWidget);
    layout->addStretch();
    layout->addWidget(volumeWidget);
    
    connect(playPauseBtn, &QToolButton::clicked, this, &AudioPlayer::playSelectedTrack);
    connect(previousBtn, &QToolButton::clicked, this, &AudioPlayer::trackFinished);
    connect(nextBtn, &QToolButton::clicked, this, &AudioPlayer::trackFinished);
    connect(progressSlider, &QSlider::sliderMoved, this, &AudioPlayer::seekTrack);
    connect(volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        mediaPlayer->setVolume(value);
    });
    
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &AudioPlayer::onPositionChanged);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &AudioPlayer::onDurationChanged);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &AudioPlayer::onPlaybackStateChanged);
}

void AudioPlayer::playTrack(const QString& filePath)
{
    mediaPlayer->setMedia(QUrl::fromLocalFile(filePath));
    mediaPlayer->play();
}

void AudioPlayer::pauseTrack()
{
    if (mediaPlayer->state() == QMediaPlayer::PlayingState) {
        mediaPlayer->pause();
    } else {
        mediaPlayer->play();
    }
}

void AudioPlayer::setVolume(int volume)
{
    mediaPlayer->setVolume(volume);
}

void AudioPlayer::seek(int position)
{
    mediaPlayer->setPosition(position);
}

bool AudioPlayer::isPlaying() const
{
    return mediaPlayer->state() == QMediaPlayer::PlayingState;
}

QString AudioPlayer::formatTime(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    return QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));
}

void AudioPlayer::playSelectedTrack()
{
    pauseTrack();
}

void AudioPlayer::onPositionChanged(qint64 position)
{
    if (!progressSlider->isSliderDown()) {
        progressSlider->setValue(position);
    }
    currentTimeLabel->setText(formatTime(position));
}

void AudioPlayer::onDurationChanged(qint64 duration)
{
    progressSlider->setRange(0, duration);
    totalTimeLabel->setText(formatTime(duration));
}

void AudioPlayer::seekTrack(int position)
{
    mediaPlayer->setPosition(position);
}

void AudioPlayer::onPlaybackStateChanged()
{
    updatePlaybackButtons();
}

void AudioPlayer::updatePlaybackButtons()
{
    if (mediaPlayer->state() == QMediaPlayer::PlayingState) {
        playPauseBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPause));
    } else {
        playPauseBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

void AudioPlayer::updateTrackInfo(const QString& title, const QString& artist)
{
    nowPlayingLabel->setText(title);
}