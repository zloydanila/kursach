#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include "core/models/User.h"

class UserManager : public QObject
{
    Q_OBJECT

public:
    explicit UserManager(QObject *parent = nullptr);
    
    bool updateStatus(int userId, UserStatus status);
    bool updateBio(int userId, const QString& bio);
    bool updateAvatar(int userId, const QString& avatarPath);
    bool updateEmail(int userId, const QString& email);
    
    User getUser(int userId);
    UserStatus getUserStatus(int userId);
    QString getUserAvatar(int userId);

signals:
    void userUpdated(int userId);
    void statusChanged(int userId, UserStatus status);

private:
};

#endif
