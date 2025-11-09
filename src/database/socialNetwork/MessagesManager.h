#ifndef MESSAGESMANAGER_H
#define MESSAGESMANAGER_H

#include <QObject>
#include <QSqlQuery>
#include <QVariantMap>
#include <QList>
#include <QDateTime>

class MessagesManager : public QObject
{
    Q_OBJECT

public:
    static MessagesManager& instance();
    
    bool sendMessage(int fromUserId, int toUserId, const QString& text);
    QList<QVariantMap> getDialogs(int userId);
    QList<QVariantMap> getConversation(int user1Id, int user2Id, int limit = 50);
    int getUnreadCount(int userId);
    bool markMessagesAsRead(int user1Id, int user2Id);
    bool deleteMessage(int messageId);

signals:
    void newMessageReceived(int fromUserId, const QString& username, 
                           const QString& message, const QDateTime& timestamp);

private:
    MessagesManager() = default;
    QString getUsername(int userId);
};

#endif // MESSAGESMANAGER_H