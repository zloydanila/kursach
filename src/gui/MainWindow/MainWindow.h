#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include "../Frameless/FramelessWindow.h"
#include "core/models/Track.h"

class FriendsPage;
class MessagesPage;
class MusicAPIManager;
class AudioPlayer;

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
    void showRoomsPage();
    void showMyMusicPage();
    void showSearchRadioPage();

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
    bool saveAvatarToDbAndCache(const QPixmap& pixmap);
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
    QPushButton *musicSearchBtn;
    QPushButton *myMusicBtn;
    QPushButton *roomsBtn;

    QStackedWidget *mainStack;

    QWidget *profilePage;
    QWidget *notificationsPage;
    QWidget *musicPage;     // SearchMusicPage
    QWidget *myMusicPage;   // MyMusicPage
    QWidget *roomsPage;     // RoomsPage

    FriendsPage *friendsPage;
    MessagesPage *messagesPage;

    // Старые поля поиска по трекам — оставим, чтобы конструктор не падал
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

#endif
