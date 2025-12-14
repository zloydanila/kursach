#ifndef TRACKLISTITEM_H
#define TRACKLISTITEM_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "core/models/Track.h"

class TrackListItem : public QWidget
{
    Q_OBJECT

public:
    explicit TrackListItem(const TrackData& track, bool showAlbum = false, QWidget *parent = nullptr);
    
    int trackId() const { return m_trackId; }
    const TrackData& track() const { return m_track; }

signals:
    void playClicked(int trackId);
    void moreClicked(int trackId, const QPoint& pos);

private:
    void setupUI(bool showAlbum);
    QString formatDuration(int seconds);
    
    int m_trackId;
    TrackData m_track;
    
    QLabel* m_numberLabel;
    QLabel* m_titleLabel;
    QLabel* m_artistLabel;
    QLabel* m_albumLabel;
    QLabel* m_durationLabel;
    QPushButton* m_playButton;
    QPushButton* m_moreButton;
};

#endif
