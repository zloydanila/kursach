#include "FriendManager.h"
#include "database/DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

static QString escSql(const QString& s)
{
    QString t = s;
    t.replace("'", "''");
    return t;
}

FriendManager::FriendManager(int userId, QObject *parent)
    : QObject(parent)
    , m_currentUserId(userId)
{
}

bool FriendManager::sendFriendRequest(int targetUserId)
{
    if (m_currentUserId == targetUserId) return false;

    QSqlQuery q(DatabaseManager::instance().database());
    const QString sql = QString(
        "INSERT INTO friend_requests(sender_id, receiver_id, status, created_at) "
        "VALUES (%1, %2, 'pending', NOW()) "
        "ON CONFLICT (sender_id, receiver_id) "
        "DO UPDATE SET status='pending', created_at=EXCLUDED.created_at"
    ).arg(m_currentUserId).arg(targetUserId);

    if (!q.exec(sql)) {
        qWarning() << "sendFriendRequest error:" << q.lastError();
        return false;
    }
    emit friendRequestSent(targetUserId);
    emit friendRequestChanged();
    return true;
}

bool FriendManager::acceptFriendRequest(int requesterId)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    if (!db.transaction()) return false;

    QSqlQuery q(db);

    QString upd = QString(
        "UPDATE friend_requests SET status='accepted' "
        "WHERE sender_id=%1 AND receiver_id=%2 AND status='pending'"
    ).arg(requesterId).arg(m_currentUserId);

    if (!q.exec(upd)) {
        qWarning() << "acceptFriendRequest update error:" << q.lastError();
        db.rollback();
        return false;
    }
    if (q.numRowsAffected() == 0) { db.rollback(); return false; }

    QString ins1 = QString(
        "INSERT INTO friendships(user_id, friend_id, created_at) "
        "VALUES (%1, %2, NOW()) ON CONFLICT (user_id, friend_id) DO NOTHING"
    ).arg(m_currentUserId).arg(requesterId);
    if (!q.exec(ins1)) { qWarning() << "insert1 error:" << q.lastError(); db.rollback(); return false; }

    QString ins2 = QString(
        "INSERT INTO friendships(user_id, friend_id, created_at) "
        "VALUES (%1, %2, NOW()) ON CONFLICT (user_id, friend_id) DO NOTHING"
    ).arg(requesterId).arg(m_currentUserId);
    if (!q.exec(ins2)) { qWarning() << "insert2 error:" << q.lastError(); db.rollback(); return false; }

    if (!db.commit()) return false;
    emit friendRequestChanged();
    return true;
}

bool FriendManager::rejectFriendRequest(int requesterId)
{
    QSqlQuery q(DatabaseManager::instance().database());
    QString sql = QString(
        "UPDATE friend_requests SET status='rejected' "
        "WHERE sender_id=%1 AND receiver_id=%2 AND status='pending'"
    ).arg(requesterId).arg(m_currentUserId);

    if (!q.exec(sql)) {
        qWarning() << "rejectFriendRequest error:" << q.lastError();
        return false;
    }
    if (q.numRowsAffected() > 0) { emit friendRequestChanged(); return true; }
    return false;
}

bool FriendManager::removeFriend(int friendId)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    if (!db.transaction()) return false;

    QSqlQuery q(db);
    QString sql = QString(
        "DELETE FROM friendships "
        "WHERE (user_id=%1 AND friend_id=%2) OR (user_id=%2 AND friend_id=%1)"
    ).arg(m_currentUserId).arg(friendId);

    if (!q.exec(sql)) {
        qWarning() << "removeFriend error:" << q.lastError();
        db.rollback();
        return false;
    }
    return db.commit();
}

bool FriendManager::isFriend(int otherUserId)
{
    QSqlQuery q(DatabaseManager::instance().database());
    QString sql = QString(
        "SELECT 1 FROM friendships WHERE user_id=%1 AND friend_id=%2 LIMIT 1"
    ).arg(m_currentUserId).arg(otherUserId);

    if (!q.exec(sql)) {
        qWarning() << "isFriend error:" << q.lastError();
        return false;
    }
    return q.next();
}

QVector<User> FriendManager::getFriends()
{
    QVector<User> out;
    QSqlQuery q(DatabaseManager::instance().database());

    QString sql = QString(
        "SELECT u.id, u.username, u.email, u.avatar_path, u.status, u.bio, u.last_seen, u.created_at "
        "FROM friendships f "
        "JOIN users u ON u.id = f.friend_id "
        "WHERE f.user_id = %1 "
        "ORDER BY u.username"
    ).arg(m_currentUserId);

    if (!q.exec(sql)) {
        qWarning() << "getFriends error:" << q.lastError();
        return out;
    }

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

    QString sql = QString(
        "SELECT u.id, u.username, u.email, u.avatar_path, u.status, u.bio, u.last_seen, u.created_at "
        "FROM friend_requests fr "
        "JOIN users u ON u.id = fr.sender_id "
        "WHERE fr.receiver_id = %1 AND fr.status = 'pending' "
        "ORDER BY fr.created_at DESC"
    ).arg(m_currentUserId);

    if (!q.exec(sql)) {
        qWarning() << "getPendingRequests error:" << q.lastError();
        return out;
    }

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

    QString term = query.trimmed();
    if (term.isEmpty()) return out;

    QString pat = "%" + escSql(term) + "%";

    QSqlQuery q(DatabaseManager::instance().database());
    QString sql = QString(
        "SELECT id, username, email, avatar_path, status, bio, last_seen, created_at "
        "FROM users "
        "WHERE id <> %1 AND (username ILIKE '%2' OR email ILIKE '%2') "
        "ORDER BY username LIMIT 50"
    ).arg(m_currentUserId).arg(pat);

    if (!q.exec(sql)) {
        qWarning() << "searchUsers error:" << q.lastError();
        return out;
    }

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
        u.isFriend = isFriend(u.id);
        out.push_back(u);
    }
    return out;
}
