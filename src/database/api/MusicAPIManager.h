#ifndef MUSICAPIMANAGER_H
#define MUSICAPIMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>

class MusicAPIManager : public QObject
{
    Q_OBJECT

public:
    explicit MusicAPIManager(QObject *parent = nullptr);
    void searchTracks(const QString& query, int userId);
    void getTopTracks(int userId);
    void searchTracksWithAudio(const QString& query, int userId);
    void playTrackFromAPI(const QString& trackId, const QString& service);

signals:
    void tracksFound(const QVariantList& tracks);
    void jamendoTracksFound(const QVariantList& tracks);
    void trackAudioUrlReady(const QString& trackId, const QString& audioUrl);
    void networkError(const QString& error); // ДОБАВИТЬ ЭТОТ СИГНАЛ

private slots:
    void handleNetworkResponse(QNetworkReply *reply);

private:
    void processSearchResponse(const QByteArray& responseData);
    void processTopTracksResponse(const QByteArray& responseData);
    void processTracksData(const QJsonArray& tracksArray);
    void processJamendoSearchResponse(const QByteArray& responseData);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    QString m_baseUrl;
    QString m_jamendoClientId;
    QString m_jamendoBaseUrl;
    int m_currentUserId;
};

#endif // MUSICAPIMANAGER_H