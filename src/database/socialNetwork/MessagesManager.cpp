#include <QDebug>
#include <QDateTime>
#include "../DatabaseManager.h"
#include "MessagesManager.h"

MessagesManager& MessagesManager::instance(){
    static MessagesManager instance;
    return instance;
}

bool MessagesManager::sendMessage(int fromUserId, int toUserId, const QString& text){
    if(fromUserId == toUserId){
        qDebug() << "Нельзя отправить сообщение самому себе";
        return false;
    }
    if(text.trimmed().isEmpty()){
        qDebug() << "Сообщение не может быть пустым";
        return false;
    }

    QSqlQuery query;
    query.prepare(
        "INSERT INTO messages (from_user_id, to_user_id, message_text) " 
        "VALUES (?, ?, ?)"
    );
    query.addBindValue(fromUserId);
    query.addBindValue(toUserId);
    query.addBindValue(text.trimmed());
    if(query.exec()){
        QString username = getUsername(fromUserId);
        emit newMessageReceived(toUserId, username, text.trimmed(), QDateTime::currentDateTime());
        return true;
    }else{
        return false;
    }
}


QList<QVariantMap> MessagesManager::getConversation(int user1Id, int user2Id, int limit)
{
    QList<QVariantMap> messages;
    QSqlQuery query;
    query.prepare(
        "SELECT m.id, m.from_user_id, m.to_user_id, m.message_text, m.is_read, m.created_at, "
        "u_from.username as from_username, u_to.username as to_username "
        "FROM messages m "
        "JOIN users u_from ON m.from_user_id = u_from.id "
        "JOIN users u_to ON m.to_user_id = u_to.id "
        "WHERE (m.from_user_id = ? AND m.to_user_id = ?) OR "
        "      (m.from_user_id = ? AND m.to_user_id = ?) "
        "ORDER BY m.created_at DESC "
        "LIMIT ?"
    );
    query.addBindValue(user1Id);
    query.addBindValue(user2Id);
    query.addBindValue(user2Id);
    query.addBindValue(user1Id);
    query.addBindValue(limit);
    
    if (query.exec()) {
        while (query.next()) {
            QVariantMap message;
            message["id"] = query.value("id").toInt();
            message["from_user_id"] = query.value("from_user_id").toInt();
            message["to_user_id"] = query.value("to_user_id").toInt();
            message["from_username"] = query.value("from_username").toString();
            message["to_username"] = query.value("to_username").toString();
            message["text"] = query.value("message_text").toString();
            message["is_read"] = query.value("is_read").toBool();
            message["timestamp"] = query.value("created_at").toDateTime();
            message["is_own"] = (query.value("from_user_id").toInt() == user1Id);
            
            messages.append(message);
        }
    }

    std::reverse(messages.begin(), messages.end());
    
    return messages;
}

QList<QVariantMap> MessagesManager::getDialogs(int userId)
{
    QList<QVariantMap> dialogs;
    
    QSqlQuery query;
    query.prepare(
        "SELECT DISTINCT "
        "  CASE WHEN m.from_user_id = ? THEN m.to_user_id ELSE m.from_user_id END as other_user_id "
        "FROM messages m "
        "WHERE m.from_user_id = ? OR m.to_user_id = ?"
    );
    query.addBindValue(userId);
    query.addBindValue(userId);
    query.addBindValue(userId);
    
    if (!query.exec()) {
        return dialogs;
    }
    
    QList<int> otherUserIds;
    while (query.next()) {
        otherUserIds.append(query.value("other_user_id").toInt());
    }
    
    for (int otherUserId : otherUserIds) {
        QSqlQuery userQuery;
        userQuery.prepare("SELECT username FROM users WHERE id = ?");
        userQuery.addBindValue(otherUserId);
        
        if (userQuery.exec() && userQuery.next()) {
            QVariantMap dialog;
            dialog["user_id"] = otherUserId;
            dialog["username"] = userQuery.value("username").toString();
            
            QSqlQuery lastMsgQuery;
            lastMsgQuery.prepare(
                "SELECT message_text FROM messages "
                "WHERE (from_user_id = ? AND to_user_id = ?) OR (from_user_id = ? AND to_user_id = ?) "
                "ORDER BY created_at DESC LIMIT 1"
            );
            lastMsgQuery.addBindValue(userId);
            lastMsgQuery.addBindValue(otherUserId);
            lastMsgQuery.addBindValue(otherUserId);
            lastMsgQuery.addBindValue(userId);
            
            if (lastMsgQuery.exec() && lastMsgQuery.next()) {
                dialog["last_message"] = lastMsgQuery.value("message_text").toString();
            } else {
                dialog["last_message"] = "Нет сообщений";
            }
            
            dialog["unread_count"] = 0; 
            
            dialogs.append(dialog);
        }
    }
    
    return dialogs;
}
int MessagesManager::getUnreadCount(int userId)
{
    QSqlQuery query;
    query.prepare(
        "SELECT COUNT(*) as unread_count "
        "FROM messages "
        "WHERE to_user_id = ? AND is_read = false"
    );
    query.addBindValue(userId);
    
    if (query.exec() && query.next()) {
        return query.value("unread_count").toInt();
    }
    
    return 0;
}

bool MessagesManager::markMessagesAsRead(int user1Id, int user2Id)
{
    QSqlQuery query;
    query.prepare(
        "UPDATE messages SET is_read = true "
        "WHERE from_user_id = ? AND to_user_id = ? AND is_read = false"
    );
    query.addBindValue(user2Id);
    query.addBindValue(user1Id);
    
    return query.exec();
}

bool MessagesManager::deleteMessage(int messageId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM messages WHERE id = ?");
    query.addBindValue(messageId);
    
    return query.exec();
}

QString MessagesManager::getUsername(int userId)
{
    QSqlQuery query;
    query.prepare("SELECT username FROM users WHERE id = ?");
    query.addBindValue(userId);
    
    if (query.exec() && query.next()) {
        return query.value("username").toString();
    }
    
    return "Unknown User";
}
