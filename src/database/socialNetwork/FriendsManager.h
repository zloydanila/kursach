#ifndef FRIENDSMANAGER_H
#define FRIENDSMANAGER_H

#include <QObject>
#include <QSqlQuery>
#include <QVariantMap>
#include <QList>
#include "MessagesManager.h"

class FriendsManager : public QObject
{
    Q_OBJECT

public:
    static FriendsManager& instance();
    
    bool sendFriendRequest(int fromUserId, int toUserId);
    bool acceptFriendRequest(int fromUserId, int toUserId);
    bool rejectFriendRequest(int fromUserId, int toUserId);
    bool removeFriend(int userId, int friendId);
    
    QList<QVariantMap> getPendingRequests(int userId);
    QList<QVariantMap> getFriends(int userId);
    QList<QVariantMap> searchUsers(const QString& query, int currentUserId);
    
    bool isFriend(int user1Id, int user2Id);
    QString getFriendStatus(int user1Id, int user2Id);

signals:
    void friendRequestReceived(int fromUserId, const QString& username);
    void friendRequestAccepted(int fromUserId, int toUserId);
    void friendRemoved(int user1Id, int user2Id);

private:
    FriendsManager() = default;
    QString getUsername(int userId);
};

#endif 