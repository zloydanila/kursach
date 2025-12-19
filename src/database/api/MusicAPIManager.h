#ifndef MUSICAPIMANAGERH
#define MUSICAPIMANAGERH

#include <QObject>
#include <QVector>
#include <core/models/Track.h>
#include <network/RadioBrowserAPI.h>

class MusicAPIManager : public QObject
{
    Q_OBJECT

public:
    explicit MusicAPIManager(QObject *parent = nullptr);
    void searchStations(const QString &query, int userId);
    void getTopStations(int userId);
    void getStationsByGenre(const QString &genre, int userId);

signals:
    void stationsFound(const QVariantList &stations);
    void errorOccurred(const QString &error);

private slots:
    void onRadioBrowserStations(const QVariantList &stations);

private:
    RadioBrowserAPI *mradioBrowser;
};

#endif
