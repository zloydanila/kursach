#include "RoomManager.h"
#include "database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

RoomManager::RoomManager(int userId, QObject *parent)
    : QObject(parent), m_userId(userId)
{
}

int RoomManager::createRoom(const QString& name, const QString& description, 
                           const QString& genre, int maxMembers, bool isPrivate)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "INSERT INTO rooms (name, description, genre, host_id, max_members, is_private, created_at) "
        "VALUES (?, ?, ?, ?, ?, ?, NOW()) RETURNING id"
    );
    query.addBindValue(name);
    query.addBindValue(description);
    query.addBindValue(genre);
    query.addBindValue(m_userId);
    query.addBindValue(maxMembers);
    query.addBindValue(isPrivate);
    
    if (query.exec() && query.next()) {
        int roomId = query.value(0).toInt();
        
        QSqlQuery memberQuery(DatabaseManager::instance().database());
        memberQuery.prepare(
            "INSERT INTO room_members (room_id, user_id, is_host, joined_at) "
            "VALUES (?, ?, true, NOW())"
        );
        memberQuery.addBindValue(roomId);
        memberQuery.addBindValue(m_userId);
        memberQuery.exec();
        
        emit roomCreated(roomId);
        return roomId;
    }
    
    return -1;
}

bool RoomManager::deleteRoom(int roomId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM rooms WHERE id = ? AND host_id = ?");
    query.addBindValue(roomId);
    query.addBindValue(m_userId);
    
    if (query.exec()) {
        emit roomDeleted(roomId);
        return true;
    }
    
    return false;
}

bool RoomManager::joinRoom(int roomId)
{
    QSqlQuery checkQuery(DatabaseManager::instance().database());
    checkQuery.prepare(
        "SELECT COUNT(*) FROM room_members WHERE room_id = ?"
    );
    checkQuery.addBindValue(roomId);
    
    if (!checkQuery.exec() || !checkQuery.next()) {
        return false;
    }
    
    int currentMembers = checkQuery.value(0).toInt();
    
    QSqlQuery roomQuery(DatabaseManager::instance().database());
    roomQuery.prepare("SELECT max_members FROM rooms WHERE id = ?");
    roomQuery.addBindValue(roomId);
    
    if (!roomQuery.exec() || !roomQuery.next()) {
        return false;
    }
    
    int maxMembers = roomQuery.value(0).toInt();
    
    if (currentMembers >= maxMembers) {
        return false;
    }
    
    QSqlQuery insertQuery(DatabaseManager::instance().database());
    insertQuery.prepare(
        "INSERT INTO room_members (room_id, user_id, is_host, joined_at) "
        "VALUES (?, ?, false, NOW()) ON CONFLICT (room_id, user_id) DO NOTHING"
    );
    insertQuery.addBindValue(roomId);
    insertQuery.addBindValue(m_userId);
    
    if (insertQuery.exec()) {
        emit roomJoined(roomId);
        return true;
    }
    
    return false;
}

bool RoomManager::leaveRoom(int roomId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM room_members WHERE room_id = ? AND user_id = ?");
    query.addBindValue(roomId);
    query.addBindValue(m_userId);
    
    if (query.exec()) {
        emit roomLeft(roomId);
        return true;
    }
    
    return false;
}

QVector<Room> RoomManager::getPublicRooms()
{
    QVector<Room> rooms;
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT r.id, r.name, r.description, r.genre, r.host_id, "
        "r.current_track_id, r.current_position, r.is_playing, r.max_members, "
        "r.created_at, t.title, t.artist, "
        "(SELECT COUNT(*) FROM room_members WHERE room_id = r.id) as member_count "
        "FROM rooms r "
        "LEFT JOIN tracks t ON t.id = r.current_track_id "
        "WHERE r.is_private = false "
        "ORDER BY r.created_at DESC"
    );
    
    if (query.exec()) {
        while (query.next()) {
            Room room;
            room.id = query.value(0).toInt();
            room.name = query.value(1).toString();
            room.description = query.value(2).toString();
            room.genre = query.value(3).toString();
            room.hostId = query.value(4).toInt();
            room.currentTrackId = query.value(5).toInt();
            room.currentPosition = query.value(6).toLongLong();
            room.isPlaying = query.value(7).toBool();
            room.maxMembers = query.value(8).toInt();
            room.createdAt = query.value(9).toDateTime();
            room.currentTrackTitle = query.value(10).toString();
            room.currentTrackArtist = query.value(11).toString();
            
            int memberCount = query.value(12).toInt();
            room.members.resize(memberCount);
            
            rooms.append(room);
        }
    }
    
    return rooms;
}

Room RoomManager::getRoomDetails(int roomId)
{
    Room room;
    room.id = -1;
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT id, name, description, genre, host_id, current_track_id, "
        "current_position, is_playing, max_members, created_at "
        "FROM rooms WHERE id = ?"
    );
    query.addBindValue(roomId);
    
    if (query.exec() && query.next()) {
        room.id = query.value(0).toInt();
        room.name = query.value(1).toString();
        room.description = query.value(2).toString();
        room.genre = query.value(3).toString();
        room.hostId = query.value(4).toInt();
        room.currentTrackId = query.value(5).toInt();
        room.currentPosition = query.value(6).toLongLong();
        room.isPlaying = query.value(7).toBool();
        room.maxMembers = query.value(8).toInt();
        room.createdAt = query.value(9).toDateTime();
    }
    
    return room;
}

QVector<RoomMember> RoomManager::getRoomMembers(int roomId)
{
    QVector<RoomMember> members;
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT u.id, u.username, u.avatar_path, rm.is_host, rm.joined_at "
        "FROM room_members rm "
        "INNER JOIN users u ON u.id = rm.user_id "
        "WHERE rm.room_id = ? "
        "ORDER BY rm.is_host DESC, rm.joined_at"
    );
    query.addBindValue(roomId);
    
    if (query.exec()) {
        while (query.next()) {
            RoomMember member;
            member.userId = query.value(0).toInt();
            member.username = query.value(1).toString();
            member.avatarPath = query.value(2).toString();
            member.isHost = query.value(3).toBool();
            member.joinedAt = query.value(4).toDateTime();
            
            members.append(member);
        }
    }
    
    return members;
}

bool RoomManager::updateRoomTrack(int roomId, int trackId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "UPDATE rooms SET current_track_id = ?, current_position = 0 "
        "WHERE id = ? AND host_id = ?"
    );
    query.addBindValue(trackId);
    query.addBindValue(roomId);
    query.addBindValue(m_userId);
    
    if (query.exec()) {
        emit roomUpdated(roomId);
        return true;
    }
    
    return false;
}

bool RoomManager::updatePlaybackState(int roomId, bool isPlaying, qint64 position)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "UPDATE rooms SET is_playing = ?, current_position = ? "
        "WHERE id = ? AND host_id = ?"
    );
    query.addBindValue(isPlaying);
    query.addBindValue(position);
    query.addBindValue(roomId);
    query.addBindValue(m_userId);
    
    return query.exec();
}
