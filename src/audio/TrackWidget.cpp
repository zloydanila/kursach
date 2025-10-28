#include "TrackWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

TrackWidget::TrackWidget(const QString& title, const QString& artist, const QString& duration, int trackId, QWidget *parent)
    : QWidget(parent), m_trackId(trackId)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    
    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);
    
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("QLabel { color: white; font-weight: bold; font-size: 14px; }");
    
    QLabel *artistLabel = new QLabel(artist);
    artistLabel->setStyleSheet("QLabel { color: #BBBBBB; font-size: 12px; }");
    
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(artistLabel);
    
    QLabel *durationLabel = new QLabel(duration);
    durationLabel->setStyleSheet("QLabel { color: #BBBBBB; font-size: 12px; }");
    
    QPushButton *playBtn = new QPushButton("▶");
    playBtn->setFixedSize(30, 30);
    playBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 15px;"
        "   font-size: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );
    
    connect(playBtn, &QPushButton::clicked, [this]() {
        emit playRequested(m_trackId);
    });
    
    layout->addLayout(textLayout);
    layout->addStretch();
    layout->addWidget(durationLabel);
    layout->addWidget(playBtn);
    
    setStyleSheet("TrackWidget { background-color: transparent; }");
    setFixedHeight(50);
}