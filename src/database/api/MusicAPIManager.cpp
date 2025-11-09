#include "MusicAPIManager.h"
#include "../database/DatabaseManager.h"
#include <QDebug>
#include <QSslError>

#include "MusicAPIManager.h"
#include "../database/DatabaseManager.h"
#include <QDebug>
#include <QSslError>

MusicAPIManager::MusicAPIManager(QObject *parent) 
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_apiKey("5d63cb6ba6780da3f1306e10539e4b01")  
    , m_baseUrl("https://ws.audioscrobbler.com/2.0/")
    , m_jamendoClientId("e31f93b6")
    , m_jamendoBaseUrl("https://api.jamendo.com/v3.0/")
    , m_currentUserId(-1)
{
    qDebug() << "🎵 MusicAPIManager initialized";
    
    m_networkManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    
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

void MusicAPIManager::searchTracksWithAudio(const QString& query, int userId)
{
    m_currentUserId = userId;
    
    qDebug() << "=== JAMENDO SEARCH START ===";
    qDebug() << "Query:" << query;
    
    // Если Jamendo Client ID не настроен, используем обычный поиск
    if (m_jamendoClientId.isEmpty() || m_jamendoClientId == "YOUR_JAMENDO_CLIENT_ID") {
        qDebug() << "⚠️ Jamendo Client ID not configured, using Last.fm search";
        searchTracks(query, userId);
        return;
    }
    
    QUrl url(m_jamendoBaseUrl + "tracks/");
    QUrlQuery params;
    params.addQueryItem("client_id", m_jamendoClientId);
    params.addQueryItem("format", "json");
    params.addQueryItem("search", query);
    params.addQueryItem("limit", "10");
    params.addQueryItem("audioformat", "mp32");  // MP3 формат
    params.addQueryItem("imagesize", "300");     // Размер обложки
    
    url.setQuery(params);
    
    qDebug() << "Jamendo Request URL:" << url.toString();
    
    QNetworkRequest request(url);
    QNetworkReply *reply = m_networkManager->get(request);
    
    // Связываем конкретный reply с обработчиком Jamendo
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            this->processJamendoSearchResponse(reply->readAll());
        } else {
            qDebug() << "❌ Jamendo network error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

void MusicAPIManager::playTrackFromAPI(const QString& trackId, const QString& service)
{
    if (service == "jamendo") {
        // Для Jamendo audio URL уже есть в данных трека
        emit trackAudioUrlReady(trackId, "");
    }
}

void MusicAPIManager::handleNetworkResponse(QNetworkReply *reply)
{
    if (!reply) {
        qDebug() << "❌ No reply object!";
        emit networkError("No reply object");
        return;
    }
    
    qDebug() << "\n=== NETWORK RESPONSE ===";
    qDebug() << "URL:" << reply->url().toString();
    qDebug() << "Error:" << reply->error();
    qDebug() << "Error string:" << reply->errorString();
    
    QByteArray responseData = reply->readAll();
    qDebug() << "Response size:" << responseData.size() << "bytes";
    
    if (responseData.isEmpty()) {
        qDebug() << "❌ Empty response!";
        emit networkError("Empty response");
        reply->deleteLater();
        return;
    }
    
    qDebug() << "First 200 chars:" << responseData.left(200);
    
    QString urlString = reply->url().toString();
    
    if (reply->error() == QNetworkReply::NoError) {
        if (urlString.contains("track.search")) {
            processSearchResponse(responseData);
        } else if (urlString.contains("chart.gettoptracks")) {
            processTopTracksResponse(responseData);
        } else {
            qDebug() << "❌ Unknown request type";
            emit networkError("Unknown request type");
        }
    } else {
        qDebug() << "❌ Network error:" << reply->errorString();
        emit networkError(reply->errorString());
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
        qDebug() << "❌ JSON Parse error:" << parseError.errorString();
        qDebug() << "At position:" << parseError.offset;
        qDebug() << "Error context:" << responseData.mid(parseError.offset - 10, 20);
        return;
    }
    
    if (doc.isNull()) {
        qDebug() << "❌ Failed to parse JSON for top tracks";
        return;
    }
    
    QJsonObject root = doc.object();
    
    if (root.contains("error")) {
        qDebug() << "❌ API Error:" << root["error"].toInt() << "-" << root["message"].toString();
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
            qDebug() << "❌ No 'track' array in tracks object";
        }
    } else {
        qDebug() << "❌ Unexpected top tracks response structure";
        qDebug() << "Root keys:" << root.keys();
    }
}

void MusicAPIManager::processSearchResponse(const QByteArray& responseData)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "❌ JSON Parse error:" << parseError.errorString();
        qDebug() << "At position:" << parseError.offset;
        return;
    }
    
    if (doc.isNull()) {
        qDebug() << "❌ Failed to parse JSON for search";
        return;
    }
    
    QJsonObject root = doc.object();
    
    if (root.contains("error")) {
        qDebug() << "❌ API Error:" << root["error"].toInt() << "-" << root["message"].toString();
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
        qDebug() << "❌ Unexpected search response structure";
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
        if (trackObj.contains("image")) {
            QJsonArray images = trackObj["image"].toArray();
            if (!images.isEmpty()) {
                // Берем самое большое изображение (последнее в массиве)
                for (int i = images.size() - 1; i >= 0; --i) {
                    QString imageUrl = images[i].toObject()["#text"].toString();
                    if (!imageUrl.isEmpty()) {
                        track["coverUrl"] = imageUrl;
                        break;
                    }
                }
            }
        }

        // Обработка статистики
        if (trackObj.contains("listeners")) {
            track["listeners"] = trackObj["listeners"].toString();
        }
        if (trackObj.contains("playcount")) {
            track["playcount"] = trackObj["playcount"].toString();
        }
        
        processedTracks.append(track);
        
        qDebug() << "🎵 Processing track:" << track["title"].toString() << "-" << track["artist"].toString();
        
        // Сохраняем в БД
        DatabaseManager::instance().addTrackFromAPI(
            m_currentUserId,
            track["title"].toString(),
            track["artist"].toString(),
            "", // album
            0,  // duration  
            "", // genre
            track["coverUrl"].toString()
        );
    }
    
    qDebug() << "🎵 Emitting tracksFound signal with" << processedTracks.size() << "tracks";
    emit tracksFound(processedTracks);
}

void MusicAPIManager::processJamendoSearchResponse(const QByteArray& responseData)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "❌ Jamendo JSON Parse error:" << parseError.errorString();
        return;
    }
    
    QJsonObject root = doc.object();
    QJsonArray results = root["results"].toArray();
    
    qDebug() << "✅ Jamendo results:" << results.size() << "tracks found";
    
    QVariantList processedTracks;
    
    for (const QJsonValue &trackValue : results) {
        QJsonObject trackObj = trackValue.toObject();
        
        QVariantMap track;
        track["id"] = trackObj["id"].toString();
        track["title"] = trackObj["name"].toString();
        track["artist"] = trackObj["artist_name"].toString();
        track["duration"] = trackObj["duration"].toInt();
        track["audioUrl"] = trackObj["audio"].toString();
        track["coverUrl"] = trackObj["image"].toString();
        track["album"] = trackObj["album_name"].toString();
        track["service"] = "jamendo";
        
        // Статистика
        track["listeners"] = QString::number(trackObj["listens"].toInt());
        
        processedTracks.append(track);
        
        qDebug() << "🎵 Jamendo track:" << track["title"].toString() 
                 << "-" << track["artist"].toString()
                 << "Audio:" << track["audioUrl"].toString();
        
        // Сохраняем в БД
        DatabaseManager::instance().addTrackFromAPI(
            m_currentUserId,
            track["title"].toString(),
            track["artist"].toString(),
            track["album"].toString(),
            track["duration"].toInt(),
            "", // genre
            track["coverUrl"].toString()
        );
    }
    
    emit jamendoTracksFound(processedTracks);
}
