#include "AudioPlayer.h"
#include <QSlider>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QStyle>
#include <QApplication>
#include <QDebug>

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , mediaPlayer(new QMediaPlayer(this))
    , playerControls(nullptr)
    , nowPlayingLabel(nullptr)
    , previousBtn(nullptr)
    , nextBtn(nullptr)
    , volumeIcon(nullptr)
    , volumeSlider(nullptr)
    , stopBtn(nullptr)
{
    mediaPlayer->setVolume(50);
    
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &AudioPlayer::onPlaybackStateChanged);
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia || status == QMediaPlayer::InvalidMedia) {
            stopRadio();
        }
    });
}

AudioPlayer::~AudioPlayer() {}

void AudioPlayer::setupPlayerControls(QWidget *parent)
{
    playerControls = new QWidget(parent);
    playerControls->setFixedHeight(80);
    playerControls->setStyleSheet(R"(
        QWidget {
            background: rgba(15, 15, 20, 0.95);
            border-top: 1px solid rgba(255, 255, 255, 0.05);
        }
    )");
    
    QHBoxLayout *layout = new QHBoxLayout(playerControls);
    layout->setContentsMargins(30, 10, 30, 10);
    layout->setSpacing(20);
    
    nowPlayingLabel = new QLabel("Нет активной радиостанции");
    nowPlayingLabel->setStyleSheet(R"(
        QLabel {
            color: #FFFFFF;
            font-size: 14px;
            font-weight: 600;
            background: transparent;
        }
    )");
    nowPlayingLabel->setMinimumWidth(200);
    nowPlayingLabel->setMaximumWidth(300);
    layout->addWidget(nowPlayingLabel);
    layout->addStretch();
    
    QString playerBtnStyle = R"(
        QToolButton {
            background: transparent;
            border: none;
            color: rgba(255, 255, 255, 0.7);
            border-radius: 15px;
            font-size: 16px;
        }
        QToolButton:hover {
            color: #FFFFFF;
            background: rgba(255, 255, 255, 0.1);
        }
    )";
    
    previousBtn = new QToolButton();
    previousBtn->setText("⏮");
    previousBtn->setStyleSheet(playerBtnStyle);
    previousBtn->setFixedSize(40, 40);
    layout->addWidget(previousBtn);

    stopBtn = new QToolButton();
    nextBtn->setText("⏭");
    nextBtn->setStyleSheet(playerBtnStyle);
    nextBtn->setFixedSize(40, 40);
    layout->addWidget(nextBtn);
    
    stopBtn->setText("⏹");
    stopBtn->setStyleSheet(playerBtnStyle);
    stopBtn->setFixedSize(40, 40);
    connect(stopBtn, &QToolButton::clicked, this, &AudioPlayer::stopRadio);
    layout->addWidget(stopBtn);

    nextBtn = new QToolButton();
    nextBtn->setText("⏭");
    nextBtn->setStyleSheet(playerBtnStyle);
    nextBtn->setFixedSize(40, 40);
    layout->addWidget(nextBtn);

    layout->addSpacing(20);

    QWidget *volumeWidget = new QWidget();
    QHBoxLayout *volumeLayout = new QHBoxLayout(volumeWidget);
    volumeLayout->setContentsMargins(0, 0, 0, 0);
    volumeLayout->setSpacing(8);
    
    volumeIcon = new QToolButton();
    volumeIcon->setText("🔊");
    volumeIcon->setStyleSheet(playerBtnStyle);
    volumeIcon->setFixedSize(35, 35);
    connect(volumeIcon, &QToolButton::clicked, this, &AudioPlayer::onVolumeIconClicked);
    
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setFixedWidth(100);
    volumeSlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            background: rgba(255, 255, 255, 0.1);
            height: 4px;
            border-radius: 2px;
        }
        QSlider::sub-page:horizontal {
            background: rgba(255, 255, 255, 0.3);
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #FFFFFF;
            width: 10px;
            height: 10px;
            margin: -3px 0;
            border-radius: 5px;
        }
        QSlider::handle:horizontal:hover {
            background: #8A2BE2;
        }
    )");
    
    volumeLayout->addWidget(volumeIcon);
    volumeLayout->addWidget(volumeSlider);
    layout->addWidget(volumeWidget);
    
    connect(volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        mediaPlayer->setVolume(value);
        updateVolumeIcon();
    });
    
    updateNowPlayingLabel();
    updateVolumeIcon();
}

void AudioPlayer::playRadio(const QString& streamUrl, const QString& title, const QString& artist)
{
    qDebug() << "Playing radio station:" << title << streamUrl;
    mediaPlayer->setMedia(QUrl(streamUrl));
    mediaPlayer->play();
    m_currentRadioTitle = title;
    m_currentRadioArtist = artist;
    m_isRadioPlaying = true;
    updateNowPlayingLabel();
    emit radioStateChanged(true, title);
}

void AudioPlayer::stopRadio()
{
    mediaPlayer->stop();
    m_isRadioPlaying = false;
    m_currentRadioTitle.clear();
    m_currentRadioArtist.clear();
    updateNowPlayingLabel();
    emit radioStateChanged(false, "");
}

void AudioPlayer::playTrack(const QString& filePath)
{
    mediaPlayer->setMedia(QUrl::fromLocalFile(filePath));
    mediaPlayer->play();
    m_isRadioPlaying = false;
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
    if (volumeSlider) {
        volumeSlider->setValue(volume);
    }
    updateVolumeIcon();
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
{}

void AudioPlayer::onDurationChanged(qint64 duration)
{}

void AudioPlayer::seekTrack(int position)
{
    mediaPlayer->setPosition(position);
}

void AudioPlayer::onPlaybackStateChanged()
{
    updatePlaybackButtons();
    if (mediaPlayer->state() == QMediaPlayer::StoppedState) {
        stopRadio();
    }
}

void AudioPlayer::updatePlaybackButtons()
{}

void AudioPlayer::updateTrackInfo(const QString& title, const QString& artist)
{
    if (nowPlayingLabel) {
        if (artist.isEmpty()) {
            nowPlayingLabel->setText(title);
        } else {
            nowPlayingLabel->setText(QString("%1 - %2").arg(title, artist));
        }
    }
}

void AudioPlayer::onVolumeIconClicked()
{
    int currentVolume = volumeSlider->value();
    if (currentVolume > 0) {
        volumeSlider->setValue(0);
    } else {
        volumeSlider->setValue(50);
    }
}

void AudioPlayer::updateNowPlayingLabel()
{
    if (nowPlayingLabel) {
        if (m_isRadioPlaying && !m_currentRadioTitle.isEmpty()) {
            QString displayText = m_currentRadioTitle;
            if (!m_currentRadioArtist.isEmpty()) {
                displayText = QString("%1 - %2").arg(m_currentRadioTitle, m_currentRadioArtist);
            }
            nowPlayingLabel->setText(displayText);
        } else {
            nowPlayingLabel->setText("Нет активной радиостанции");
        }
    }
}

void AudioPlayer::updateVolumeIcon()
{
    if (volumeIcon) {
        int volume = volumeSlider->value();
        if (volume == 0) {
            volumeIcon->setText("🔇");
        } else if (volume < 33) {
            volumeIcon->setText("🔈");
        } else if (volume < 66) {
            volumeIcon->setText("🔉");
        } else {
            volumeIcon->setText("🔊");
        }
    }
}
