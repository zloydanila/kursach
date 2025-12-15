#include "ChatManager.h"
#include "database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ChatManager::ChatManager(int userId, QObject *parent)
    : QObject(parent), m_currentUserId(userId)
{
}

bool ChatManager::sendMessage(int receiverId, const QString& content, MessageType type)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "INSERT INTO messages (sender_id, receiver_id, content, type, timestamp, is_read) "
        "VALUES (?, ?, ?, ?, NOW(), false) RETURNING id"
    );
    query.addBindValue(m_currentUserId);
    query.addBindValue(receiverId);
    query.addBindValue(content);
    query.addBindValue(static_cast<int>(type));
    
    if (query.exec() && query.next()) {
        int messageId = query.value(0).toInt();
        emit messageSent(messageId);
        return true;
    }
    
    return false;
}

bool ChatManager::shareTrack(int receiverId, int trackId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "INSERT INTO messages (sender_id, receiver_id, type, shared_track_id, timestamp, is_read) "
        "VALUES (?, ?, ?, ?, NOW(), false)"
    );
    query.addBindValue(m_currentUserId);
    query.addBindValue(receiverId);
    query.addBindValue(static_cast<int>(MessageType::TrackShare));
    query.addBindValue(trackId);
    
    return query.exec();
}

QVector<Message> ChatManager::getMessages(int otherUserId, int limit)
{
    QVector<Message> messages;
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT m.id, m.sender_id, m.receiver_id, u.username, u.avatar_path, "
        "m.content, m.type, m.timestamp, m.is_read, m.shared_track_id, "
        "t.title, t.artist "
        "FROM messages m "
        "INNER JOIN users u ON u.id = m.sender_id "
        "LEFT JOIN tracks t ON t.id = m.shared_track_id "
        "WHERE (m.sender_id = ? AND m.receiver_id = ?) "
        "OR (m.sender_id = ? AND m.receiver_id = ?) "
        "ORDER BY m.timestamp DESC LIMIT ?"
    );
    query.addBindValue(m_currentUserId);
    query.addBindValue(otherUserId);
    query.addBindValue(otherUserId);
    query.addBindValue(m_currentUserId);
    query.addBindValue(limit);
    
    if (query.exec()) {
        while (query.next()) {
            Message msg;
            msg.id = query.value(0).toInt();
            msg.senderId = query.value(1).toInt();
            msg.receiverId = query.value(2).toInt();
            msg.senderUsername = query.value(3).toString();
            msg.senderAvatarPath = query.value(4).toString();
            msg.content = query.value(5).toString();
            msg.type = static_cast<MessageType>(query.value(6).toInt());
            msg.timestamp = query.value(7).toDateTime();
            msg.isRead = query.value(8).toBool();
            msg.sharedTrackId = query.value(9).toInt();
            msg.trackTitle = query.value(10).toString();
            msg.trackArtist = query.value(11).toString();
            
            messages.append(msg);
        }
    }
    
    return messages;
}

QVector<Message> ChatManager::getUnreadMessages()
{
    QVector<Message> messages;
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT m.id, m.sender_id, m.receiver_id, u.username, u.avatar_path, "
        "m.content, m.type, m.timestamp, m.shared_track_id, t.title, t.artist "
        "FROM messages m "
        "INNER JOIN users u ON u.id = m.sender_id "
        "LEFT JOIN tracks t ON t.id = m.shared_track_id "
        "WHERE m.receiver_id = ? AND m.is_read = false "
        "ORDER BY m.timestamp DESC"
    );
    query.addBindValue(m_currentUserId);
    
    if (query.exec()) {
        while (query.next()) {
            Message msg;
            msg.id = query.value(0).toInt();
            msg.senderId = query.value(1).toInt();
            msg.receiverId = query.value(2).toInt();
            msg.senderUsername = query.value(3).toString();
            msg.senderAvatarPath = query.value(4).toString();
            msg.content = query.value(5).toString();
            msg.type = static_cast<MessageType>(query.value(6).toInt());
            msg.timestamp = query.value(7).toDateTime();
            msg.sharedTrackId = query.value(8).toInt();
            msg.trackTitle = query.value(9).toString();
            msg.trackArtist = query.value(10).toString();
            msg.isRead = false;
            
            messages.append(msg);
        }
    }
    
    return messages;
}

bool ChatManager::markAsRead(int messageId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE messages SET is_read = true WHERE id = ? AND receiver_id = ?");
    query.addBindValue(messageId);
    query.addBindValue(m_currentUserId);
    
    return query.exec();
}

QVector<ChatPreview> ChatManager::getChatList()
{
    QVector<ChatPreview> out;

    QSqlQuery q(DatabaseManager::instance().database());
    const QString uid = QString::number(m_currentUserId);

    const QString sql =
        "SELECT peer_id, username, last_message, unread_count "
        "FROM ("
        "  SELECT DISTINCT ON (peer_id) "
        "    peer_id, username, last_message, unread_count, ts "
        "  FROM ("
        "    SELECT "
        "      CASE WHEN m.sender_id=" + uid + " THEN m.receiver_id ELSE m.sender_id END AS peer_id, "
        "      u.username AS username, "
        "      COALESCE(m.content, '') AS last_message, "
        "      (SELECT COUNT(*) FROM messages x "
        "         WHERE x.receiver_id=" + uid + " AND x.sender_id=u.id AND x.is_read=false) AS unread_count, "
        "      m.timestamp AS ts "
        "    FROM messages m "
        "    JOIN users u ON u.id = CASE WHEN m.sender_id=" + uid + " THEN m.receiver_id ELSE m.sender_id END "
        "    WHERE m.sender_id=" + uid + " OR m.receiver_id=" + uid + " "
        "  ) t "
        "  ORDER BY peer_id, ts DESC"
        ") latest "
        "ORDER BY ts DESC, peer_id";

    if (!q.exec(sql)) {
        qWarning() << "getChatList failed:" << q.lastError() << q.lastQuery();
        return out;
    }

    while (q.next()) {
        ChatPreview p;
        p.userId = q.value(0).toInt();
        p.username = q.value(1).toString();
        p.lastMessage = q.value(2).toString();
        p.unreadCount = q.value(3).toInt();
        out.push_back(p);
    }
    return out;
}
int ChatManager::getUnreadCount()
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT COUNT(*) FROM messages WHERE receiver_id = ? AND is_read = false");
    query.addBindValue(m_currentUserId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}
