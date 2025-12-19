#include "MusicAPIManager.h"
#include <QVariantMap>

MusicAPIManager::MusicAPIManager(QObject *parent)
    : QObject(parent), mradioBrowser(new RadioBrowserAPI(this))
{
    connect(mradioBrowser, &RadioBrowserAPI::stationsReady,
            this, &MusicAPIManager::onRadioBrowserStations);
    connect(mradioBrowser, &RadioBrowserAPI::errorOccurred,
            this, &MusicAPIManager::errorOccurred);
}

void MusicAPIManager::searchStations(const QString &query, int userId)
{
    mradioBrowser->searchStations(query, 50);
}

void MusicAPIManager::getTopStations(int userId)
{
    mradioBrowser->getTopStations(50);
}

void MusicAPIManager::getStationsByGenre(const QString &genre, int userId)
{
    mradioBrowser->getStationsByGenre(genre, 50);
}

void MusicAPIManager::onRadioBrowserStations(const QVariantList &stations)
{
    emit stationsFound(stations);
}
