#ifndef MUSICAPIMANAGER_H
#define MUSICAPIMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QVariant>

class MusicAPIManager : public QObject
{
    Q_OBJECT

public:
    explicit MusicAPIManager(QObject *parent = nullptr);
    
    void searchTracks(const QString& query, int userId);
    void getTopTracks(int userId);

signals:
    void tracksFound(const QVariantList& tracks);
    void errorOccurred(const QString& error);

private slots:
    void handleNetworkResponse(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    QString m_baseUrl;
    int m_currentUserId;
    
    void processTracksData(const QJsonArray& tracksArray);
    void processSearchResponse(const QByteArray& responseData);        
    void processTopTracksResponse(const QByteArray& responseData);  
};

#endif 