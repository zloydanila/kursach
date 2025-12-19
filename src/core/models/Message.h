#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QDateTime>

enum class MessageType {
    Text = 0,
    Image = 1,
    Audio = 2,
    TrackShare = 3
};

struct Message {
    int id;
    int senderId;
    int receiverId;
    QString senderUsername;
    QString senderAvatarPath;
    QString content;
    MessageType type;
    QDateTime timestamp;
    bool isRead;
    int sharedTrackId;
    QString trackTitle;
    QString trackArtist;
};

struct ChatPreview {
    int userId;
    QString username;
    QString avatarPath;
    QString lastMessage;
    QDateTime lastMessageTime;
    int unreadCount;
    bool isOnline;
};

#endif
