#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>

enum class UserStatus {
    Online = 0,
    Away = 1,
    DoNotDisturb = 2,
    Invisible = 3,
    Offline = 4
};

struct User {
    int id;
    QString username;
    QString email;
    QString avatarPath;
    
    QByteArray avatarData;
UserStatus status;
    QDateTime lastSeen;
    QString bio;
    QDateTime createdAt;
    bool isFriend;
    bool friendRequestSent;
    bool friendRequestReceived;
};

#endif
