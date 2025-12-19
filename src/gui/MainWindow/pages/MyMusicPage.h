#ifndef MYMUSICPAGE_H
#define MYMUSICPAGE_H

#include <QWidget>
#include <QVector>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include "core/models/Track.h"

class TrackManager;
class LocalFileHandler;

class MyMusicPage : public QWidget
{
    Q_OBJECT
public:
    explicit MyMusicPage(int userId, QWidget *parent = nullptr);
    ~MyMusicPage();

    void addRadioStation(const TrackData &station);

public slots:
    void reloadStations();

signals:
    void playRadioRequested(const QString& url, const QString& title, const QString& country);

    
    void radioPlaylistChanged(const QVector<TrackData>& stations);

   
    void currentRadioIndexChanged(int index);

private slots:
    void onRefreshClicked();
    void onPlayRadioClicked();
    void onSearchChanged(const QString &text);
    void onRadioListContextMenu(const QPoint &pos);
    void onPlayRadio(int stationId);

private:
    void setupUI();
    void loadRadioStations();
    void displayRadioStations(const QVector<TrackData> &stations);

    int muserId;

    TrackManager* mtrackManager;
    LocalFileHandler* mfileHandler;
    void* mradioPlayer;

    QLabel* mtitleLabel;
    QLabel* mstatsLabel;
    QLineEdit* msearchEdit;
    QListWidget* mradioList;
    QPushButton* mrefreshButton;
    QPushButton* mplayRandomButton;

    QVector<TrackData> mallRadioStations;
};

#endif
