#include "MainWindow.h"
#include <QApplication>
#include <QFile>
#include <QMouseEvent>
#include <QCloseEvent>

#include "database/api/MusicAPIManager.h"
#include "audio/AudioPlayer.h"
#include "database/DatabaseManager.h"

MainWindow::MainWindow(const QString &username, int userId, QWidget *parent)
    : FramelessWindow(parent)
    , currentUsername(username)
    , currentUserId(userId)
    , currentRadioIndex(-1)
    , sidebar(nullptr)
    , avatarButton(nullptr)
    ,
    avatarOverlay(nullptr)
    , usernameLabel(nullptr)
    , profileBtn(nullptr)
    , messagesBtn(nullptr)
    , friendsBtn(nullptr)
    , notificationsBtn(nullptr)
    , musicSearchBtn(nullptr)
    , myMusicBtn(nullptr)
    , roomsBtn(nullptr)
    , mainStack(nullptr)
    , profilePage(nullptr)
    , notificationsPage(nullptr)
    , musicPage(nullptr)
    , myMusicPage(nullptr)
    , roomsPage(nullptr)
    , messagesPage(nullptr)
    , friendsPage(nullptr)
    , searchInput(nullptr)
    , searchButton(nullptr)
    , topTracksButton(nullptr)
    , tracksList(nullptr)
    , trackInfo(nullptr)
    , userTracksList(nullptr)
    , refreshTracksBtn(nullptr)
    , addLocalTrackBtn(nullptr)
    , apiManager(new MusicAPIManager(this))
    , audioPlayer(new AudioPlayer(this))
{
    setupUI();
    setupConnections();
    setupAvatar();
    loadUserAvatar();
    loadUserRadio();

    DatabaseManager::instance().setUserStatus(currentUserId, UserStatus::Online);

    setWindowTitle("Chorus - " + username);

    QFile styleFile("styles/styles.css");
    if (styleFile.open(QIODevice::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        setStyleSheet(styleSheet);
    }

    if (avatarButton) avatarButton->installEventFilter(this);

    setMinimumSize(1200, 750);
    resize(1400, 850);

    QFont font = QApplication::font();
    font.setPointSize(10);
    QApplication::setFont(font);
}

MainWindow::~MainWindow() {}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
         return FramelessWindow::eventFilter(obj, event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    DatabaseManager::instance().setUserStatus(currentUserId, UserStatus::Offline);
    FramelessWindow::closeEvent(event);
}
