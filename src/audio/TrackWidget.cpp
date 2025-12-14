#include "TrackWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>

TrackWidget::TrackWidget(const TrackData& track, QWidget *parent)
    : QWidget(parent), m_trackId(track.id)
{
    setupUI(track);
}

void TrackWidget::setupUI(const TrackData& track)
{
    setFixedHeight(60);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(15, 10, 15, 10);
    mainLayout->setSpacing(15);
    
    QWidget* textWidget = new QWidget();
    QVBoxLayout* textLayout = new QVBoxLayout(textWidget);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(3);
    
    m_titleLabel = new QLabel(track.title);
    m_titleLabel->setStyleSheet("color: white; font-size: 15px; font-weight: 600;");
    
    m_artistLabel = new QLabel(track.artist);
    m_artistLabel->setStyleSheet("color: rgba(255, 255, 255, 0.6); font-size: 13px;");
    
    textLayout->addWidget(m_titleLabel);
    textLayout->addWidget(m_artistLabel);
    
    m_durationLabel = new QLabel(formatDuration(track.duration));
    m_durationLabel->setStyleSheet("color: rgba(255, 255, 255, 0.5); font-size: 13px;");
    m_durationLabel->setFixedWidth(60);
    
    m_playButton = new QPushButton("▶");
    m_playButton->setFixedSize(40, 40);
    m_playButton->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 20px;
            font-size: 14px;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
    )");
    
    connect(m_playButton, &QPushButton::clicked, [this]() {
        emit playRequested(m_trackId);
    });
    
    mainLayout->addWidget(textWidget, 1);
    mainLayout->addWidget(m_durationLabel);
    mainLayout->addWidget(m_playButton);
    
    setStyleSheet("TrackWidget { background: transparent; }");
}

QString TrackWidget::formatDuration(int seconds)
{
    int minutes = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(minutes).arg(secs, 2, 10, QChar('0'));
}
