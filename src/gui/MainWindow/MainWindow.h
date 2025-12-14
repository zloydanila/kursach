#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QGraphicsDropShadowEffect>
#include "../Frameless/FramelessWindow.h"
#include "core/models/Track.h"
#include <vector>


class FriendsPage;
class MessagesPage;
class MusicAPIManager;
class AudioPlayer;


enum PageIndex {
    MYMUSICPAGE = 0,
    PROFILEPAGE = 1,
    MESSAGESPAGE = 2,
    FRIENDSPAGE = 3,
    NOTIFICATIONSPAGE = 4,
    PLAYLISTPAGE = 5,
    MUSICPAGE = 6,
    ROOMSPAGE = 7
};


class MainWindow : public FramelessWindow {
    Q_OBJECT


public:
    MainWindow(const QString &username, int userId, QWidget *parent = nullptr);
    ~MainWindow();
    


protected:
    bool eventFilter(QObject *obj, QEvent *event) override;


private slots:
    void setupConnections();
    void playRadio(int radioId);
    void onPlaylistItemClicked(QListWidgetItem *item);
    void onRadioDeleteRequested(int radioId);
    void loadUserRadio();
    void showProfilePage();
    void showMessagesPage();
    void showFriendsPage();
    void showNotificationsPage();
    void showPlaylistPage();
    void showRoomsPage();
    void changeAvatar();
    void onRadioStationAdded();


private:
    void setupUI();
    void createSidebar();
    void createPages();
    QWidget* createSimplePage(const QString& title, const QString& description);


    void setupAvatar();
    void setAvatarPixmap(const QPixmap& pixmap);
    void setDefaultAvatar();
    void saveAvatar(const QPixmap& avatar);
    void loadUserAvatar();
    void showAvatarOverlay();
    void hideAvatarOverlay();
    void onAvatarButtonEnter();
    void onAvatarButtonLeave();



    QString currentUsername;
    int currentUserId;
    QVector<TrackData> currentRadios;
    int currentRadioIndex;


    QWidget *sidebar;
    QPushButton *avatarButton;
    QLabel *avatarOverlay;
    QLabel *usernameLabel;


    QPushButton *profileBtn;
    QPushButton *messagesBtn;
    QPushButton *friendsBtn;
    QPushButton *notificationsBtn;
    QPushButton *playlistBtn;
    QPushButton *musicSearchBtn;
    QPushButton *myMusicBtn;
    QPushButton *roomsBtn;


    QStackedWidget *mainStack;
    QWidget *profilePage;
    QWidget *messagesPage;
    QWidget *friendsPage;
    QWidget *notificationsPage;
    QWidget *playlistPage;
    QWidget *musicPage;
    QWidget *myMusicPage;
    QWidget *roomsPage;


    QLineEdit *searchInput;
    QPushButton *searchButton;
    QPushButton *topTracksButton;
    QListWidget *tracksList;
    QTextEdit *trackInfo;
    QListWidget *userTracksList;
    QPushButton *refreshTracksBtn;
    QPushButton *addLocalTrackBtn;


    MusicAPIManager *apiManager;
    AudioPlayer *audioPlayer;
};


#endif // MAINWINDOW_H