#include "FriendManager.h"
#include "database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

FriendManager::FriendManager(int userId, QObject *parent)
    : QObject(parent), m_currentUserId(userId)
{
}

static QString esc(QString s) { return s.replace("'", "''"); }

bool FriendManager::sendFriendRequest(int targetUserId)
{
    QSqlQuery q(DatabaseManager::instance().database());
    QString sql = QString(
        "INSERT INTO friend_requests (sender_id, receiver_id, status, created_at) "
        "VALUES (%1, %2, 'pending', NOW()) "
        "ON CONFLICT (sender_id, receiver_id) DO NOTHING"
    ).arg(m_currentUserId).arg(targetUserId);

    if (!q.exec(sql)) { qDebug() << q.lastError().text() << sql; return false; }
    return true;
}

bool FriendManager::acceptFriendRequest(int requesterId)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    if (!db.transaction()) return false;

    QSqlQuery q(db);
    QString sql1 = QString(
        "UPDATE friend_requests SET status='accepted' "
        "WHERE sender_id=%1 AND receiver_id=%2 AND status='pending'"
    ).arg(requesterId).arg(m_currentUserId);

    if (!q.exec(sql1)) { qDebug() << q.lastError().text() << sql1; db.rollback(); return false; }

    QString sql2 = QString(
        "INSERT INTO friendships (user_id, friend_id, created_at) "
        "VALUES (%1,%2,NOW()),(%2,%1,NOW())"
    ).arg(m_currentUserId).arg(requesterId);

    if (!q.exec(sql2)) { qDebug() << q.lastError().text() << sql2; db.rollback(); return false; }

    return db.commit();
}

bool FriendManager::rejectFriendRequest(int requesterId)
{
    QSqlQuery q(DatabaseManager::instance().database());
    QString sql = QString(
        "UPDATE friend_requests SET status='rejected' "
        "WHERE sender_id=%1 AND receiver_id=%2"
    ).arg(requesterId).arg(m_currentUserId);

    if (!q.exec(sql)) { qDebug() << q.lastError().text() << sql; return false; }
    return true;
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

    if (!q.exec(sql)) { qDebug() << q.lastError().text() << sql; db.rollback(); return false; }
    if (!db.commit()) return false;

    emit friendRemoved(friendId);
    return true;
}

QVector<User> FriendManager::getFriends()
{
    QVector<User> friends;
    QSqlQuery q(DatabaseManager::instance().database());

    QString sql = QString(
        "SELECT u.id, u.username, u.avatar_path "
        "FROM users u "
        "INNER JOIN friendships f ON f.friend_id=u.id "
        "WHERE f.user_id=%1 "
        "ORDER BY u.username"
    ).arg(m_currentUserId);

    if (!q.exec(sql)) { qDebug() << "getFriends:" << q.lastError().text() << sql; return friends; }

    while (q.next()) {
        User u;
        u.id = q.value(0).toInt();
        u.username = q.value(1).toString();
        u.avatarPath = q.value(2).toString();
        u.isFriend = true;
        friends.push_back(u);
    }
    return friends;
}

QVector<User> FriendManager::getPendingRequests()
{
    QVector<User> req;
    QSqlQuery q(DatabaseManager::instance().database());

    QString sql = QString(
        "SELECT u.id, u.username, u.avatar_path "
        "FROM users u "
        "INNER JOIN friend_requests fr ON fr.sender_id=u.id "
        "WHERE fr.receiver_id=%1 AND fr.status='pending' "
        "ORDER BY fr.created_at DESC"
    ).arg(m_currentUserId);

    if (!q.exec(sql)) { qDebug() << "getPendingRequests:" << q.lastError().text() << sql; return req; }

    while (q.next()) {
        User u;
        u.id = q.value(0).toInt();
        u.username = q.value(1).toString();
        u.avatarPath = q.value(2).toString();
        u.friendRequestReceived = true;
        req.push_back(u);
    }
    return req;
}

QVector<User> FriendManager::searchUsers(const QString& queryText)
{
    QVector<User> users;
    QSqlQuery q(DatabaseManager::instance().database());

    QString pat = "%" + esc(queryText.trimmed().toLower()) + "%";
    QString sql = QString(
        "SELECT id, username, avatar_path "
        "FROM users "
        "WHERE LOWER(username) LIKE '%1' AND id<>%2 "
        "ORDER BY username "
        "LIMIT 20"
    ).arg(pat).arg(m_currentUserId);

    qDebug() << "SQL(search):" << sql;

    if (!q.exec(sql)) { qDebug() << "searchUsers:" << q.lastError().text(); return users; }

    while (q.next()) {
        User u;
        u.id = q.value(0).toInt();
        u.username = q.value(1).toString();
        u.avatarPath = q.value(2).toString();
        users.push_back(u);
    }
    return users;
}
