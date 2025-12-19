#include "TrackListItem.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

TrackListItem::TrackListItem(const TrackData& track, bool showAlbum, QWidget *parent)
    : QWidget(parent)
    , m_trackId(track.id)
    , m_track(track)
{
    setupUI(showAlbum);
}

void TrackListItem::setupUI(bool showAlbum)
{
    setFixedHeight(65);
    setCursor(Qt::PointingHandCursor);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(15, 8, 15, 8);
    mainLayout->setSpacing(15);
    
    m_numberLabel = new QLabel(QString::number(m_track.position + 1));
    m_numberLabel->setFixedWidth(40);
    m_numberLabel->setAlignment(Qt::AlignCenter);
    m_numberLabel->setStyleSheet("color: rgba(255, 255, 255, 0.5); font-size: 14px;");
    
    m_playButton = new QPushButton("▶");
    m_playButton->setFixedSize(35, 35);
    m_playButton->setStyleSheet(R"(
        QPushButton {
            background: rgba(138, 43, 226, 0.2);
            border: 1px solid #8A2BE2;
            color: #8A2BE2;
            border-radius: 17px;
            font-size: 12px;
        }
        QPushButton:hover {
            background: #8A2BE2;
            color: white;
        }
    )");
    m_playButton->hide();
    
    QWidget* infoWidget = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(3);
    
    m_titleLabel = new QLabel(m_track.title);
    m_titleLabel->setStyleSheet("color: white; font-size: 15px; font-weight: 600;");
    
    m_artistLabel = new QLabel(m_track.artist);
    m_artistLabel->setStyleSheet("color: rgba(255, 255, 255, 0.6); font-size: 13px;");
    
    infoLayout->addWidget(m_titleLabel);
    infoLayout->addWidget(m_artistLabel);
    
    if (showAlbum) {
        m_albumLabel = new QLabel(m_track.album);
        m_albumLabel->setStyleSheet("color: rgba(255, 255, 255, 0.5); font-size: 13px;");
        m_albumLabel->setFixedWidth(200);
    }
    
    m_durationLabel = new QLabel(formatDuration(m_track.duration));
    m_durationLabel->setStyleSheet("color: rgba(255, 255, 255, 0.5); font-size: 14px;");
    m_durationLabel->setFixedWidth(60);
    
    m_moreButton = new QPushButton("⋮");
    m_moreButton->setFixedSize(30, 30);
    m_moreButton->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            color: rgba(255, 255, 255, 0.5);
            font-size: 18px;
            border-radius: 15px;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.1);
            color: white;
        }
    )");
    
    mainLayout->addWidget(m_numberLabel);
    mainLayout->addWidget(m_playButton);
    mainLayout->addWidget(infoWidget, 1);
    
    if (showAlbum && m_albumLabel) {
        mainLayout->addWidget(m_albumLabel);
    }
    
    mainLayout->addWidget(m_durationLabel);
    mainLayout->addWidget(m_moreButton);
    
    connect(m_playButton, &QPushButton::clicked, [this]() {
        emit playClicked(m_trackId);
    });
    
    connect(m_moreButton, &QPushButton::clicked, [this]() {
        emit moreClicked(m_trackId, m_moreButton->mapToGlobal(m_moreButton->rect().bottomLeft()));
    });
    
    setStyleSheet(R"(
        TrackListItem {
            background: transparent;
            border-radius: 8px;
        }
        TrackListItem:hover {
            background: rgba(255, 255, 255, 0.05);
        }
    )");
}

QString TrackListItem::formatDuration(int seconds)
{
    int minutes = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(minutes).arg(secs, 2, 10, QChar('0'));
}
