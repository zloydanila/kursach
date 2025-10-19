#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QLineEdit>
#include <QListWidget>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include "api/MusicAPIManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& username, int userId, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showProfilePage();
    void showMessagesPage();
    void showFriendsPage();
    void showNotificationsPage();
    void showPlaylistPage();
    void searchMusic();
    void onTracksFound(const QVariantList& tracks);
    void showTopTracks();
    void changeAvatar();

private:
    void setupUI();
    void setupConnections();
    void createSidebar();
    void createPages();
    void createMusicPage();
    void loadUserAvatar();
    void setupAvatar();
    void setAvatarPixmap(const QPixmap& pixmap);
    void setDefaultAvatar();
    void saveAvatar(const QPixmap& avatar);

    QString currentUsername;
    int currentUserId;
    
    // UI элементы
    QWidget *sidebar;
    QPushButton *avatarButton;
    QLabel *usernameLabel;
    QPushButton *profileBtn;
    QPushButton *messagesBtn;
    QPushButton *friendsBtn;
    QPushButton *notificationsBtn;
    QPushButton *playlistBtn;
    QPushButton *musicSearchBtn;
    
    QStackedWidget *mainStack;
    QWidget *profilePage;
    QWidget *messagesPage;
    QWidget *friendsPage;
    QWidget *notificationsPage;
    QWidget *playlistPage;
    QWidget *musicPage;
    
    // Элементы для страницы музыки
    QLineEdit *searchInput;
    QPushButton *searchButton;
    QPushButton *topTracksButton;
    QListWidget *tracksList;
    QTextEdit *trackInfo;
    
    MusicAPIManager *apiManager;
    QString avatarPath;

    enum PageIndex {
        PROFILE_PAGE = 0,
        MESSAGES_PAGE,
        FRIENDS_PAGE,
        NOTIFICATIONS_PAGE,
        PLAYLIST_PAGE,
        MUSIC_PAGE
    };
};

#endif 