#include "WebSocketClient.h"
#include <QJsonDocument>
#include <QDebug>

WebSocketClient::WebSocketClient(int userId, QObject *parent)
    : QObject(parent)
    , m_webSocket(new QWebSocket())
    , m_userId(userId)
    , m_isConnected(false)
{
    connect(m_webSocket, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessageReceived);
    connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), 
            this, &WebSocketClient::onError);
}

WebSocketClient::~WebSocketClient()
{
    if (m_isConnected) {
        m_webSocket->close();
    }
    delete m_webSocket;
}

void WebSocketClient::connectToServer(const QString& url)
{
    if (m_isConnected) {
        return;
    }
    
    m_webSocket->open(QUrl(url));
}

void WebSocketClient::disconnect()
{
    if (m_isConnected) {
        m_webSocket->close();
    }
}

void WebSocketClient::joinRoom(int roomId)
{
    QJsonObject json;
    json["type"] = "join_room";
    json["userId"] = m_userId;
    json["roomId"] = roomId;
    
    sendJson(json);
}

void WebSocketClient::leaveRoom(int roomId)
{
    QJsonObject json;
    json["type"] = "leave_room";
    json["userId"] = m_userId;
    json["roomId"] = roomId;
    
    sendJson(json);
}

void WebSocketClient::sendMessage(int roomId, const QString& message)
{
    QJsonObject json;
    json["type"] = "chat_message";
    json["userId"] = m_userId;
    json["roomId"] = roomId;
    json["message"] = message;
    
    sendJson(json);
}

void WebSocketClient::updatePlaybackState(int roomId, bool isPlaying, qint64 position)
{
    QJsonObject json;
    json["type"] = "playback_state";
    json["userId"] = m_userId;
    json["roomId"] = roomId;
    json["isPlaying"] = isPlaying;
    json["position"] = position;
    
    sendJson(json);
}

void WebSocketClient::onConnected()
{
    m_isConnected = true;
    
    QJsonObject authJson;
    authJson["type"] = "authenticate";
    authJson["userId"] = m_userId;
    
    sendJson(authJson);
    
    emit connected();
}

void WebSocketClient::onDisconnected()
{
    m_isConnected = false;
    emit disconnected();
}

void WebSocketClient::onTextMessageReceived(const QString& message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    
    if (!doc.isObject()) {
        return;
    }
    
    QJsonObject json = doc.object();
    QString type = json["type"].toString();
    
    if (type == "playback_state") {
        int roomId = json["roomId"].toInt();
        bool isPlaying = json["isPlaying"].toBool();
        qint64 position = json["position"].toVariant().toLongLong();
        
        emit roomStateUpdated(roomId, isPlaying, position);
    }
    else if (type == "user_joined") {
        int roomId = json["roomId"].toInt();
        int userId = json["userId"].toInt();
        QString username = json["username"].toString();
        
        emit userJoined(roomId, userId, username);
    }
    else if (type == "user_left") {
        int roomId = json["roomId"].toInt();
        int userId = json["userId"].toInt();
        
        emit userLeft(roomId, userId);
    }
    
    emit messageReceived(json);
}

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "WebSocket error:" << error << m_webSocket->errorString();
}

void WebSocketClient::sendJson(const QJsonObject& json)
{
    if (!m_isConnected) {
        return;
    }
    
    QJsonDocument doc(json);
    m_webSocket->sendTextMessage(doc.toJson(QJsonDocument::Compact));
}
