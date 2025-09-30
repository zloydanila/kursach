#ifndef USER_H
#define USER_H

#include "QString"

struct User
{
    int id;
    QString username;
    QString passwordHash;
    QString avatarPath;
    QString createdAt;
    
    User() : id(0) {}
};

#endif

