#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include <QObject>
#include <QVector>
#include "core/models/Message.h"

class ChatManager : public QObject
{
    Q_OBJECT

public:
    explicit ChatManager(int userId, QObject *parent = nullptr);
    
    bool sendMessage(int receiverId, const QString& content, MessageType type = MessageType::Text);
    bool shareTrack(int receiverId, int trackId);
    QVector<Message> getMessages(int otherUserId, int limit = 50);
    QVector<Message> getUnreadMessages();
    bool markAsRead(int messageId);
    QVector<ChatPreview> getChatList();
    int getUnreadCount();

signals:
    void messageReceived(const Message& message);
    void messageSent(int messageId);

private:
    int m_currentUserId;
};

#endif
