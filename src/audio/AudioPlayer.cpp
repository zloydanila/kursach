#include "AudioPlayer.h"

#include <QSlider>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QStyle>
#include <QApplication>
#include <QUrl>

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , mediaPlayer(new QMediaPlayer(this))
{
    mediaPlayer->setVolume(50);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &AudioPlayer::onPlaybackStateChanged);
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
    layout->setSpacing(14);

    nowPlayingLabel = new QLabel("Нет активной радиостанции");
    nowPlayingLabel->setStyleSheet("color: #FFFFFF; font-size: 14px; font-weight: 600; background: transparent;");
    nowPlayingLabel->setMinimumWidth(220);
    nowPlayingLabel->setMaximumWidth(420);
    layout->addWidget(nowPlayingLabel);
    layout->addStretch();

    const QString playerBtnStyle = R"(
        QToolButton {
            background: rgba(155, 75, 255, 0.35);
            border: 1px solid rgba(155, 75, 255, 0.22);
            border-radius: 16px;
        }
        QToolButton:hover { background: rgba(155, 75, 255, 0.55); }
        QToolButton:pressed { background: rgba(155, 75, 255, 0.75); }
    )";

    previousBtn = new QToolButton();
    previousBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipBackward));
    previousBtn->setIconSize(QSize(22, 22));
    previousBtn->setStyleSheet(playerBtnStyle);
    previousBtn->setFixedSize(44, 44);
    connect(previousBtn, &QToolButton::clicked, this, &AudioPlayer::playPrev);
    layout->addWidget(previousBtn);

    playPauseBtn = new QToolButton();
    playPauseBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
    playPauseBtn->setIconSize(QSize(24, 24));
    playPauseBtn->setStyleSheet(playerBtnStyle);
    playPauseBtn->setFixedSize(48, 48);
    connect(playPauseBtn, &QToolButton::clicked, this, &AudioPlayer::togglePlayPause);
    layout->addWidget(playPauseBtn);

    nextBtn = new QToolButton();
    nextBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipForward));
    nextBtn->setIconSize(QSize(22, 22));
    nextBtn->setStyleSheet(playerBtnStyle);
    nextBtn->setFixedSize(44, 44);
    connect(nextBtn, &QToolButton::clicked, this, &AudioPlayer::playNext);
    layout->addWidget(nextBtn);

    layout->addSpacing(16);

    QWidget *volumeWidget = new QWidget();
    QHBoxLayout *volumeLayout = new QHBoxLayout(volumeWidget);
    volumeLayout->setContentsMargins(0, 0, 0, 0);
    volumeLayout->setSpacing(5);

    volumeIcon = new QToolButton();
    volumeIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaVolume));
    volumeIcon->setIconSize(QSize(16, 16));
    volumeIcon->setStyleSheet(playerBtnStyle);
    volumeIcon->setFixedSize(34, 34);
    volumeLayout->addWidget(volumeIcon);

    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(mediaPlayer->volume());
    volumeSlider->setFixedWidth(130);
    volumeSlider->setStyleSheet(R"(
        QSlider::groove:horizontal { background: rgba(255,255,255,0.10); height: 4px; border-radius: 2px; }
        QSlider::sub-page:horizontal { background: rgba(155,75,255,0.65); border-radius: 2px; }
        QSlider::handle:horizontal { background: #FFFFFF; width: 12px; height: 12px; margin: -4px 0; border-radius: 6px; }
        QSlider::handle:horizontal:hover { background: #9B4BFF; }
    )");
    connect(volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        mediaPlayer->setVolume(value);
        if (value == 0) volumeIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        else volumeIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaVolume));
    });
    volumeLayout->addWidget(volumeSlider);

    layout->addWidget(volumeWidget);

    updateNowPlayingLabel();
    onPlaybackStateChanged();
}

void AudioPlayer::setRadioPlaylist(const QVector<RadioItem>& radios)
{
    m_radios = radios;
    if (m_radios.isEmpty()) {
        m_currentIndex = -1;
        return;
    }
    if (m_currentIndex < 0 || m_currentIndex >= m_radios.size())
        m_currentIndex = 0;
}

void AudioPlayer::setCurrentRadioIndex(int index)
{
    if (index < 0 || index >= m_radios.size()) {
        m_currentIndex = -1;
        return;
    }
    m_currentIndex = index;
}

void AudioPlayer::playRadio(const QString& streamUrl, const QString& title, const QString& artist)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    mediaPlayer->setSource(QUrl(streamUrl));
#else
    mediaPlayer->setMedia(QUrl(streamUrl));
#endif
    mediaPlayer->play();

    m_currentRadioTitle = title;
    m_currentRadioArtist = artist;
    updateNowPlayingLabel();
    emit radioStateChanged(true, title);
}

void AudioPlayer::stopRadio()
{
    mediaPlayer->stop();
    m_currentRadioTitle.clear();
    m_currentRadioArtist.clear();
    updateNowPlayingLabel();
    emit radioStateChanged(false, "");
}

bool AudioPlayer::isPlaying() const
{
    return mediaPlayer->state() == QMediaPlayer::PlayingState;
}

void AudioPlayer::onPlaybackStateChanged()
{
    if (!playPauseBtn) return;
    if (mediaPlayer->state() == QMediaPlayer::PlayingState)
        playPauseBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPause));
    else
        playPauseBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
}

void AudioPlayer::updateNowPlayingLabel()
{
    if (!nowPlayingLabel) return;

    if (!m_currentRadioTitle.isEmpty()) {
        QString t = m_currentRadioTitle;
        if (!m_currentRadioArtist.isEmpty())
            t = QString("%1 - %2").arg(m_currentRadioTitle, m_currentRadioArtist);
        nowPlayingLabel->setText(t);
    } else {
        nowPlayingLabel->setText("Нет активной радиостанции");
    }
}

void AudioPlayer::playCurrentFromPlaylist()
{
    if (m_radios.isEmpty()) return;
    if (m_currentIndex < 0 || m_currentIndex >= m_radios.size())
        m_currentIndex = 0;

    const auto &r = m_radios[m_currentIndex];
    playRadio(r.url, r.title, r.artist);
}

void AudioPlayer::playPrev()
{
    if (m_radios.isEmpty()) return;
    if (m_currentIndex < 0) m_currentIndex = 0;
    m_currentIndex = (m_currentIndex - 1 + m_radios.size()) % m_radios.size();
    playCurrentFromPlaylist();
}

void AudioPlayer::playNext()
{
    if (m_radios.isEmpty()) return;
    if (m_currentIndex < 0) m_currentIndex = 0;
    m_currentIndex = (m_currentIndex + 1) % m_radios.size();
    playCurrentFromPlaylist();
}

void AudioPlayer::togglePlayPause()
{
    // Одна кнопка: если уже есть источник — просто play/pause
    if (mediaPlayer->mediaStatus() != QMediaPlayer::NoMedia) {
        if (mediaPlayer->state() == QMediaPlayer::PlayingState) mediaPlayer->pause();
        else mediaPlayer->play();
        return;
    }

    // Если источника нет — стартуем текущую/первую станцию
    playCurrentFromPlaylist();
}

void AudioPlayer::pauseTrack()
{
    // оставлено для совместимости если где-то вызывается
    togglePlayPause();
}

void AudioPlayer::setVolume(int volume)
{
    mediaPlayer->setVolume(volume);
    if (volumeSlider) volumeSlider->setValue(volume);
}

void AudioPlayer::seek(int position)
{
    mediaPlayer->setPosition(position);
}

void AudioPlayer::updateTrackInfo(const QString& title, const QString& artist)
{
    m_currentRadioTitle = title;
    m_currentRadioArtist = artist;
    updateNowPlayingLabel();
}

void AudioPlayer::playTrack(const QString& filePath)
{
    playRadio(filePath, m_currentRadioTitle.isEmpty() ? "Радио" : m_currentRadioTitle, m_currentRadioArtist);
}
