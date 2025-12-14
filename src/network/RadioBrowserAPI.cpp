#include "RadioBrowserAPI.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

RadioBrowserAPI::RadioBrowserAPI(QObject *parent)
    : QObject(parent)
{
}

void RadioBrowserAPI::searchStations(const QString &query, int limit)
{
    QUrl url("https://de1.api.radio-browser.info/json/stations/search");
    QUrlQuery q;
    q.addQueryItem("name", query);
    q.addQueryItem("limit", QString::number(limit));
    q.addQueryItem("order", "votes");
    q.addQueryItem("reverse", "true");
    url.setQuery(q);

    qDebug() << "RadioBrowserAPI::searchStations URL:" << url.toString();
    QNetworkReply *reply = mnetwork.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &RadioBrowserAPI::onSearchFinished);
}

void RadioBrowserAPI::getTopStations(int limit)
{
    QUrl url("https://de1.api.radio-browser.info/json/stations/topclick");
    QUrlQuery q;
    q.addQueryItem("limit", QString::number(limit));
    url.setQuery(q);

    qDebug() << "RadioBrowserAPI::getTopStations URL:" << url.toString();
    QNetworkReply *reply = mnetwork.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &RadioBrowserAPI::onSearchFinished);
}

void RadioBrowserAPI::getStationsByGenre(const QString &genre, int limit)
{
    QString genreLower = genre.toLower();
    QUrl url("https://de1.api.radio-browser.info/json/stations/search");
    QUrlQuery q;
    q.addQueryItem("tag", genreLower);
    q.addQueryItem("limit", QString::number(limit));
    q.addQueryItem("order", "votes");
    q.addQueryItem("reverse", "true");
    url.setQuery(q);

    qDebug() << "RadioBrowserAPI::getStationsByGenre URL:" << url.toString();
    QNetworkReply *reply = mnetwork.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &RadioBrowserAPI::onSearchFinished);
}

void RadioBrowserAPI::onSearchFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "RadioBrowserAPI::onSearchFinished error:" << reply->errorString();
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray arr = doc.array();
    QVariantList stations;

    qDebug() << "RadioBrowserAPI::onSearchFinished - Stations count:" << arr.size();

    for (const QJsonValue &v : arr) {
        TrackData station = parseStation(v.toObject());
        if (!station.title.isEmpty() && !station.streamUrl.isEmpty()) {
            QVariantMap m;
            m["title"] = station.title;
            m["artist"] = station.artist;
            m["streamUrl"] = station.streamUrl;
            m["genre"] = station.album;
            m["bitrate"] = station.bitrate;
            m["favicon"] = station.cover;
            m["id"] = station.id;
            stations.append(m);
        }
    }

    qDebug() << "RadioBrowserAPI::onSearchFinished - Valid stations:" << stations.count();
    emit stationsReady(stations);
}

TrackData RadioBrowserAPI::parseStation(const QJsonObject &obj)
{
    TrackData station;
    station.title = obj.value("name").toString();
    station.artist = obj.value("country").toString();
    station.streamUrl = obj.value("url").toString();
    station.album = obj.value("tags").toString();
    station.bitrate = obj.value("bitrate").toInt();
    station.source = "radiobrowser";
    station.isFromCloud = true;
    station.id = qHash(station.streamUrl);

    QString favicon = obj.value("favicon").toString();
    if (!favicon.isEmpty()) {
        station.cover = favicon;
    }

    return station;
}