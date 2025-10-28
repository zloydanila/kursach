#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QProgressDialog>

class QStackedWidget;
class QListWidget;
class QPushButton;
class QLabel;
class QLineEdit;
class QTextEdit;
class QProgressDialog;
class MusicAPIManager;
class AudioPlayer;
struct Track;

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
    void loadUserTracks();
    void playTrack(int trackId);
    void onPlaylistItemClicked(QListWidgetItem *item);
    void onTrackDeleteRequested(int trackId);
    void onNetworkError(const QString& error);
    void onAvatarButtonEnter();
    void onAvatarButtonLeave();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void setupConnections();
    void createSidebar();
    void createPages();
    void createMusicPage();
    void createMyMusicPage();
    void loadUserAvatar();
    void setupAvatar();
    void setAvatarPixmap(const QPixmap& pixmap);
    void setDefaultAvatar();
    void saveAvatar(const QPixmap& avatar);
    void enableSearchControls(bool enable);

    QString currentUsername;
    int currentUserId;
    
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
    
    QStackedWidget *mainStack;
    QWidget *profilePage;
    QWidget *messagesPage;
    QWidget *friendsPage;
    QWidget *notificationsPage;
    QWidget *playlistPage;
    QWidget *musicPage;
    QWidget *myMusicPage;
    
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
    QLabel *loadingLabel;

    QString avatarPath;
    QList<Track> currentTracks;
    int currentTrackIndex;

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

#endif