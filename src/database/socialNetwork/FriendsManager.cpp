#include "FriendsManager.h"
#include "../DatabaseManager.h"
#include <QDebug>
#include <QDateTime>

FriendsManager& FriendsManager::instance()
{
    static FriendsManager instance;
    return instance;
}

bool FriendsManager::sendFriendRequest(int fromUserId, int toUserId)
{
    if (fromUserId == toUserId) {
        return false;
    }
    
    QSqlQuery query;
    query.prepare(
        "INSERT INTO friends (user_id, friend_id, status) "
        "VALUES (?, ?, 'pending') "
        "ON CONFLICT (user_id, friend_id) DO UPDATE SET status = 'pending'"
    );
    query.addBindValue(fromUserId);
    query.addBindValue(toUserId);
    
    if (query.exec()) {

        emit friendRequestReceived(toUserId, getUsername(fromUserId));
        return true;
    } else {
        return false;
    }
}

bool FriendsManager::acceptFriendRequest(int fromUserId, int toUserId)
{
    QSqlQuery query;
    query.prepare(
        "UPDATE friends SET status = 'accepted' "
        "WHERE user_id = ? AND friend_id = ? AND status = 'pending'"
    );
    query.addBindValue(fromUserId);
    query.addBindValue(toUserId);

    if (query.exec() && query.numRowsAffected() > 0) {

        emit friendRequestAccepted(fromUserId, toUserId);
        return true;
    }

    return false;
}


bool FriendsManager::rejectFriendRequest(int fromUserId, int toUserId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM friends WHERE user_id = ? AND friend_id = ? AND status = 'pending'");
    query.addBindValue(fromUserId);
    query.addBindValue(toUserId);
    
    if (query.exec() && query.numRowsAffected() > 0) {
        return true;
    }
    
    return false;
}

bool FriendsManager::removeFriend(int userId, int friendId)
{
    QSqlQuery query;
    query.prepare(
        "DELETE FROM friends WHERE "
        "(user_id = ? AND friend_id = ?) OR (user_id = ? AND friend_id = ?)"
    );
    query.addBindValue(userId);
    query.addBindValue(friendId);
    query.addBindValue(friendId);
    query.addBindValue(userId);
    
    if (query.exec()) {
        emit friendRemoved(userId, friendId);
        return true;
    }
    
    return false;
}

QList<QVariantMap> FriendsManager::getPendingRequests(int userId)
{
    QList<QVariantMap> requests;
    QSqlQuery query;
    query.prepare(
        "SELECT u.id, u.username, u.avatar_path, f.created_at "
        "FROM friends f "
        "JOIN users u ON f.user_id = u.id "
        "WHERE f.friend_id = ? AND f.status = 'pending' "
        "ORDER BY f.created_at DESC"
    );
    query.addBindValue(userId);
    
    if (query.exec()) {
        while (query.next()) {
            QVariantMap request;
            request["id"] = query.value("id").toInt();
            request["username"] = query.value("username").toString();
            request["avatar_path"] = query.value("avatar_path").toString();
            request["created_at"] = query.value("created_at").toDateTime();
            requests.append(request);
        }
    }
    
    return requests;
}

QList<QVariantMap> FriendsManager::getFriends(int userId)
{
    QList<QVariantMap> friends;
    QSqlQuery query;
    query.prepare(
        "SELECT DISTINCT u.id, u.username, u.avatar_path, f.created_at "
        "FROM friends f "
        "JOIN users u ON ("
        "    (f.user_id = ? AND f.friend_id = u.id) OR "
        "    (f.friend_id = ? AND f.user_id = u.id)"
        ") "
        "WHERE f.status = 'accepted' AND u.id != ? "  
        "ORDER BY u.username"
    );
    query.addBindValue(userId);
    query.addBindValue(userId);
    query.addBindValue(userId);  

    if (query.exec()) {
        while (query.next()) {
            QVariantMap friendData;
            friendData["id"] = query.value("id").toInt();
            friendData["username"] = query.value("username").toString();
            friendData["avatar_path"] = query.value("avatar_path").toString();
            friendData["created_at"] = query.value("created_at").toDateTime();
            friends.append(friendData);
        }
    }

    return friends;
}


QList<QVariantMap> FriendsManager::searchUsers(const QString& queryStr, int currentUserId)
{
    QList<QVariantMap> users;
    QSqlQuery query;
    query.prepare(
        "SELECT id, username, avatar_path "
        "FROM users "
        "WHERE username ILIKE ? AND id != ? "
        "ORDER BY username "
        "LIMIT 20"
    );
    query.addBindValue("%" + queryStr + "%");
    query.addBindValue(currentUserId);
    
    if (query.exec()) {
        while (query.next()) {
            QVariantMap user;
            user["id"] = query.value("id").toInt();
            user["username"] = query.value("username").toString();
            user["avatar_path"] = query.value("avatar_path").toString();
            user["friend_status"] = getFriendStatus(currentUserId, user["id"].toInt());
            
            users.append(user);
        }
    }
    
    return users;
}

bool FriendsManager::isFriend(int user1Id, int user2Id)
{
    QSqlQuery query;
    query.prepare(
        "SELECT 1 FROM friends WHERE "
        "((user_id = ? AND friend_id = ?) OR (user_id = ? AND friend_id = ?)) "
        "AND status = 'accepted'"
    );
    query.addBindValue(user1Id);
    query.addBindValue(user2Id);
    query.addBindValue(user2Id);
    query.addBindValue(user1Id);
    
    return query.exec() && query.next();
}

QString FriendsManager::getFriendStatus(int user1Id, int user2Id)
{
    QSqlQuery query;
    query.prepare(
        "SELECT status FROM friends WHERE user_id = ? AND friend_id = ?"
    );
    query.addBindValue(user1Id);
    query.addBindValue(user2Id);
    
    if (query.exec() && query.next()) {
        return query.value("status").toString();
    }
    
    return "none";
}

QString FriendsManager::getUsername(int userId)
{
    QSqlQuery query;
    query.prepare("SELECT username FROM users WHERE id = ?");
    query.addBindValue(userId);
    
    if (query.exec() && query.next()) {
        return query.value("username").toString();
    }
    
    return "Unknown User";
}