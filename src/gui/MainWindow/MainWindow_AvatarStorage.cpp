#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "MainWindow.h"
#include "database/DatabaseManager.h"

static QString avatarCacheFilePath(int userId)
{
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base + "/avatars");
    return base + QString("/avatars/user%1.png").arg(userId);
}

bool MainWindow::saveAvatarToDbAndCache(const QPixmap& pixmap)
{
    if (pixmap.isNull()) return false;

    QByteArray bytes;
    {
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        if (!pixmap.save(&buffer, "PNG")) return false;
    }

    {
        QSqlQuery q(DatabaseManager::instance().database());
        q.prepare("UPDATE users SET avatar_data = ? WHERE id = ?");
        q.addBindValue(bytes);
        q.addBindValue(currentUserId);
        if (!q.exec()) {
            qDebug() << "saveAvatar db error:" << q.lastError().text();
            return false;
        }
    }

    const QString cachePath = avatarCacheFilePath(currentUserId);
    QFile f(cachePath);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(bytes);
        f.close();
    }

    return true;
}

void MainWindow::loadUserAvatar()
{
    {
        QSqlQuery q(DatabaseManager::instance().database());
        q.prepare("SELECT avatar_data FROM users WHERE id = ?");
        q.addBindValue(currentUserId);

        if (q.exec() && q.next()) {
            const QByteArray bytes = q.value(0).toByteArray();
            if (!bytes.isEmpty()) {
                QPixmap px;
                if (px.loadFromData(bytes, "PNG")) {
                    setAvatarPixmap(px);

                    const QString cachePath = avatarCacheFilePath(currentUserId);
                    QFile f(cachePath);
                    if (f.open(QIODevice::WriteOnly)) {
                        f.write(bytes);
                        f.close();
                    }
                    return;
                }
            }
        } else if (q.lastError().isValid()) {
            qDebug() << "loadUserAvatar db error:" << q.lastError().text();
        }
    }

    const QString cachePath = avatarCacheFilePath(currentUserId);
    if (QFile::exists(cachePath)) {
        QPixmap px(cachePath);
        if (!px.isNull()) {
            setAvatarPixmap(px);
            return;
        }
    }

    setDefaultAvatar();
}
