#include "ChatManager.h"
#include "database/DatabaseManager.h"
#include <QSqlQuery>
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
    QVector<ChatPreview> chats;
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "WITH last_messages AS ( "
        "    SELECT DISTINCT ON (LEAST(sender_id, receiver_id), GREATEST(sender_id, receiver_id)) "
        "           CASE WHEN sender_id = ? THEN receiver_id ELSE sender_id END as other_user_id, "
        "           content, timestamp, is_read "
        "    FROM messages "
        "    WHERE sender_id = ? OR receiver_id = ? "
        "    ORDER BY LEAST(sender_id, receiver_id), GREATEST(sender_id, receiver_id), timestamp DESC "
        ") "
        "SELECT u.id, u.username, u.avatar_path, lm.content, lm.timestamp, "
        "       COUNT(m.id) FILTER (WHERE m.receiver_id = ? AND m.is_read = false) as unread, "
        "       u.status "
        "FROM last_messages lm "
        "INNER JOIN users u ON u.id = lm.other_user_id "
        "LEFT JOIN messages m ON (m.sender_id = u.id AND m.receiver_id = ?) OR (m.receiver_id = u.id AND m.sender_id = ?) "
        "GROUP BY u.id, u.username, u.avatar_path, lm.content, lm.timestamp, u.status "
        "ORDER BY lm.timestamp DESC"
    );
    query.addBindValue(m_currentUserId);
    query.addBindValue(m_currentUserId);
    query.addBindValue(m_currentUserId);
    query.addBindValue(m_currentUserId);
    query.addBindValue(m_currentUserId);
    query.addBindValue(m_currentUserId);
    
    if (query.exec()) {
        while (query.next()) {
            ChatPreview chat;
            chat.userId = query.value(0).toInt();
            chat.username = query.value(1).toString();
            chat.avatarPath = query.value(2).toString();
            chat.lastMessage = query.value(3).toString();
            chat.lastMessageTime = query.value(4).toDateTime();
            chat.unreadCount = query.value(5).toInt();
            chat.isOnline = query.value(6).toInt() == 0;
            
            chats.append(chat);
        }
    }
    
    return chats;
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
