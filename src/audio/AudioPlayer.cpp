#include "AudioPlayer.h"
#include <QSlider>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>
#include <QTime>
#include <QApplication>
#include <QGraphicsDropShadowEffect>

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , mediaPlayer(new QMediaPlayer(this))
    , playerControls(nullptr)
    , nowPlayingLabel(nullptr)
    , currentTimeLabel(nullptr)
    , totalTimeLabel(nullptr)
    , progressSlider(nullptr)
    , playPauseBtn(nullptr)
    , previousBtn(nullptr)
    , nextBtn(nullptr)
    , volumeSlider(nullptr)
{
    mediaPlayer->setVolume(50);
}

AudioPlayer::~AudioPlayer()
{
}

void AudioPlayer::setupPlayerControls(QWidget *parent)
{
    playerControls = new QWidget(parent);
    playerControls->setFixedHeight(100);
    playerControls->setStyleSheet(R"(
        QWidget {
            background: rgba(15, 15, 20, 0.95);
            border-top: 1px solid rgba(255, 255, 255, 0.05);
        }
    )");
    
    QHBoxLayout *layout = new QHBoxLayout(playerControls);
    layout->setContentsMargins(30, 15, 30, 15);
    layout->setSpacing(20);
    
    // Левая часть: информация о треке
    QWidget *trackInfoWidget = new QWidget();
    trackInfoWidget->setFixedWidth(250);
    QVBoxLayout *trackLayout = new QVBoxLayout(trackInfoWidget);
    trackLayout->setContentsMargins(0, 0, 0, 0);
    trackLayout->setSpacing(5);
    
    nowPlayingLabel = new QLabel("Не воспроизводится");
    nowPlayingLabel->setStyleSheet(R"(
        QLabel {
            color: #FFFFFF;
            font-size: 14px;
            font-weight: 600;
            background: transparent;
        }
    )");
    nowPlayingLabel->setWordWrap(true);
    
    QLabel *artistLabel = new QLabel("SoundSpace");
    artistLabel->setStyleSheet(R"(
        QLabel {
            color: rgba(255, 255, 255, 0.6);
            font-size: 12px;
            background: transparent;
        }
    )");
    
    trackLayout->addWidget(nowPlayingLabel);
    trackLayout->addWidget(artistLabel);
    layout->addWidget(trackInfoWidget);
    
    // Центральная часть: управление воспроизведением
    QWidget *controlsWidget = new QWidget();
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setSpacing(10);
    
    QHBoxLayout *playbackLayout = new QHBoxLayout();
    playbackLayout->setAlignment(Qt::AlignCenter);
    playbackLayout->setSpacing(15);
    
    // Стили для кнопок плеера
    QString playerBtnStyle = R"(
        QToolButton {
            background: transparent;
            border: none;
            color: rgba(255, 255, 255, 0.7);
            border-radius: 15px;
        }
        QToolButton:hover {
            color: #FFFFFF;
            background: rgba(255, 255, 255, 0.1);
        }
    )";
    
    previousBtn = new QToolButton();
    previousBtn->setIcon(QIcon::fromTheme("media-skip-backward"));
    previousBtn->setStyleSheet(playerBtnStyle);
    previousBtn->setFixedSize(40, 40);
    
    playPauseBtn = new QToolButton();
    playPauseBtn->setIcon(QIcon::fromTheme("media-playback-start"));
    playPauseBtn->setStyleSheet(R"(
        QToolButton {
            background: #8A2BE2;
            border: none;
            border-radius: 20px;
            color: white;
            padding: 10px;
        }
        QToolButton:hover {
            background: #9B4BFF;
        }
    )");
    playPauseBtn->setFixedSize(50, 50);
    
    nextBtn = new QToolButton();
    nextBtn->setIcon(QIcon::fromTheme("media-skip-forward"));
    nextBtn->setStyleSheet(playerBtnStyle);
    nextBtn->setFixedSize(40, 40);
    
    playbackLayout->addWidget(previousBtn);
    playbackLayout->addWidget(playPauseBtn);
    playbackLayout->addWidget(nextBtn);
    
    // Прогресс бар с временем
    QHBoxLayout *progressLayout = new QHBoxLayout();
    progressLayout->setSpacing(10);
    
    currentTimeLabel = new QLabel("0:00");
    currentTimeLabel->setStyleSheet(R"(
        QLabel {
            color: rgba(255, 255, 255, 0.5);
            font-size: 11px;
            min-width: 40px;
        }
    )");
    
    progressSlider = new QSlider(Qt::Horizontal);
    progressSlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            background: rgba(255, 255, 255, 0.1);
            height: 4px;
            border-radius: 2px;
        }
        QSlider::sub-page:horizontal {
            background: #8A2BE2;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #FFFFFF;
            width: 12px;
            height: 12px;
            margin: -4px 0;
            border-radius: 6px;
        }
        QSlider::handle:horizontal:hover {
            background: #8A2BE2;
            width: 14px;
            height: 14px;
            margin: -5px 0;
        }
    )");
    
    totalTimeLabel = new QLabel("0:00");
    totalTimeLabel->setStyleSheet(R"(
        QLabel {
            color: rgba(255, 255, 255, 0.5);
            font-size: 11px;
            min-width: 40px;
        }
    )");
    
    progressLayout->addWidget(currentTimeLabel);
    progressLayout->addWidget(progressSlider, 1);
    progressLayout->addWidget(totalTimeLabel);
    
    controlsLayout->addLayout(playbackLayout);
    controlsLayout->addLayout(progressLayout);
    layout->addWidget(controlsWidget, 1);
    
    // Правая часть: дополнительные функции
    QWidget *rightWidget = new QWidget();
    rightWidget->setFixedWidth(200);
    QHBoxLayout *rightLayout = new QHBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(15);
    
    // Кнопка добавления в избранное
    QToolButton *likeBtn = new QToolButton();
    likeBtn->setText("♡");
    likeBtn->setStyleSheet(R"(
        QToolButton {
            background: transparent;
            border: none;
            color: rgba(255, 255, 255, 0.6);
            font-size: 16px;
        }
        QToolButton:hover {
            color: #FF4444;
        }
        QToolButton[liked="true"] {
            color: #FF4444;
        }
    )");
    
    // Ползунок громкости
    QWidget *volumeWidget = new QWidget();
    QHBoxLayout *volumeLayout = new QHBoxLayout(volumeWidget);
    volumeLayout->setContentsMargins(0, 0, 0, 0);
    volumeLayout->setSpacing(8);
    
    QToolButton *volumeIcon = new QToolButton();
    volumeIcon->setText("🔊");
    volumeIcon->setStyleSheet(playerBtnStyle);
    
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setFixedWidth(80);
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
    
    rightLayout->addWidget(likeBtn);
    rightLayout->addWidget(volumeWidget);
    layout->addWidget(rightWidget);
    
    // Подключаем сигналы
    connect(playPauseBtn, &QToolButton::clicked, this, &AudioPlayer::playSelectedTrack);
    connect(previousBtn, &QToolButton::clicked, this, &AudioPlayer::trackFinished);
    connect(nextBtn, &QToolButton::clicked, this, &AudioPlayer::trackFinished);
    connect(progressSlider, &QSlider::sliderMoved, this, &AudioPlayer::seekTrack);
    
    // Исправленный connect для volumeSlider
    connect(volumeSlider, &QSlider::valueChanged, this, [this, volumeIcon](int value) {
        mediaPlayer->setVolume(value);
        
        // Обновляем иконку громкости
        if (value == 0) {
            volumeIcon->setText("🔇");
        } else if (value < 33) {
            volumeIcon->setText("🔈");
        } else if (value < 66) {
            volumeIcon->setText("🔉");
        } else {
            volumeIcon->setText("🔊");
        }
    });
    
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &AudioPlayer::onPositionChanged);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &AudioPlayer::onDurationChanged);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &AudioPlayer::onPlaybackStateChanged);
    
    // Кнопка лайка
    connect(likeBtn, &QToolButton::clicked, likeBtn, [likeBtn]() {
        bool liked = likeBtn->property("liked").toBool();
        likeBtn->setProperty("liked", !liked);
        likeBtn->setStyleSheet(likeBtn->styleSheet());
        likeBtn->setText(!liked ? "❤️" : "♡");
    });
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
    volumeSlider->setValue(volume);
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
        playPauseBtn->setIcon(QIcon::fromTheme("media-playback-pause"));
    } else {
        playPauseBtn->setIcon(QIcon::fromTheme("media-playback-start"));
    }
}

void AudioPlayer::updateTrackInfo(const QString& title, const QString& artist)
{
    if (nowPlayingLabel) {
        nowPlayingLabel->setText(title);
        // Обновляем artistLabel если нужно
        QLabel *artistLabel = nowPlayingLabel->parentWidget()->findChild<QLabel*>();
        if (artistLabel) {
            artistLabel->setText(artist);
        }
    }
}