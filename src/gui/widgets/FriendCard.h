#ifndef FRIENDCARD_H
#define FRIENDCARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "core/models/User.h"

class FriendCard : public QWidget
{
    Q_OBJECT

public:
    explicit FriendCard(const User& user, bool isRequest, bool isSearchResult, QWidget *parent = nullptr);

signals:
    void acceptClicked();
    void rejectClicked();
    void removeClicked();
    void messageClicked();
    void addFriendClicked();

private:
    void setupUI(const User& user, bool isRequest, bool isSearchResult);
    QString getStatusColor(UserStatus status);
    QString getStatusText(UserStatus status);
    
    QLabel* m_avatarLabel;
    QLabel* m_usernameLabel;
    QLabel* m_statusLabel;
};

#endif

