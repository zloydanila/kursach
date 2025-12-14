#include "RoomCard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

RoomCard::RoomCard(const Room& room, QWidget *parent)
    : QWidget(parent), m_roomId(room.id)
{
    setupUI(room);
}

void RoomCard::setupUI(const Room& room)
{
    setFixedHeight(180);
    setStyleSheet(R"(
        QWidget {
            background: rgba(255, 255, 255, 0.03);
            border: 1px solid rgba(255, 255, 255, 0.05);
            border-radius: 12px;
        }
        QWidget:hover {
            background: rgba(255, 255, 255, 0.05);
            border: 1px solid rgba(138, 43, 226, 0.3);
        }
    )");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(12);
    
    QWidget* topRow = new QWidget();
    QHBoxLayout* topLayout = new QHBoxLayout(topRow);
    topLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* iconLabel = new QLabel("🎵");
    iconLabel->setStyleSheet("color: #8A2BE2; font-size: 18px;");
    
    m_nameLabel = new QLabel(room.name);
    m_nameLabel->setStyleSheet("color: white; font-size: 16px; font-weight: 600;");
    m_nameLabel->setWordWrap(true);
    
    m_membersLabel = new QLabel(QString("%1/%2").arg(room.members.size()).arg(room.maxMembers));
    m_membersLabel->setStyleSheet(R"(
        color: rgba(255, 255, 255, 0.7);
        font-size: 13px;
        background: rgba(255, 255, 255, 0.05);
        padding: 4px 10px;
        border-radius: 10px;
    )");
    
    topLayout->addWidget(iconLabel);
    topLayout->addWidget(m_nameLabel, 1);
    topLayout->addWidget(m_membersLabel);
    
    m_genreLabel = new QLabel(room.genre);
    m_genreLabel->setStyleSheet("color: #8A2BE2; font-size: 14px; font-weight: 500;");
    
    m_trackLabel = new QLabel(room.currentTrackTitle.isEmpty() ? 
        "Нет текущего трека" : 
        QString("%1 - %2").arg(room.currentTrackArtist, room.currentTrackTitle));
    m_trackLabel->setStyleSheet("color: rgba(255, 255, 255, 0.7); font-size: 13px; font-style: italic;");
    m_trackLabel->setWordWrap(true);
    
    m_joinButton = new QPushButton("Присоединиться");
    m_joinButton->setFixedHeight(38);
    m_joinButton->setStyleSheet(R"(
        QPushButton {
            background: rgba(138, 43, 226, 0.2);
            border: 1px solid #8A2BE2;
            color: #8A2BE2;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: rgba(138, 43, 226, 0.3);
        }
    )");
    
    connect(m_joinButton, &QPushButton::clicked, [this]() {
        emit joinClicked(m_roomId);
    });
    
    mainLayout->addWidget(topRow);
    mainLayout->addWidget(m_genreLabel);
    mainLayout->addWidget(m_trackLabel, 1);
    mainLayout->addWidget(m_joinButton);
}
