#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QPushButton;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& username, QWidget *parent = nullptr); 
    ~MainWindow();

private slots:
    void showProfilePage();
    void showMessagesPage();
    void showFriendsPage();
    void showNotificationsPage();
    void showPlaylistPage();

private:
    enum PageIndex {
        PROFILE_PAGE = 0,
        MESSAGES_PAGE = 1,
        FRIENDS_PAGE = 2,
        NOTIFICATIONS_PAGE = 3,
        PLAYLIST_PAGE = 4
    };
    void setupUI();
    void setupConnections();
    void createSidebar();
    void createPages();
    
    QString currentUsername;
    

    QWidget *sidebar;
    QLabel *avatarLabel;
    QLabel *usernameLabel;
    QPushButton *profileBtn;
    QPushButton *messagesBtn;
    QPushButton *friendsBtn;
    QPushButton *notificationsBtn;
    QPushButton *playlistBtn;
    

    QStackedWidget *mainStack;
    QWidget *profilePage;
    QWidget *messagesPage;
    QWidget *friendsPage;
    QWidget *notificationsPage;
    QWidget *playlistPage;
};

#endif