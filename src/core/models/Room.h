#ifndef ROOM_H
#define ROOM_H

#include <QString>
#include <QDateTime>
#include <QVector>

struct RoomMember {
    int userId;
    QString username;
    QString avatarPath;
    bool isHost;
    QDateTime joinedAt;
};

struct Room {
    int id;
    QString name;
    QString description;
    QString genre;
    int hostId;
    int currentTrackId;
    QString currentTrackTitle;
    QString currentTrackArtist;
    qint64 currentPosition;
    bool isPlaying;
    int maxMembers;
    QVector<RoomMember> members;
    QDateTime createdAt;
    bool isPrivate;
    QString password;
};

#endif
