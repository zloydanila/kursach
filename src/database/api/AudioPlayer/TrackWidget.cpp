#include "TrackWidget.h"

TrackWidget::TrackWidget(const QString& title, const QString& artist, const QString& duration, int trackId, QWidget *parent)
    : QWidget(parent), m_trackId(trackId)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);

    QLabel *titleLabel = new QLabel(title);
    QLabel *artistLabel = new QLabel(artist);
    QLabel *durationLabel = new QLabel(duration);

    titleLabel->setStyleSheet("color: white; font-weight: bold;");
    artistLabel->setStyleSheet("color: #CCCCCC;");
    durationLabel->setStyleSheet("color: #CCCCCC;");

    layout->addWidget(titleLabel, 1);
    layout->addWidget(artistLabel, 1);
    layout->addWidget(durationLabel);

    QPushButton *playButton = new QPushButton("▶");
    playButton->setFixedSize(30, 30);
    playButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );

    QPushButton *addButton = new QPushButton("+");
    addButton->setFixedSize(30, 30);
    addButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2A2A2A;"
        "   color: white;"
        "   border: 1px solid #8A2BE2;"
        "   border-radius: 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #8A2BE2;"
        "}"
    );

    layout->addWidget(playButton);
    layout->addWidget(addButton);

    connect(playButton, &QPushButton::clicked, this, &TrackWidget::onPlayClicked);
    connect(addButton, &QPushButton::clicked, this, &TrackWidget::onAddToPlaylistClicked);
}

void TrackWidget::onPlayClicked()
{
    emit playRequested(m_trackId);
}

void TrackWidget::onAddToPlaylistClicked()
{
    emit addToPlaylistRequested(m_trackId);
}