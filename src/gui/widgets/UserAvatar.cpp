#include "UserAvatar.h"
#include <QPainter>
#include <QPainterPath>

UserAvatar::UserAvatar(int size, QWidget *parent)
    : QWidget(parent)
    , m_size(size)
    , m_status(UserStatus::Offline)
    , m_hasAvatar(false)
{
    setFixedSize(size, size);
}

void UserAvatar::setUser(const User& user)
{
    m_status = user.status;
    
    if (!user.avatarPath.isEmpty()) {
        setAvatar(user.avatarPath);
    } else {
        m_initials = user.username.left(1).toUpper();
        m_hasAvatar = false;
    }
    
    update();
}

void UserAvatar::setAvatar(const QString& avatarPath)
{
    QPixmap pixmap(avatarPath);
    if (!pixmap.isNull()) {
        m_avatar = pixmap.scaled(m_size, m_size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        m_hasAvatar = true;
    } else {
        m_hasAvatar = false;
    }
    update();
}

void UserAvatar::setStatus(UserStatus status)
{
    m_status = status;
    update();
}

void UserAvatar::setInitials(const QString& initials)
{
    m_initials = initials;
    m_hasAvatar = false;
    update();
}

void UserAvatar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QPainterPath circlePath;
    circlePath.addEllipse(0, 0, m_size, m_size);
    painter.setClipPath(circlePath);
    
    if (m_hasAvatar) {
        painter.drawPixmap(0, 0, m_avatar);
    } else {
        QLinearGradient gradient(0, 0, m_size, m_size);
        gradient.setColorAt(0, QColor(138, 43, 226));
        gradient.setColorAt(1, QColor(155, 75, 255));
        painter.fillPath(circlePath, gradient);
        
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setPointSize(m_size / 2.5);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, m_initials);
    }
    
    painter.setClipping(false);
    
    if (m_status != UserStatus::Offline) {
        QColor statusColor;
        switch (m_status) {
            case UserStatus::Online: statusColor = QColor(76, 175, 80); break;
            case UserStatus::Away: statusColor = QColor(255, 193, 7); break;
            case UserStatus::DoNotDisturb: statusColor = QColor(244, 67, 54); break;
            default: statusColor = QColor(158, 158, 158); break;
        }
        
        int statusSize = m_size / 4;
        int statusX = m_size - statusSize - 2;
        int statusY = m_size - statusSize - 2;
        
        painter.setBrush(statusColor);
        painter.setPen(QPen(QColor(15, 15, 20), 2));
        painter.drawEllipse(statusX, statusY, statusSize, statusSize);
    }
}
