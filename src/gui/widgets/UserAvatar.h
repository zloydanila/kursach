#ifndef USERAVATAR_H
#define USERAVATAR_H

#include <QWidget>
#include <QPixmap>
#include "core/models/User.h"

class UserAvatar : public QWidget
{
    Q_OBJECT

public:
    explicit UserAvatar(int size = 40, QWidget *parent = nullptr);
    
    void setUser(const User& user);
    void setAvatar(const QString& avatarPath);
    void setStatus(UserStatus status);
    void setInitials(const QString& initials);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_size;
    QPixmap m_avatar;
    QString m_initials;
    UserStatus m_status;
    bool m_hasAvatar;
};

#endif
