#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QToolButton>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QTime>
#include <QStyle>
#include <QTimer>
#include <QApplication>
#include <QMenu>
#include <QEvent>
#include <QTabWidget>
#include <QMediaPlaylist>

#include "database/socialNetwork/FriendsManager.h"
#include "database/socialNetwork/MessagesManager.h"
#include "database/DatabaseManager.h"
#include "database/api/MusicAPIManager.h"
#include "database/api/AudioPlayer/TrackWidget.h"
#include "database/api/AudioPlayer/AudioPlayer.h"
#include "database/models/Track.h"

class TrackListWidgetItem : public QListWidgetItem {
public:
    TrackListWidgetItem(const QString& text, int trackId, QListWidget* parent = nullptr)
        : QListWidgetItem(text, parent), m_trackId(trackId) {}
    
    int trackId() const { return m_trackId; }
    
private:
    int m_trackId;
};

MainWindow::MainWindow(const QString& username, int userId, QWidget *parent)
    : QMainWindow(parent)
    , currentUsername(username)
    , currentUserId(userId)
    , sidebar(nullptr)
    , avatarButton(nullptr)
    , avatarOverlay(nullptr)
    , usernameLabel(nullptr)
    , myMusicBtn(nullptr)
    , profileBtn(nullptr)
    , musicSearchBtn(nullptr)
    , playlistBtn(nullptr)
    , friendsBtn(nullptr)
    , messagesBtn(nullptr)
    , notificationsBtn(nullptr)
    , mainStack(nullptr)
    , myMusicPage(nullptr)
    , profilePage(nullptr)
    , messagesPage(nullptr)
    , friendsPage(nullptr)
    , notificationsPage(nullptr)
    , playlistPage(nullptr)
    , musicPage(nullptr)
    , userTracksList(nullptr)
    , controlsLayout(nullptr)
    , refreshTracksBtn(nullptr)
    , addLocalTrackBtn(nullptr)
    , currentTrackIndex(-1)
    , searchInput(nullptr)
    , searchButton(nullptr)
    , topTracksButton(nullptr)
    , tracksList(nullptr)
    , trackInfo(nullptr)
    , playButton(nullptr)
    , pauseButton(nullptr)
    , stopButton(nullptr)
    , currentTimeLabel(nullptr)
    , totalTimeLabel(nullptr)
    , nowPlayingLabel(nullptr)
    , mediaPlayer(nullptr)
    , currentAudioUrl("")
    , friendSearchInput(nullptr)
    , friendSearchButton(nullptr)
    , usersList(nullptr)
    , friendsList(nullptr)
    , friendRequestsList(nullptr)
    , friendsTabWidget(nullptr)
    , messagesSplitter(nullptr)
    , dialogsList(nullptr)
    , chatHeaderAvatar(nullptr)
    , chatHeaderTitle(nullptr)
    , conversationList(nullptr)
    , messageInput(nullptr)
    , sendMessageButton(nullptr)
    , currentChatUserId(-1)
    , apiManager(new MusicAPIManager(this))
    , audioPlayer(nullptr)
{
    mediaPlayer = new QMediaPlayer(this);
    setupUI();
    setupConnections();
    loadUserAvatar();
    
    setWindowTitle("SoundSpace - " + username);
    setMinimumSize(1400, 800);
    setStyleSheet("QMainWindow { background-color: #121212; }");
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent* ev)
{
    QMainWindow::resizeEvent(ev);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // Простая реализация eventFilter
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    
    createSidebar();
    contentLayout->addWidget(sidebar);
    
    createPages();
    contentLayout->addWidget(mainStack, 1);
    
    mainLayout->addLayout(contentLayout, 1);
    
    // Создаем элементы управления плеером
    QWidget *playerControls = new QWidget();
    QHBoxLayout *playerLayout = new QHBoxLayout(playerControls);
    playerLayout->setContentsMargins(20, 10, 20, 10);
    
    playButton = new QPushButton("▶");
    pauseButton = new QPushButton("⏸");
    stopButton = new QPushButton("⏹");
    currentTimeLabel = new QLabel("0:00");
    totalTimeLabel = new QLabel("0:00");
    nowPlayingLabel = new QLabel("Не воспроизводится");
    
    QString playerButtonStyle = 
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 12px;"
        "   border-radius: 4px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #555555;"
        "}";
    
    playButton->setStyleSheet(playerButtonStyle);
    pauseButton->setStyleSheet(playerButtonStyle);
    stopButton->setStyleSheet(playerButtonStyle);
    
    currentTimeLabel->setStyleSheet("QLabel { color: white; font-size: 12px; }");
    totalTimeLabel->setStyleSheet("QLabel { color: white; font-size: 12px; }");
    nowPlayingLabel->setStyleSheet("QLabel { color: white; font-size: 14px; font-weight: bold; }");
    
    playerLayout->addWidget(playButton);
    playerLayout->addWidget(pauseButton);
    playerLayout->addWidget(stopButton);
    playerLayout->addSpacing(20);
    playerLayout->addWidget(currentTimeLabel);
    playerLayout->addWidget(new QLabel(" / "));
    playerLayout->addWidget(totalTimeLabel);
    playerLayout->addSpacing(20);
    playerLayout->addWidget(nowPlayingLabel, 1);
    
    mainLayout->addWidget(playerControls);
    
    setCentralWidget(centralWidget);
}

void MainWindow::createSidebar()
{
    sidebar = new QWidget();
    sidebar->setFixedWidth(280);
    sidebar->setStyleSheet(
        "QWidget {"
        "   background-color: #000000;"
        "   border: none;"
        "}"
    );
    
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setSpacing(0);
    sidebarLayout->setContentsMargins(0, 20, 0, 20);
    
    QLabel *logoLabel = new QLabel("SoundSpace");
    logoLabel->setStyleSheet(
        "QLabel {"
        "   color: #8A2BE2;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   padding: 20px;"
        "   background-color: #000000;"
        "}"
    );
    logoLabel->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(logoLabel);
    
    QWidget *avatarContainer = new QWidget();
    avatarContainer->setStyleSheet("QWidget { background-color: transparent; }");
    QHBoxLayout *avatarLayout = new QHBoxLayout(avatarContainer);
    avatarLayout->setAlignment(Qt::AlignCenter);
    avatarLayout->setContentsMargins(0, 20, 0, 20);
    
    avatarButton = new QPushButton();
    avatarButton->setFixedSize(80, 80);
    avatarButton->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   border: 3px solid #8A2BE2;"
        "   border-radius: 40px;"
        "}"
        "QPushButton:hover {"
        "   border: 3px solid #9B4BFF;"
        "   background-color: rgba(138, 43, 226, 0.3);"
        "}"
    );
    avatarButton->setCursor(Qt::PointingHandCursor);
    
    avatarLayout->addWidget(avatarButton);
    sidebarLayout->addWidget(avatarContainer);
    
    usernameLabel = new QLabel(currentUsername);
    usernameLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   padding: 10px;"
        "   background-color: transparent;"
        "}"
    );
    usernameLabel->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(usernameLabel);
    
    sidebarLayout->addSpacing(20);
    
    QString navButtonStyle = 
        "QPushButton {"
        "   background-color: transparent;"
        "   color: #CCCCCC;"
        "   border: none;"
        "   padding: 15px 25px;"
        "   text-align: left;"
        "   font-size: 14px;"
        "   border-radius: 0px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1A1A1A;"
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #8A2BE2;"
        "}";
    
    myMusicBtn = new QPushButton("Моя музыка");
    profileBtn = new QPushButton("Профиль");
    musicSearchBtn = new QPushButton("Поиск музыки");
    playlistBtn = new QPushButton("Мои плейлисты");
    friendsBtn = new QPushButton("Друзья");
    messagesBtn = new QPushButton("Сообщения");
    notificationsBtn = new QPushButton("Уведомления");
    
    myMusicBtn->setStyleSheet(navButtonStyle);
    profileBtn->setStyleSheet(navButtonStyle);
    musicSearchBtn->setStyleSheet(navButtonStyle);
    playlistBtn->setStyleSheet(navButtonStyle);
    friendsBtn->setStyleSheet(navButtonStyle);
    messagesBtn->setStyleSheet(navButtonStyle);
    notificationsBtn->setStyleSheet(navButtonStyle);
    
    sidebarLayout->addWidget(myMusicBtn);
    sidebarLayout->addWidget(profileBtn);
    sidebarLayout->addWidget(musicSearchBtn);
    sidebarLayout->addWidget(playlistBtn);
    sidebarLayout->addWidget(friendsBtn);
    sidebarLayout->addWidget(messagesBtn);
    sidebarLayout->addWidget(notificationsBtn);
    
    sidebarLayout->addStretch();
    
    QPushButton *logoutBtn = new QPushButton("Выйти");
    logoutBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px;"
        "   margin: 20px;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );
    connect(logoutBtn, &QPushButton::clicked, this, &QMainWindow::close);
    sidebarLayout->addWidget(logoutBtn);
}

void MainWindow::createPages()
{
    mainStack = new QStackedWidget();
    mainStack->setStyleSheet("QStackedWidget { background-color: #121212; }");
    
    auto createSimplePage = [](const QString& title) -> QWidget* {
        QWidget *page = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(30, 30, 30, 30);
        
        QLabel *label = new QLabel(title);
        label->setStyleSheet("QLabel { color: white; font-size: 24px; font-weight: bold; }");
        label->setAlignment(Qt::AlignCenter);
        
        layout->addWidget(label);
        layout->addStretch();
        return page;
    };
    
    myMusicPage = createSimplePage("Моя музыка");
    profilePage = createSimplePage("Профиль\n\nПользователь: " + currentUsername);
    createMessagesPage();
    createFriendsPage();
    notificationsPage = createSimplePage("Уведомления");
    playlistPage = createSimplePage("Мои плейлисты");
    createMusicPage();
    
    mainStack->addWidget(myMusicPage);
    mainStack->addWidget(profilePage);
    mainStack->addWidget(messagesPage);
    mainStack->addWidget(friendsPage);
    mainStack->addWidget(notificationsPage);
    mainStack->addWidget(playlistPage);
    mainStack->addWidget(musicPage);
}

void MainWindow::createMessagesPage()
{
    messagesPage = new QWidget();
    auto *root = new QVBoxLayout(messagesPage);
    root->setContentsMargins(30,30,30,30);
    root->setSpacing(16);

    QLabel *title = new QLabel("Сообщения");
    title->setStyleSheet("QLabel{ color: white; font-size: 24px; font-weight: bold; }");
    root->addWidget(title, 0, Qt::AlignLeft);

    messagesSplitter = new QSplitter(Qt::Horizontal);
    messagesSplitter->setHandleWidth(1);

    // LEFT — список диалогов
    QWidget *left = new QWidget();
    auto *leftLay = new QVBoxLayout(left);
    leftLay->setContentsMargins(0,0,0,0);
    leftLay->setSpacing(10);

    QLabel *dialogsTitle = new QLabel("Диалоги");
    dialogsTitle->setStyleSheet("QLabel{ color:#9B9B9B; font-size:14px; }");
    leftLay->addWidget(dialogsTitle);

    dialogsList = new QListWidget();
    dialogsList->setStyleSheet(
        "QListWidget{background:#1A1A1A;border:1px solid #2A2A2A;border-radius:10px;color:white;}"
        "QListWidget::item{margin:6px;padding:10px;border-radius:10px;}"
        "QListWidget::item:selected{background:#2A2A2A;}"
    );
    leftLay->addWidget(dialogsList, 1);

    // RIGHT — чат
    QWidget *right = new QWidget();
    auto *rightLay = new QVBoxLayout(right);
    rightLay->setContentsMargins(0,0,0,0);
    rightLay->setSpacing(10);

    QWidget *header = new QWidget();
    auto *headerLay = new QHBoxLayout(header);
    headerLay->setContentsMargins(0,0,0,0);
    headerLay->setSpacing(10);

    chatHeaderAvatar = new QLabel();
    chatHeaderAvatar->setFixedSize(36,36);
    chatHeaderAvatar->setStyleSheet("QLabel{background:#8A2BE2;border-radius:18px;}");

    chatHeaderTitle = new QLabel("Выберите диалог");
    chatHeaderTitle->setStyleSheet("QLabel{color:white;font-size:16px;font-weight:700;}");

    headerLay->addWidget(chatHeaderAvatar);
    headerLay->addWidget(chatHeaderTitle);
    headerLay->addStretch(1);

    conversationList = new QListWidget();
    conversationList->setStyleSheet(
        "QListWidget{background:#1A1A1A;border:1px solid #2A2A2A;border-radius:10px;color:white;}"
        "QListWidget::item{margin:6px;border:none;}"
    );

    QWidget *inputBar = new QWidget();
    auto *inputLay = new QHBoxLayout(inputBar);
    inputLay->setContentsMargins(0,0,0,0);
    inputLay->setSpacing(10);

    messageInput = new QLineEdit();
    messageInput->setPlaceholderText("Введите сообщение…");
    messageInput->setStyleSheet(
        "QLineEdit{background:#1A1A1A;border:2px solid #2A2A2A;border-radius:12px;padding:12px 14px;color:white;}"
        "QLineEdit:focus{border:2px solid #8A2BE2;}"
    );

    sendMessageButton = new QPushButton("📤");
    sendMessageButton->setFixedSize(46,46);
    sendMessageButton->setStyleSheet(
        "QPushButton{background:#8A2BE2;color:#fff;border:none;border-radius:10px;font-size:18px;}"
        "QPushButton:hover{background:#7B1FA2;}"
    );

    inputLay->addWidget(messageInput, 1);
    inputLay->addWidget(sendMessageButton);

    rightLay->addWidget(header);
    rightLay->addWidget(conversationList, 1);
    rightLay->addWidget(inputBar);

    messagesSplitter->addWidget(left);
    messagesSplitter->addWidget(right);
    messagesSplitter->setStretchFactor(0, 0);
    messagesSplitter->setStretchFactor(1, 1);
    messagesSplitter->setSizes({320, 900});

    root->addWidget(messagesSplitter, 1);
}

void MainWindow::createFriendsPage()
{
    friendsPage = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(friendsPage);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);
    
    QLabel *titleLabel = new QLabel("Друзья");
    titleLabel->setStyleSheet("QLabel { color: white; font-size: 28px; font-weight: bold; }");
    mainLayout->addWidget(titleLabel);
    
    // Поиск пользователей
    QHBoxLayout *searchLayout = new QHBoxLayout();
    friendSearchInput = new QLineEdit();
    friendSearchInput->setPlaceholderText("Поиск пользователей...");
    friendSearchInput->setStyleSheet(
        "QLineEdit {"
        "   background-color: #1A1A1A;"
        "   border: 2px solid #2A2A2A;"
        "   border-radius: 25px;"
        "   padding: 12px 20px;"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #8A2BE2;"
        "}"
    );
    
    friendSearchButton = new QPushButton("Поиск");
    friendSearchButton->setFixedWidth(120);
    friendSearchButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px;"
        "   border-radius: 25px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );
    
    searchLayout->addWidget(friendSearchInput);
    searchLayout->addWidget(friendSearchButton);
    mainLayout->addLayout(searchLayout);
    
    // Табы для друзей, запросов и поиска
    friendsTabWidget = new QTabWidget();
    friendsTabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "   border: 1px solid #2A2A2A;"
        "   background-color: #1A1A1A;"
        "}"
        "QTabBar::tab {"
        "   background-color: #2A2A2A;"
        "   color: white;"
        "   padding: 8px 16px;"
        "   border: none;"
        "}"
        "QTabBar::tab:selected {"
        "   background-color: #8A2BE2;"
        "}"
        "QTabBar::tab:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );
    
    // Вкладка друзей
    QWidget *friendsTab = new QWidget();
    QVBoxLayout *friendsLayout = new QVBoxLayout(friendsTab);
    friendsList = new QListWidget();
    friendsList->setStyleSheet(
        "QListWidget {"
        "   background-color: #1A1A1A;"
        "   border: 2px solid #2A2A2A;"
        "   border-radius: 10px;"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QListWidget::item {"
        "   background-color: #1A1A1A;"
        "   border-bottom: 1px solid #2A2A2A;"
        "   padding: 15px;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #2A2A2A;"
        "}"
    );
    friendsLayout->addWidget(friendsList);
    
    // Вкладка запросов в друзья
    QWidget *requestsTab = new QWidget();
    QVBoxLayout *requestsLayout = new QVBoxLayout(requestsTab);
    friendRequestsList = new QListWidget();
    friendRequestsList->setStyleSheet(friendsList->styleSheet());
    requestsLayout->addWidget(friendRequestsList);
    
    // Вкладка поиска
    QWidget *searchTab = new QWidget();
    QVBoxLayout *searchTabLayout = new QVBoxLayout(searchTab);
    usersList = new QListWidget();
    usersList->setStyleSheet(friendsList->styleSheet());
    searchTabLayout->addWidget(usersList);
    
    friendsTabWidget->addTab(friendsTab, "Друзья");
    friendsTabWidget->addTab(requestsTab, "Запросы");
    friendsTabWidget->addTab(searchTab, "Результаты поиска");
    
    mainLayout->addWidget(friendsTabWidget, 1);
}

void MainWindow::createMusicPage()
{
    musicPage = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(musicPage);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);
    
    QLabel *titleLabel = new QLabel("Поиск музыки");
    titleLabel->setStyleSheet("QLabel { color: white; font-size: 28px; font-weight: bold; }");
    mainLayout->addWidget(titleLabel);
    
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Введите название трека или исполнителя...");
    searchInput->setStyleSheet(
        "QLineEdit {"
        "   background-color: #1A1A1A;"
        "   border: 2px solid #2A2A2A;"
        "   border-radius: 25px;"
        "   padding: 12px 20px;"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #8A2BE2;"
        "}"
    );
    
    searchButton = new QPushButton("Поиск");
    searchButton->setFixedWidth(120);
    searchButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px;"
        "   border-radius: 25px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );
    
    topTracksButton = new QPushButton("Популярные треки");
    topTracksButton->setFixedWidth(150);
    topTracksButton->setStyleSheet(searchButton->styleSheet());
    
    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(topTracksButton);
    mainLayout->addLayout(searchLayout);
    
    QHBoxLayout *contentLayout = new QHBoxLayout();
    
    tracksList = new QListWidget();
    tracksList->setStyleSheet(
        "QListWidget {"
        "   background-color: #1A1A1A;"
        "   border: 2px solid #2A2A2A;"
        "   border-radius: 10px;"
        "   color: white;"
        "   font-size: 14px;"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   background-color: #1A1A1A;"
        "   border-bottom: 1px solid #2A2A2A;"
        "   padding: 15px;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #2A2A2A;"
        "}"
    );
    
    QWidget *trackInfoWidget = new QWidget();
    QVBoxLayout *trackInfoLayout = new QVBoxLayout(trackInfoWidget);
    
    trackInfo = new QTextEdit();
    trackInfo->setReadOnly(true);
    trackInfo->setStyleSheet(
        "QTextEdit {"
        "   background-color: #1A1A1A;"
        "   border: 2px solid #2A2A2A;"
        "   border-radius: 10px;"
        "   color: white;"
        "   font-size: 14px;"
        "   padding: 20px;"
        "}"
    );
    trackInfo->setPlaceholderText("Выберите трек для просмотра информации...");
    
    QPushButton *addToLibraryBtn = new QPushButton("Добавить в библиотеку");
    addToLibraryBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   margin-top: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );
    addToLibraryBtn->setVisible(false);
    
    trackInfoLayout->addWidget(trackInfo);
    trackInfoLayout->addWidget(addToLibraryBtn);
    
    contentLayout->addWidget(tracksList, 2);
    contentLayout->addWidget(trackInfoWidget, 1);
    mainLayout->addLayout(contentLayout, 1);
}

void MainWindow::setupConnections()
{
    // Навигация
    connect(profileBtn, &QPushButton::clicked, this, &MainWindow::showProfilePage);
    connect(messagesBtn, &QPushButton::clicked, this, &MainWindow::showMessagesPage);
    connect(friendsBtn, &QPushButton::clicked, this, &MainWindow::showFriendsPage);
    connect(notificationsBtn, &QPushButton::clicked, this, &MainWindow::showNotificationsPage);
    connect(playlistBtn, &QPushButton::clicked, this, &MainWindow::showPlaylistPage);
    connect(musicSearchBtn, &QPushButton::clicked, this, [this]() { 
        mainStack->setCurrentIndex(MUSIC_PAGE); 
    });
    
    // Аватар
    connect(avatarButton, &QPushButton::clicked, this, &MainWindow::changeAvatar);
    
    // Музыка
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchMusic);
    connect(topTracksButton, &QPushButton::clicked, this, &MainWindow::showTopTracks);
    connect(searchInput, &QLineEdit::returnPressed, this, &MainWindow::searchMusic);
    connect(apiManager, &MusicAPIManager::tracksFound, this, &MainWindow::onTracksFound);
    connect(apiManager, &MusicAPIManager::networkError, this, &MainWindow::onNetworkError);
    
    // Управление плеером
    connect(playButton, &QPushButton::clicked, this, &MainWindow::playSelectedTrack);
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::pausePlayback);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopPlayback);
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &MainWindow::onPlaybackStateChanged);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
    
    // Друзья
    connect(friendSearchButton, &QPushButton::clicked, this, &MainWindow::searchUsers);
    connect(friendSearchInput, &QLineEdit::returnPressed, this, &MainWindow::searchUsers);
    
    // Сообщения
    connect(messagesBtn, &QPushButton::clicked, this, [this]() {
        mainStack->setCurrentIndex(MESSAGES_PAGE);
        refreshDialogs();
    });
    
    if (dialogsList) {
        connect(dialogsList, &QListWidget::itemClicked, this, [this](QListWidgetItem *it){
            int uid = it->data(Qt::UserRole).toInt();
            openChat(uid);
        });
    }
    
    if (messageInput) {
        connect(messageInput, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
    }
    
    if (sendMessageButton) {
        connect(sendMessageButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    }

    connect(&MessagesManager::instance(), &MessagesManager::newMessageReceived,
            this, &MainWindow::onNewMessageReceived);
}

// Навигация
void MainWindow::showProfilePage() { mainStack->setCurrentIndex(PROFILE_PAGE); }

void MainWindow::showMessagesPage() {
    mainStack->setCurrentIndex(MESSAGES_PAGE);
    refreshDialogs();
}

void MainWindow::showFriendsPage() { 
    mainStack->setCurrentIndex(FRIENDS_PAGE); 
    refreshFriendsData();
}

void MainWindow::showNotificationsPage() { mainStack->setCurrentIndex(NOTIFICATIONS_PAGE); }
void MainWindow::showPlaylistPage() { mainStack->setCurrentIndex(PLAYLIST_PAGE); }

// Музыка
void MainWindow::searchMusic()
{
    QString query = searchInput->text().trimmed();
    if (query.isEmpty()) {
        trackInfo->setPlainText("Введите поисковый запрос!");
        return;
    }
    
    tracksList->clear();
    trackInfo->setPlainText("Поиск треков...\nЗапрос: " + query);
    
    apiManager->searchTracks(query, currentUserId);
}

void MainWindow::showTopTracks()
{
    tracksList->clear();
    trackInfo->setPlainText("Загрузка популярных треков...");
    apiManager->getTopTracks(currentUserId);
}

void MainWindow::onTracksFound(const QVariantList& tracks)
{
    tracksList->clear();
    
    if (tracks.isEmpty()) {
        trackInfo->setPlainText("Треки не найдены!");
        return;
    }
    
    for (const QVariant &trackVar : tracks) {
        QVariantMap track = trackVar.toMap();
        QString title = track["title"].toString();
        QString artist = track["artist"].toString();
        QString listeners = track.value("listens", "N/A").toString();
        
        QString itemText = QString("%1 - %2").arg(title).arg(artist);
        if (listeners != "N/A") {
            itemText += QString(" [%1]").arg(listeners);
        }
        
        QListWidgetItem *item = new QListWidgetItem(itemText, tracksList);
        
        QString trackInfoText = QString(
            "Трек: %1\n"
            "Исполнитель: %2\n"
            "Прослушиваний: %3\n"
        ).arg(title).arg(artist).arg(listeners);
        
        item->setData(Qt::UserRole, trackInfoText);
        item->setData(Qt::UserRole + 1, track);
    }
    
    trackInfo->setPlainText(QString("Найдено треков: %1\n\nВыберите трек для подробной информации.").arg(tracks.size()));
}

void MainWindow::onNetworkError(const QString& error)
{
    trackInfo->setPlainText("Ошибка сети: " + error);
}

void MainWindow::addSelectedTrackToPlaylist()
{
    QListWidgetItem *currentItem = tracksList->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, "Информация", "Выберите трек для добавления в плейлист");
        return;
    }
    
    QVariant trackData = currentItem->data(Qt::UserRole + 1);
    if (trackData.isValid()) {
        QVariantMap track = trackData.toMap();
        
        bool success = DatabaseManager::instance().addTrackFromAPI(
            currentUserId,
            track["title"].toString(),
            track["artist"].toString(),
            "", 0, "", track["coverUrl"].toString()
        );
        
        if (success) {
            QMessageBox::information(this, "Успех", "Трек добавлен в библиотеку!");
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось добавить трек в библиотеку");
        }
    }
}

void MainWindow::playSelectedTrack()
{
    if (mediaPlayer->state() == QMediaPlayer::PlayingState) {
        mediaPlayer->pause();
    } else {
        if (!currentAudioUrl.isEmpty()) {
            mediaPlayer->play();
        } else {
            QMessageBox::information(this, "Информация", "Сначала выберите трек для воспроизведения");
        }
    }
}

void MainWindow::pausePlayback()
{
    mediaPlayer->pause();
}

void MainWindow::stopPlayback()
{
    mediaPlayer->stop();
}

void MainWindow::onPlaybackStateChanged(QMediaPlayer::State state)
{
    switch (state) {
    case QMediaPlayer::PlayingState:
        nowPlayingLabel->setText("Воспроизведение...");
        break;
    case QMediaPlayer::PausedState:
        nowPlayingLabel->setText("Пауза");
        break;
    case QMediaPlayer::StoppedState:
        nowPlayingLabel->setText("Остановлено");
        break;
    }
}

void MainWindow::onPositionChanged(qint64 position)
{
    currentTimeLabel->setText(formatTime(position));
}

void MainWindow::onDurationChanged(qint64 duration)
{
    totalTimeLabel->setText(formatTime(duration));
}

// Друзья
void MainWindow::searchUsers()
{
    QString query = friendSearchInput->text().trimmed();
    if (query.isEmpty()) {
        QMessageBox::information(this, "Поиск", "Введите имя пользователя для поиска");
        return;
    }
    
    usersList->clear();
    
    QList<QVariantMap> users = FriendsManager::instance().searchUsers(query, currentUserId);
    onUsersFound(users);
}

void MainWindow::onUsersFound(const QList<QVariantMap>& users)
{
    usersList->clear();
    
    if (users.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem("Пользователи не найдены");
        usersList->addItem(item);
        return;
    }
    
    for (const QVariantMap &user : users) {
        QString username = user["username"].toString();
        QString status = user["friend_status"].toString();
        
        QString itemText = username;
        if (status != "none") {
            itemText += " (" + status + ")";
        }
        
        QListWidgetItem *item = new QListWidgetItem(itemText, usersList);
        item->setData(Qt::UserRole, user["id"].toInt());
    }
    
    friendsTabWidget->setCurrentIndex(2); // Переключаем на вкладку результатов поиска
}

void MainWindow::sendFriendRequest(int userId)
{
    if (FriendsManager::instance().sendFriendRequest(currentUserId, userId)) {
        QMessageBox::information(this, "Успех", "Запрос в друзья отправлен");
        refreshFriendsData();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось отправить запрос в друзья");
    }
}

void MainWindow::acceptFriendRequest(int userId)
{
    if (FriendsManager::instance().acceptFriendRequest(userId, currentUserId)) {
        QMessageBox::information(this, "Успех", "Запрос в друзья принят");
        refreshFriendsData();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось принять запрос в друзья");
    }
}

void MainWindow::rejectFriendRequest(int userId)
{
    if (FriendsManager::instance().rejectFriendRequest(userId, currentUserId)) {
        QMessageBox::information(this, "Успех", "Запрос в друзья отклонен");
        refreshFriendsData();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось отклонить запрос в друзья");
    }
}

void MainWindow::removeFriend(int friendId)
{
    if (FriendsManager::instance().removeFriend(currentUserId, friendId)) {
        QMessageBox::information(this, "Успех", "Пользователь удален из друзей");
        refreshFriendsData();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить пользователя из друзей");
    }
}

void MainWindow::loadFriends()
{
    friendsList->clear();
    
    QList<QVariantMap> friends = FriendsManager::instance().getFriends(currentUserId);
    
    for (const QVariantMap &friendData : friends) {
        QString username = friendData["username"].toString();
        QListWidgetItem *item = new QListWidgetItem(username, friendsList);
        item->setData(Qt::UserRole, friendData["id"].toInt());
    }
}

void MainWindow::loadFriendRequests()
{
    friendRequestsList->clear();
    
    QList<QVariantMap> requests = FriendsManager::instance().getPendingRequests(currentUserId);
    
    for (const QVariantMap &request : requests) {
        QString username = request["username"].toString();
        QListWidgetItem *item = new QListWidgetItem(username, friendRequestsList);
        item->setData(Qt::UserRole, request["id"].toInt());
    }
}

void MainWindow::refreshFriendsData()
{
    loadFriends();
    loadFriendRequests();
}

// Сообщения
void MainWindow::refreshDialogs()
{
    if (!dialogsList) return;
    dialogsList->clear();

    const QList<QVariantMap> dialogs = MessagesManager::instance().getDialogs(currentUserId);

    for (const QVariantMap &d : dialogs) {
        const int uid = d.value("user_id").toInt();
        const QString username = d.value("username").toString();
        const QString preview  = d.value("last_message").toString();
        const int unread       = d.value("unread_count").toInt();

        QWidget *w = new QWidget();
        auto *lay = new QHBoxLayout(w);
        lay->setContentsMargins(8,6,8,6);
        lay->setSpacing(10);

        QLabel *ava = new QLabel();
        QPixmap px = makeAvatar(uid, username, 44);
        ava->setPixmap(px);
        ava->setFixedSize(44,44);
        ava->setStyleSheet("QLabel{border-radius:22px;}");

        auto *vl = new QVBoxLayout();
        vl->setContentsMargins(0,0,0,0);
        vl->setSpacing(2);
        QLabel *name = new QLabel(username);
        name->setStyleSheet("QLabel{color:white;font-weight:700;}");
        QLabel *msg  = new QLabel(preview);
        msg->setStyleSheet("QLabel{color:#BDBDBD;font-size:12px;}");
        msg->setWordWrap(true);
        vl->addWidget(name);
        vl->addWidget(msg);

        lay->addWidget(ava);
        lay->addLayout(vl, 1);

        if (unread > 0) {
            QLabel *badge = new QLabel(QString::number(unread));
            badge->setStyleSheet("QLabel{background:#e53935;color:#fff;border-radius:10px;padding:2px 8px;}");
            lay->addWidget(badge, 0, Qt::AlignRight | Qt::AlignVCenter);
        }

        auto *item = new QListWidgetItem(dialogsList);
        item->setData(Qt::UserRole, uid);
        item->setSizeHint(w->sizeHint());
        dialogsList->setItemWidget(item, w);
    }
}

void MainWindow::openChat(int friendId)
{
    currentChatUserId = friendId;
    if (!conversationList) return;
    conversationList->clear();

    QString friendName = "Unknown";
    const auto friends = FriendsManager::instance().getFriends(currentUserId);
    for (const auto &f : friends) {
        if (f.value("id").toInt() == friendId) {
            friendName = f.value("username").toString();
            chatHeaderAvatar->setPixmap(makeAvatar(friendId, friendName, 36));
            break;
        }
    }
    chatHeaderTitle->setText("Диалог с " + friendName);

    const QList<QVariantMap> messages =
        MessagesManager::instance().getConversation(currentUserId, friendId, 200);

    if (messages.isEmpty()) {
        auto *item = new QListWidgetItem("💭 Нет сообщений. Напишите первым!", conversationList);
        item->setTextAlignment(Qt::AlignCenter);
        item->setForeground(QColor("#888"));
    } else {
        for (const auto &m : messages)
            addMessageBubble(m, friendName);
    }

    conversationList->scrollToBottom();
    MessagesManager::instance().markMessagesAsRead(currentUserId, friendId);
}

void MainWindow::sendMessage()
{
    if (currentChatUserId == -1 || !messageInput) return;

    const QString text = messageInput->text().trimmed();
    if (text.isEmpty()) return;

    if (MessagesManager::instance().sendMessage(currentUserId, currentChatUserId, text)) {
        messageInput->clear();

        QVariantMap msg;
        msg["text"] = text;
        msg["is_own"] = true;
        msg["timestamp"] = QDateTime::currentDateTime();
        addMessageBubble(msg, "");
        if (conversationList) conversationList->scrollToBottom();

        refreshDialogs();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось отправить сообщение");
    }
}

void MainWindow::onNewMessageReceived(int fromUserId, const QString& username, const QString& message, const QDateTime& timestamp)
{
    Q_UNUSED(username);
    Q_UNUSED(message);
    Q_UNUSED(timestamp);
    
    refreshDialogs();
    if (fromUserId == currentChatUserId && mainStack->currentIndex() == MESSAGES_PAGE)
        openChat(currentChatUserId);
}

// Аватар
void MainWindow::changeAvatar()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Выберите изображение для аватарки",
        QDir::homePath(),
        "Images (*.png *.jpg *.jpeg *.bmp *.gif)"
    );
    
    if (!filePath.isEmpty()) {
        QPixmap newAvatar(filePath);
        if (newAvatar.isNull()) {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить изображение");
            return;
        }
        
        saveAvatar(newAvatar);
        setAvatarPixmap(newAvatar);
        
        QMessageBox::information(this, "Успех", "Аватарка успешно изменена!");
    }
}

void MainWindow::onAvatarButtonEnter()
{
    // Реализация при необходимости
}

void MainWindow::onAvatarButtonLeave()
{
    // Реализация при необходимости
}

// Вспомогательные методы
void MainWindow::enableSearchControls(bool enable)
{
    // Реализация при необходимости
}

int MainWindow::bubbleMaxWidth() const
{
    if (!conversationList) return 600;
    return int(conversationList->viewport()->width() * 0.68);
}

void MainWindow::addMessageBubble(const QVariantMap& message, const QString& friendName)
{
    if (!conversationList) return;

    const bool isOwn = message.value("is_own").toBool();
    const QString text = message.value("text").toString();
    const QString sender = isOwn ? "Вы" : friendName;
    const QString ts = message.value("timestamp").toDateTime().toString("HH:mm");

    QWidget *row = new QWidget();
    auto *rowLay = new QHBoxLayout(row);
    rowLay->setContentsMargins(10,6,10,6);
    rowLay->setSpacing(6);

    QWidget *stack = new QWidget();
    auto *stackLay = new QVBoxLayout(stack);
    stackLay->setContentsMargins(0,0,0,0);
    stackLay->setSpacing(3);

    QLabel *bubble = new QLabel(text);
    bubble->setWordWrap(true);
    bubble->setMaximumWidth(bubbleMaxWidth());
    bubble->setTextInteractionFlags(Qt::TextSelectableByMouse);
    bubble->setStyleSheet(isOwn
        ? "QLabel{background:#8A2BE2;color:#fff;padding:12px 14px;border-radius:18px;"
          "border:1px solid #6A1B9A;font-size:14px;}"
        : "QLabel{background:#232323;color:#EEE;padding:12px 14px;border-radius:18px;"
          "border:1px solid #8A2BE2;font-size:14px;}"
    );

    QLabel *meta = new QLabel(sender + " · " + ts);
    meta->setStyleSheet("QLabel{font-size:11px;color:#A0A0A0;}");

    stackLay->addWidget(bubble, 0, isOwn ? Qt::AlignRight : Qt::AlignLeft);
    stackLay->addWidget(meta,   0, isOwn ? Qt::AlignRight : Qt::AlignLeft);

    if (isOwn) {
        rowLay->addStretch(1);
        rowLay->addWidget(stack, 0, Qt::AlignRight);
    } else {
        rowLay->addWidget(stack, 0, Qt::AlignLeft);
        rowLay->addStretch(1);
    }

    auto *item = new QListWidgetItem(conversationList);
    item->setBackground(Qt::transparent);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setSizeHint(row->sizeHint());
    conversationList->setItemWidget(item, row);
}

void MainWindow::loadUserAvatar()
{
    QString userAvatarPath = QString("avatars/user_%1.png").arg(currentUserId);
    QFile avatarFile(userAvatarPath);
    
    if (avatarFile.exists()) {
        QPixmap avatarPixmap(userAvatarPath);
        if (!avatarPixmap.isNull()) {
            setAvatarPixmap(avatarPixmap);
            return;
        }
    }
    
    setDefaultAvatar();
}

void MainWindow::setupAvatar()
{
    setDefaultAvatar();
}

void MainWindow::setAvatarPixmap(const QPixmap& pixmap)
{
    QPixmap circularPixmap(80, 80);
    circularPixmap.fill(Qt::transparent);
    
    QPainter painter(&circularPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    
    QPainterPath path;
    path.addEllipse(0, 0, 80, 80);
    painter.setClipPath(path);
    
    QPixmap scaledPixmap = pixmap.scaled(80, 80, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    
    int x = (80 - scaledPixmap.width()) / 2;
    int y = (80 - scaledPixmap.height()) / 2;
    painter.drawPixmap(x, y, scaledPixmap);
    
    avatarButton->setIcon(QIcon(circularPixmap));
    avatarButton->setIconSize(QSize(80, 80));
    avatarButton->setText("");
}

void MainWindow::setDefaultAvatar()
{
    QPixmap defaultPixmap(80, 80);
    defaultPixmap.fill(Qt::transparent);
    
    QPainter painter(&defaultPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    QRadialGradient gradient(40, 40, 40);
    gradient.setColorAt(0, QColor("#9B4BFF"));
    gradient.setColorAt(1, QColor("#7B1FA2"));
    
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 80, 80);
    
    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Arial", 20, QFont::Bold));
    
    QString initials = currentUsername.left(2).toUpper();
    QRect textRect(0, 0, 80, 80);
    painter.drawText(textRect, Qt::AlignCenter, initials);
    
    avatarButton->setIcon(QIcon(defaultPixmap));
    avatarButton->setIconSize(QSize(80, 80));
    avatarButton->setText("");
}

void MainWindow::saveAvatar(const QPixmap& avatar)
{
    QDir().mkpath("avatars");
    
    QString filePath = QString("avatars/user_%1.png").arg(currentUserId);
    avatar.save(filePath, "PNG");
}

QPixmap MainWindow::makeAvatar(int userId, const QString &username, int size)
{
    QString path = QString("avatars/user_%1.png").arg(userId);
    QPixmap px(path);
    if (!px.isNull())
        return px.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    QPixmap avatar(size, size);
    avatar.fill(Qt::transparent);
    QPainter p(&avatar);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor("#8A2BE2"));
    p.setPen(Qt::NoPen);
    p.drawEllipse(0,0,size,size);
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", size/2, QFont::Bold));
    p.drawText(QRect(0,0,size,size), Qt::AlignCenter, username.left(1).toUpper());
    return avatar;
}

QString MainWindow::formatTime(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    
    return QString("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

// Моя музыка
void MainWindow::loadUserTracks()
{
    // Реализация при необходимости
}

void MainWindow::onPlaylistItemClicked(QListWidgetItem *item)
{
    // Реализация при необходимости
}

void MainWindow::onTrackDeleteRequested(int trackId)
{
    // Реализация при необходимости
}

void MainWindow::onAddToPlaylistRequested(int trackId)
{
    // Реализация при необходимости
}

void MainWindow::playTrack(int trackId)
{
    // Реализация при необходимости
}

void MainWindow::playNextTrack()
{
    // Реализация при необходимости
}

void MainWindow::playPrevTrack()
{
    // Реализация при необходимости
}

void MainWindow::onTrackFinished()
{
    // Реализация при необходимости
}

void MainWindow::deleteAllTracks()
{
    // Реализация при необходимости
}