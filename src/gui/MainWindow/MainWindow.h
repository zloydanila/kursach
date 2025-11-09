#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QVariantMap>
#include <QLabel>
#include <QStackedWidget>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVector>
#include <QEvent>
#include <QMediaPlayer>
#include <QTabWidget>

#include "database/api/MusicAPIManager.h"
#include "database/socialNetwork/FriendsManager.h"
#include "database/socialNetwork/MessagesManager.h"
#include "AudioPlayer/AudioPlayer.h"
#include "models/Track.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& username, int userId, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* ev) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    // ---- Навигация между страницами ----
    void showProfilePage();
    void showMessagesPage();
    void showFriendsPage();
    void showNotificationsPage();
    void showPlaylistPage();

    // ---- Поиск музыки (Jamendo / Last.fm) ----
    void searchMusic();
    void showTopTracks();
    void onTracksFound(const QVariantList& tracks);
    void onNetworkError(const QString& error);

    // ---- Управление плеером ----
    void addSelectedTrackToPlaylist();
    void playSelectedTrack();
    void pausePlayback();
    void stopPlayback();
    void onPlaybackStateChanged(QMediaPlayer::State state);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);

    // ---- Друзья ----
    void searchUsers();
    void onUsersFound(const QList<QVariantMap>& users);
    void sendFriendRequest(int userId);
    void acceptFriendRequest(int userId);
    void rejectFriendRequest(int userId);
    void removeFriend(int friendId);
    void loadFriends();
    void loadFriendRequests();
    void refreshFriendsData();

    // ---- Сообщения ----
    void refreshDialogs();
    void openChat(int friendId);
    void sendMessage();
    void onNewMessageReceived(int fromUserId, const QString& username, const QString& message, const QDateTime& timestamp);

private:
    // ---- Инициализация UI ----
    void setupUI();
    void setupConnections();
    void createSidebar();
    void createPages();
    void createMessagesPage();
    void createFriendsPage();
    void createMusicPage();

    // ---- Аватар ----
    void loadUserAvatar();
    void setupAvatar();
    void setAvatarPixmap(const QPixmap& pixmap);
    void setDefaultAvatar();
    void saveAvatar(const QPixmap& avatar);
    void changeAvatar();
    void onAvatarButtonEnter();
    void onAvatarButtonLeave();

    // ---- Вспомогательные методы ----
    void enableSearchControls(bool enable);
    int bubbleMaxWidth() const;
    void addMessageBubble(const QVariantMap& message, const QString& friendName);
    QPixmap makeAvatar(int userId, const QString &username, int size);
    QString formatTime(qint64 milliseconds);

    // ---- Моя музыка (локальная / библиотека пользователя) ----
    void loadUserTracks();
    void onPlaylistItemClicked(QListWidgetItem *item);
    void onTrackDeleteRequested(int trackId);
    void onAddToPlaylistRequested(int trackId);
    void playTrack(int trackId);
    void playNextTrack();
    void playPrevTrack();
    void onTrackFinished();
    void deleteAllTracks();

private:
    // ===== Данные пользователя =====
    QString currentUsername;
    int     currentUserId;

    // ===== Навигация / левое меню =====
    QWidget     *sidebar;
    QPushButton *avatarButton;
    QLabel      *avatarOverlay;
    QLabel      *usernameLabel;

    QPushButton *myMusicBtn;
    QPushButton *profileBtn;
    QPushButton *musicSearchBtn;
    QPushButton *playlistBtn;
    QPushButton *friendsBtn;
    QPushButton *messagesBtn;
    QPushButton *notificationsBtn;

    // ===== Основные страницы =====
    QStackedWidget *mainStack;
    QWidget        *myMusicPage;
    QWidget        *profilePage;
    QWidget        *messagesPage;
    QWidget        *friendsPage;
    QWidget        *notificationsPage;
    QWidget        *playlistPage;
    QWidget        *musicPage;

    // ===== Страница "Моя музыка" =====
    QListWidget  *userTracksList;
    QHBoxLayout  *controlsLayout;
    QPushButton  *refreshTracksBtn;
    QPushButton  *addLocalTrackBtn;
    QString       controlButtonStyle;

    QVector<Track> currentTracks;
    int            currentTrackIndex;

    // ===== Страница поиска музыки =====
    QLineEdit   *searchInput;
    QPushButton *searchButton;
    QPushButton *topTracksButton;
    QListWidget *tracksList;
    QTextEdit   *trackInfo;

    // ===== Управление плеером =====
    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QLabel      *currentTimeLabel;
    QLabel      *totalTimeLabel;
    QLabel      *nowPlayingLabel;
    QMediaPlayer *mediaPlayer;
    QString      currentAudioUrl;

    // ===== Друзья =====
    QLineEdit   *friendSearchInput;
    QPushButton *friendSearchButton;
    QListWidget *usersList;
    QListWidget *friendsList;
    QListWidget *friendRequestsList;
    QTabWidget  *friendsTabWidget;

    // ===== Сообщения =====
    QSplitter    *messagesSplitter;
    QListWidget  *dialogsList;
    QLabel       *chatHeaderAvatar;
    QLabel       *chatHeaderTitle;
    QListWidget  *conversationList;
    QLineEdit    *messageInput;
    QPushButton  *sendMessageButton;
    int           currentChatUserId;

    // ===== Менеджеры =====
    MusicAPIManager *apiManager;
    AudioPlayer     *audioPlayer;

    // ===== Индексы страниц =====
    enum PageIndex {
        MY_MUSIC_PAGE = 0,
        PROFILE_PAGE,
        MESSAGES_PAGE,
        FRIENDS_PAGE,
        NOTIFICATIONS_PAGE,
        PLAYLIST_PAGE,
        MUSIC_PAGE
    };
};

#endif // MAINWINDOW_H