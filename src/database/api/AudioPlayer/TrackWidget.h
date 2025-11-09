#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class TrackWidget : public QWidget
{
    Q_OBJECT

public:
    TrackWidget(const QString& title, const QString& artist, const QString& duration, int trackId, QWidget *parent = nullptr);

signals:
    void playRequested(int trackId);
    void addToPlaylistRequested(int trackId);

private slots:
    void onPlayClicked();
    void onAddToPlaylistClicked();

private:
    int m_trackId;
};

#endif // TRACKWIDGET_H