#ifndef FRIENDREQUEST_H
#define FRIENDREQUEST_H

#include <QString>

struct FriendRequest{

    int id;
    int fromUserId;
    int toUserId;
    QString status; // "принято, отправлено, отклонено"

    FriendRequest(): id(0), fromUserId(0), toUserId(0){}

};

#endif