#include "../../database/api/MusicAPIManager.h"
#include "../../database/DatabaseManager.h"
#include <QDebug>
#include <QSslError>

MusicAPIManager::MusicAPIManager(QObject *parent) 
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_apiKey("5d63cb6ba6780da3f1306e10539e4b01")  
    , m_baseUrl("https://ws.audioscrobbler.com/2.0/")
    , m_currentUserId(-1)
{
    qDebug() << "MusicAPIManager initialized";
    
    // Настройки сетевого менеджера
    m_networkManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    
    // Подключаем общий обработчик
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &MusicAPIManager::handleNetworkResponse);
}

void MusicAPIManager::searchTracks(const QString& query, int userId)
{
    m_currentUserId = userId;
    
    qDebug() << "=== SEARCH TRACKS START ===";
    qDebug() << "Query:" << query;
    qDebug() << "User ID:" << userId;
    
    QUrl url(m_baseUrl);
    QUrlQuery params;
    params.addQueryItem("method", "track.search");
    params.addQueryItem("track", query);
    params.addQueryItem("api_key", m_apiKey);
    params.addQueryItem("format", "json");
    params.addQueryItem("limit", "10");
    
    url.setQuery(params);
    
    qDebug() << "Request URL:" << url.toString();
    
    QNetworkRequest request(url);
    m_networkManager->get(request);
}

void MusicAPIManager::getTopTracks(int userId)
{
    m_currentUserId = userId;
    
    qDebug() << "=== TOP TRACKS START ===";
    qDebug() << "User ID:" << userId;
    
    QUrl url(m_baseUrl);
    QUrlQuery params;
    params.addQueryItem("method", "chart.gettoptracks");
    params.addQueryItem("api_key", m_apiKey);
    params.addQueryItem("format", "json");
    params.addQueryItem("limit", "10");
    
    url.setQuery(params);
    
    qDebug() << "Request URL:" << url.toString();
    
    QNetworkRequest request(url);
    m_networkManager->get(request);
}

void MusicAPIManager::handleNetworkResponse(QNetworkReply *reply)
{
    if (!reply) {
        qDebug() << "No reply object!";
        return;
    }
    
    qDebug() << "\n=== NETWORK RESPONSE ===";
    qDebug() << "URL:" << reply->url().toString();
    qDebug() << "Error:" << reply->error();
    qDebug() << "Error string:" << reply->errorString();
    
    // Сразу читаем все данные
    QByteArray responseData = reply->readAll();
    qDebug() << "Response size:" << responseData.size() << "bytes";
    
    if (responseData.isEmpty()) {
        qDebug() << "Empty response!";
        reply->deleteLater();
        return;
    }
    
    qDebug() << "First 200 chars:" << responseData.left(200);
    
    // Определяем тип запроса по URL
    QString urlString = reply->url().toString();
    
    if (reply->error() == QNetworkReply::NoError) {
        if (urlString.contains("track.search")) {
            processSearchResponse(responseData);
        } else if (urlString.contains("chart.gettoptracks")) {
            processTopTracksResponse(responseData);
        } else {
            qDebug() << "Unknown request type";
        }
    } else {
        qDebug() << "Network error:" << reply->errorString();
        reply->ignoreSslErrors();
    }
    
    reply->deleteLater();
    qDebug() << "=== RESPONSE COMPLETE ===\n";
}

void MusicAPIManager::processTopTracksResponse(const QByteArray& responseData)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON Parse error:" << parseError.errorString();
        qDebug() << "At position:" << parseError.offset;
        qDebug() << "Error context:" << responseData.mid(parseError.offset - 10, 20);
        return;
    }
    
    if (doc.isNull()) {
        qDebug() << "Failed to parse JSON for top tracks";
        return;
    }
    
    QJsonObject root = doc.object();
    
    if (root.contains("error")) {
        qDebug() << "API Error:" << root["error"].toInt() << "-" << root["message"].toString();
        return;
    }
    
    if (root.contains("tracks")) {
        QJsonObject tracksObj = root["tracks"].toObject();
        
        // Проверим структуру
        qDebug() << "Tracks object keys:" << tracksObj.keys();
        
        if (tracksObj.contains("track")) {
            QJsonArray tracks = tracksObj["track"].toArray();
            qDebug() << "✅ Top tracks results:" << tracks.size() << "tracks found";
            
            // Покажем первый трек для отладки
            if (!tracks.isEmpty()) {
                QJsonObject firstTrack = tracks.first().toObject();
                qDebug() << "First track name:" << firstTrack["name"].toString();
                qDebug() << "First track artist:" << firstTrack["artist"].toObject()["name"].toString();
            }
            
            processTracksData(tracks);
        } else {
            qDebug() << "No 'track' array in tracks object";
        }
    } else {
        qDebug() << "Unexpected top tracks response structure";
        qDebug() << "Root keys:" << root.keys();
    }
}

void MusicAPIManager::processSearchResponse(const QByteArray& responseData)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON Parse error:" << parseError.errorString();
        qDebug() << "At position:" << parseError.offset;
        return;
    }
    
    if (doc.isNull()) {
        qDebug() << "Failed to parse JSON for search";
        return;
    }
    
    QJsonObject root = doc.object();
    
    if (root.contains("error")) {
        qDebug() << "API Error:" << root["error"].toInt() << "-" << root["message"].toString();
        return;
    }
    
    if (root.contains("results")) {
        QJsonObject results = root["results"].toObject();
        QJsonObject trackMatches = results["trackmatches"].toObject();
        QJsonArray tracks = trackMatches["track"].toArray();
        
        qDebug() << "✅ Search results:" << tracks.size() << "tracks found";
        
        // Покажем первый трек для отладки
        if (!tracks.isEmpty()) {
            QJsonObject firstTrack = tracks.first().toObject();
            qDebug() << "First search track:" << firstTrack["name"].toString() << "-" << firstTrack["artist"].toString();
        }
        
        processTracksData(tracks);
    } else {
        qDebug() << "Unexpected search response structure";
        qDebug() << "Root keys:" << root.keys();
    }
}

void MusicAPIManager::processTracksData(const QJsonArray& tracksArray)
{
    QVariantList processedTracks;
    
    for (const QJsonValue &trackValue : tracksArray) {
        QJsonObject trackObj = trackValue.toObject();
        
        QVariantMap track;
        
        // Для популярных треков структура отличается от поисковых
        if (trackObj.contains("artist") && trackObj["artist"].isObject()) {
            // Это популярный трек - artist это объект
            QJsonObject artistObj = trackObj["artist"].toObject();
            track["artist"] = artistObj["name"].toString();
        } else {
            // Это поисковый трек - artist это строка
            track["artist"] = trackObj.contains("artist") ? 
                             (trackObj["artist"].isString() ? trackObj["artist"].toString() : 
                              "Unknown Artist") : "Unknown Artist";
        }
        
        track["title"] = trackObj["name"].toString();
        
        // Обработка изображений
        QString coverUrl;
        if (trackObj.contains("image")) {
            QJsonArray images = trackObj["image"].toArray();
            if (!images.isEmpty()) {
                // Берем самое большое изображение (последнее в массиве)
                for (int i = images.size() - 1; i >= 0; --i) {
                    QString imageUrl = images[i].toObject()["#text"].toString();
                    if (!imageUrl.isEmpty()) {
                        coverUrl = imageUrl;
                        break;
                    }
                }
            }
        }
        
        track["coverUrl"] = coverUrl;

        // Обработка статистики
        if (trackObj.contains("listeners")) {
            track["listeners"] = trackObj["listeners"].toString();
        }
        if (trackObj.contains("playcount")) {
            track["playcount"] = trackObj["playcount"].toString();
        }
        
        processedTracks.append(track);
        
        qDebug() << "Processing track:" << track["title"].toString() << "-" << track["artist"].toString();
        
        // Сохраняем в БД - используем новый метод с 4 параметрами
        DatabaseManager::instance().addTrackFromAPI(
            m_currentUserId,
            track["title"].toString(),
            track["artist"].toString(),
            coverUrl
        );
    }
    
    qDebug() << "Emitting tracksFound signal with" << processedTracks.size() << "tracks";
    emit tracksFound(processedTracks);
}