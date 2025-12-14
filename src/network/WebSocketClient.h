#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>

class WebSocketClient : public QObject
{
    Q_OBJECT

public:
    explicit WebSocketClient(int userId, QObject *parent = nullptr);
    ~WebSocketClient();
    
    void connectToServer(const QString& url);
    void disconnect();
    
    void joinRoom(int roomId);
    void leaveRoom(int roomId);
    void sendMessage(int roomId, const QString& message);
    void updatePlaybackState(int roomId, bool isPlaying, qint64 position);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QJsonObject& message);
    void roomStateUpdated(int roomId, bool isPlaying, qint64 position);
    void userJoined(int roomId, int userId, const QString& username);
    void userLeft(int roomId, int userId);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onError(QAbstractSocket::SocketError error);

private:
    void sendJson(const QJsonObject& json);
    
    QWebSocket* m_webSocket;
    int m_userId;
    bool m_isConnected;
};

#endif
