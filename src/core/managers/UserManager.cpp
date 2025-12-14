#include "UserManager.h"

UserManager::UserManager(QObject *parent)
    : QObject(parent)
{
}

User UserManager::getUser(int userId)
{
    User user;
    user.id = userId;
    user.username = "User";
    user.status = UserStatus::Offline;
    return user;
}

bool UserManager::updateAvatar(int userId, const QString& avatarPath)
{
    Q_UNUSED(userId);
    Q_UNUSED(avatarPath);
    return true;
}

bool UserManager::updateEmail(int userId, const QString& email)
{
    Q_UNUSED(userId);
    Q_UNUSED(email);
    return true;
}

bool UserManager::updateBio(int userId, const QString& bio)
{
    Q_UNUSED(userId);
    Q_UNUSED(bio);
    return true;
}
