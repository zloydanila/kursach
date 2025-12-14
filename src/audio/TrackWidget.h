#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "core/models/Track.h"

class TrackWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrackWidget(const TrackData& track, QWidget *parent = nullptr);
    
    int trackId() const { return m_trackId; }

signals:
    void playRequested(int trackId);
    void addToPlaylistRequested(int trackId);
    void deleteRequested(int trackId);

private:
    void setupUI(const TrackData& track);
    QString formatDuration(int seconds);
    
    int m_trackId;
    QLabel* m_titleLabel;
    QLabel* m_artistLabel;
    QLabel* m_durationLabel;
    QPushButton* m_playButton;
};

#endif
