#ifndef FRIENDSPAGE_H
#define FRIENDSPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include "network/FriendManager.h"

class FriendsPage : public QWidget
{
    Q_OBJECT

public:
    explicit FriendsPage(int userId, QWidget *parent = nullptr);
    
    void refreshData();

signals:
    void openChatWithFriend(int friendId, const QString& friendName);

private slots:
    void onSearchClicked();
    void onShowFriends();
    void onShowRequests();
    void onAddFriend(int userId);
    void onAcceptRequest(int userId);
    void onRejectRequest(int userId);
    void onRemoveFriend(int userId);
    void onMessageFriend(int userId);

private:
    void setupUI();
    void loadFriends();
    void loadRequests();
    void searchUsers();
    void displaySearchResults(const QVector<User>& users);
    
    int m_userId;
    FriendManager* m_friendManager;
    
    QLineEdit* m_searchInput;
    QPushButton* m_friendsBtn;
    QPushButton* m_requestsBtn;
    QStackedWidget* m_stackedWidget;
    QScrollArea* m_friendsWidget;
    QScrollArea* m_requestsWidget;
    QScrollArea* m_searchWidget;
    QVBoxLayout* m_friendsLayout;
    QVBoxLayout* m_requestsLayout;
    QVBoxLayout* m_searchLayout;
};

#endif
