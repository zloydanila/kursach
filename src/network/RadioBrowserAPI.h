#ifndef RADIOBROWSERAPIH
#define RADIOBROWSERAPIH

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVariant>
#include <core/models/Track.h>

class RadioBrowserAPI : public QObject
{
    Q_OBJECT

public:
    explicit RadioBrowserAPI(QObject *parent = nullptr);
    void searchStations(const QString &query, int limit = 50);
    void getTopStations(int limit = 50);
    void getStationsByGenre(const QString &genre, int limit = 50);

signals:
    void stationsReady(const QVariantList &stations);
    void errorOccurred(const QString &error);

private slots:
    void onSearchFinished();

private:
    TrackData parseStation(const QJsonObject &obj);
    QNetworkAccessManager mnetwork;
};

#endif
