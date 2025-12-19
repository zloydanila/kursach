#include "UserManager.h"
#include "database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QIODevice>
#include <QByteArray>
#include <QDebug>

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
    // 1) update avatar_path
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("UPDATE users SET avatar_path = ? WHERE id = ?");
    q.addBindValue(avatarPath);
    q.addBindValue(userId);
    if (!q.exec()) {
        qWarning() << "updateAvatar avatar_path failed:" << q.lastError();
        return false;
    }

    // 2) store bytes in avatar_data (so other clients can load it)
    QFile f(avatarPath);
    if (f.open(QIODevice::ReadOnly)) {
        const QByteArray bytes = f.readAll();

        QSqlQuery q2(DatabaseManager::instance().database());
        q2.prepare("UPDATE users SET avatar_data = ? WHERE id = ?");
        q2.addBindValue(bytes);
        q2.addBindValue(userId);

        if (!q2.exec()) {
            qWarning() << "updateAvatar avatar_data failed:" << q2.lastError();
            // avatar_path already updated, so keep true
        }
    }

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
