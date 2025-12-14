#ifndef MYMUSICPAGEH
#define MYMUSICPAGEH

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMediaPlayer>
#include <core/models/Track.h>

class TrackManager;
class LocalFileHandler;
class AudioPlayer;

class MyMusicPage : public QWidget
{
    Q_OBJECT

public:
    explicit MyMusicPage(int userId, QWidget *parent = nullptr);
    ~MyMusicPage();

    void loadRadioStations();

signals:
    void playTrackRequested(const TrackData &track);

private slots:
    void onTrackDoubleClicked(QListWidgetItem *item);
    void onSearchChanged(const QString &text);
    void onTrackContextMenu(const QPoint &pos);
    void onDeleteTrack(int trackId);
    void onRefreshClicked();
    void onPlayRadioClicked();
    void onRadioListContextMenu(const QPoint &pos);
    void onPlayRadio(int stationId);
    void onDeleteRadio(int stationId);

private:
    void setupUI();
    void loadTracks();
    void displayTracks(const QVector<TrackData> &tracks);
    void displayRadioStations(const QVector<TrackData> &stations);

    int muserId;
    TrackManager *mtrackManager;
    LocalFileHandler *mfileHandler;
    AudioPlayer *maudioPlayer;
    QMediaPlayer *mradioPlayer;

    QLabel *mtitleLabel;
    QLabel *mstatsLabel;
    QPushButton *mrefreshButton;
    QPushButton *mplayRandomButton;
    QLineEdit *msearchEdit;
    QListWidget *mtracksList;
    QListWidget *mradioList;

    QVector<TrackData> mallTracks;
    QVector<TrackData> mallRadioStations;
};

#endif