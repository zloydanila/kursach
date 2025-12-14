#include <gui/MainWindow/MainWindow.h>
#include "pages/FriendsPage.h"
#include "pages/MessagesPage.h"
#include <QVBoxLayout>
#include <QInputDialog>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QLineEdit>
#include <QListWidget>
#include <QTextEdit>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QToolButton>
#include <QTimer>
#include <QApplication>
#include <QMenu>
#include <QEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QDebug>
#include "database/DatabaseManager.h"
#include "database/api/MusicAPIManager.h"
#include "audio/AudioPlayer.h"
#include "core/models/Track.h"

MainWindow::MainWindow(const QString &username, int userId, QWidget *parent)
    : FramelessWindow(parent)
    , currentUsername(username)
    , currentUserId(userId)
    , sidebar(nullptr)
    , avatarButton(nullptr)
    , avatarOverlay(nullptr)
    , usernameLabel(nullptr)
    , profileBtn(nullptr)
    , messagesBtn(nullptr)
    , friendsBtn(nullptr)
    , notificationsBtn(nullptr)
    , playlistBtn(nullptr)
    , musicSearchBtn(nullptr)
    , myMusicBtn(nullptr)
    , roomsBtn(nullptr)
    , mainStack(nullptr)
    , profilePage(nullptr)
    , messagesPage(nullptr)
    , friendsPage(nullptr)
    , notificationsPage(nullptr)
    , playlistPage(nullptr)
    , musicPage(nullptr)
    , myMusicPage(nullptr)
    , roomsPage(nullptr)
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
    , currentRadioIndex(-1)
{
    setupUI();
    setupConnections();
    setupAvatar();
    loadUserAvatar();
    loadUserRadio();

    setWindowTitle("Chorus - " + username);

    QFile styleFile("styles/styles.css");
    if (styleFile.open(QIODevice::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        setStyleSheet(styleSheet);
    }

    if (avatarButton) {
        avatarButton->installEventFilter(this);
    }

    setMinimumSize(1200, 750);
    resize(1400, 850);

    QFont font = QApplication::font();
    font.setPointSize(10);
    QApplication::setFont(font);
}

MainWindow::~MainWindow()
{
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == avatarButton) {
        if (event->type() == QEvent::Enter) {
            showAvatarOverlay();
            return true;
        } else if (event->type() == QEvent::Leave) {
            hideAvatarOverlay();
            return true;
        }
    }

    if (obj == avatarOverlay) {
        if (event->type() == QEvent::MouseButtonPress) {
            changeAvatar();
            hideAvatarOverlay();
            return true;
        }
    }

    return FramelessWindow::eventFilter(obj, event);
}