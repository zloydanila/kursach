#ifndef ROOMMANAGER_H
#define ROOMMANAGER_H

#include <QObject>
#include <QVector>
#include "core/models/Room.h"

class RoomManager : public QObject
{
    Q_OBJECT

public:
    explicit RoomManager(int userId, QObject *parent = nullptr);
    
    int createRoom(const QString& name, const QString& description, 
                   const QString& genre, int maxMembers, bool isPrivate);
    bool deleteRoom(int roomId);
    bool joinRoom(int roomId);
    bool leaveRoom(int roomId);
    
    QVector<Room> getPublicRooms();
    Room getRoomDetails(int roomId);
    QVector<RoomMember> getRoomMembers(int roomId);
    
    bool updateRoomTrack(int roomId, int trackId);
    bool updatePlaybackState(int roomId, bool isPlaying, qint64 position);

signals:
    void roomCreated(int roomId);
    void roomDeleted(int roomId);
    void roomJoined(int roomId);
    void roomLeft(int roomId);
    void roomUpdated(int roomId);
    void memberJoined(int roomId, const RoomMember& member);
    void memberLeft(int roomId, int userId);

private:
    int m_userId;
};

#endif
