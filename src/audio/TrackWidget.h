#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QWidget>

class TrackWidget : public QWidget {
    Q_OBJECT
public:
    explicit TrackWidget(const QString& title, const QString& artist, const QString& duration, int trackId, QWidget *parent = nullptr);
    
signals:
    void playRequested(int trackId);
    void addToPlaylistRequested(int trackId);
    
private:
    int m_trackId;
};

#endif // TRACKWIDGET_H