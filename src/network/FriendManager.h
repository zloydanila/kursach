#ifndef FRIENDMANAGER_H
#define FRIENDMANAGER_H

#include <QObject>
#include <QVector>
#include "core/models/User.h"

class FriendManager : public QObject
{
    Q_OBJECT

public:
    explicit FriendManager(int userId, QObject *parent = nullptr);
    
    bool sendFriendRequest(int targetUserId);
    bool acceptFriendRequest(int requesterId);
    bool rejectFriendRequest(int requesterId);
    bool removeFriend(int friendId);
    
    bool isFriend(int otherUserId);
QVector<User> getFriends();
    QVector<User> getPendingRequests();
    QVector<User> searchUsers(const QString& query);

signals:
    void friendRequestReceived(const User& user);
    void friendAdded(const User& user);
    void friendRemoved(int userId);
    void friendStatusChanged(int userId, UserStatus status);
    void friendRequestSent(int toUserId);
    void friendRequestChanged();

private:
    int m_currentUserId;
};

#endif
