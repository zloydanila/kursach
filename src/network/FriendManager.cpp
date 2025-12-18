#include "FriendManager.h"
#include "database/DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

FriendManager::FriendManager(int userId, QObject *parent)
    : QObject(parent)
    , m_currentUserId(userId)
{
}

bool FriendManager::sendFriendRequest(int targetUserId)
{
    if (m_currentUserId == targetUserId) return false;

    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare(
        "INSERT INTO friend_requests(sender_id, receiver_id, status, created_at) "
        "VALUES (?, ?, 'pending', NOW()) "
        "ON CONFLICT (sender_id, receiver_id) "
        "DO UPDATE SET status='pending', created_at=EXCLUDED.created_at"
    );
    q.addBindValue(m_currentUserId);
    q.addBindValue(targetUserId);

    if (!q.exec()) return false;
    emit friendRequestSent(targetUserId);
    emit friendRequestChanged();
    return true;
}

bool FriendManager::acceptFriendRequest(int requesterId)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    if (!db.transaction()) return false;

    QSqlQuery q(db);

    q.prepare(
        "UPDATE friend_requests SET status='accepted' "
        "WHERE sender_id=? AND receiver_id=? AND status='pending'"
    );
    q.addBindValue(requesterId);
    q.addBindValue(m_currentUserId);

    if (!q.exec()) { db.rollback(); return false; }
    if (q.numRowsAffected() == 0) { db.rollback(); return false; }

    q.prepare(
        "INSERT INTO friendships(user_id, friend_id, created_at) "
        "VALUES (?, ?, NOW()) ON CONFLICT (user_id, friend_id) DO NOTHING"
    );
    q.addBindValue(m_currentUserId);
    q.addBindValue(requesterId);
    if (!q.exec()) { db.rollback(); return false; }

    q.prepare(
        "INSERT INTO friendships(user_id, friend_id, created_at) "
        "VALUES (?, ?, NOW()) ON CONFLICT (user_id, friend_id) DO NOTHING"
    );
    q.addBindValue(requesterId);
    q.addBindValue(m_currentUserId);
    if (!q.exec()) { db.rollback(); return false; }

    if (!db.commit()) return false;
    emit friendRequestChanged();
    return true;
}

bool FriendManager::rejectFriendRequest(int requesterId)
{
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare(
        "UPDATE friend_requests SET status='rejected' "
        "WHERE sender_id=? AND receiver_id=? AND status='pending'"
    );
    q.addBindValue(requesterId);
    q.addBindValue(m_currentUserId);

    if (!q.exec()) return false;
    if (q.numRowsAffected() > 0) { emit friendRequestChanged(); return true; }
    return false;
}

bool FriendManager::removeFriend(int friendId)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    if (!db.transaction()) return false;

    QSqlQuery q(db);
    q.prepare(
        "DELETE FROM friendships "
        "WHERE (user_id=? AND friend_id=?) OR (user_id=? AND friend_id=?)"
    );
    q.addBindValue(m_currentUserId);
    q.addBindValue(friendId);
    q.addBindValue(friendId);
    q.addBindValue(m_currentUserId);

    if (!q.exec()) { db.rollback(); return false; }
    return db.commit();
}

bool FriendManager::isFriend(int otherUserId)
{
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT 1 FROM friendships WHERE user_id=? AND friend_id=? LIMIT 1");
    q.addBindValue(m_currentUserId);
    q.addBindValue(otherUserId);

    if (!q.exec()) return false;
    return q.next();
}

QVector<User> FriendManager::getFriends()
{
    QVector<User> out;
    QSqlQuery q(DatabaseManager::instance().database());

    q.prepare(
        "SELECT u.id, u.username, u.email, u.avatar_path, CASE WHEN u.status = 3 THEN 3 WHEN NOW() - u.last_seen <= INTERVAL '30 seconds' THEN u.status ELSE 4 END AS status, u.bio, u.last_seen, u.created_at "
        "FROM friendships f "
        "JOIN users u ON u.id = f.friend_id "
        "WHERE f.user_id = ? "
        "ORDER BY u.username"
    );
    q.addBindValue(m_currentUserId);

    if (!q.exec()) return out;

    while (q.next()) {
        User u{};
        u.id = q.value(0).toInt();
        u.username = q.value(1).toString();
        u.email = q.value(2).toString();
        u.avatarPath = q.value(3).toString();
        u.status = static_cast<UserStatus>(q.value(4).toInt());
        u.bio = q.value(5).toString();
        u.lastSeen = q.value(6).toDateTime();
        u.createdAt = q.value(7).toDateTime();
        u.isFriend = true;
        out.push_back(u);
    }
    return out;
}

QVector<User> FriendManager::getPendingRequests()
{
    QVector<User> out;
    QSqlQuery q(DatabaseManager::instance().database());

    q.prepare(
        "SELECT u.id, u.username, u.email, u.avatar_path, CASE WHEN u.status = 3 THEN 3 WHEN NOW() - u.last_seen <= INTERVAL '30 seconds' THEN u.status ELSE 4 END AS status, u.bio, u.last_seen, u.created_at "
        "FROM friend_requests fr "
        "JOIN users u ON u.id = fr.sender_id "
        "WHERE fr.receiver_id = ? AND fr.status = 'pending' "
        "ORDER BY fr.created_at DESC"
    );
    q.addBindValue(m_currentUserId);

    if (!q.exec()) return out;

    while (q.next()) {
        User u{};
        u.id = q.value(0).toInt();
        u.username = q.value(1).toString();
        u.email = q.value(2).toString();
        u.avatarPath = q.value(3).toString();
        u.status = static_cast<UserStatus>(q.value(4).toInt());
        u.bio = q.value(5).toString();
        u.lastSeen = q.value(6).toDateTime();
        u.createdAt = q.value(7).toDateTime();
        u.friendRequestReceived = true;
        out.push_back(u);
    }
    return out;
}

QVector<User> FriendManager::searchUsers(const QString& query)
{
    QVector<User> out;

    const QString term = query.trimmed();
    if (term.isEmpty()) return out;

    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare(
        "SELECT u.id, u.username, u.email, u.avatar_path, CASE WHEN u.status = 3 THEN 3 WHEN NOW() - u.last_seen <= INTERVAL '30 seconds' THEN u.status ELSE 4 END AS status, u.bio, u.last_seen, u.created_at, "
        "       (f.friend_id IS NOT NULL) AS is_friend "
        "FROM users u "
        "LEFT JOIN friendships f ON f.user_id = ? AND f.friend_id = u.id "
        "WHERE u.id <> ? AND (u.username ILIKE ? OR u.email ILIKE ?) "
        "ORDER BY u.username "
        "LIMIT 50"
    );

    const QString pat = "%" + term + "%";
    q.addBindValue(m_currentUserId);
    q.addBindValue(m_currentUserId);
    q.addBindValue(pat);
    q.addBindValue(pat);

    if (!q.exec()) return out;

    while (q.next()) {
        User u{};
        u.id = q.value(0).toInt();
        u.username = q.value(1).toString();
        u.email = q.value(2).toString();
        u.avatarPath = q.value(3).toString();
        u.status = static_cast<UserStatus>(q.value(4).toInt());
        u.bio = q.value(5).toString();
        u.lastSeen = q.value(6).toDateTime();
        u.createdAt = q.value(7).toDateTime();
        u.isFriend = q.value(8).toBool();
        out.push_back(u);
    }
    return out;
}
