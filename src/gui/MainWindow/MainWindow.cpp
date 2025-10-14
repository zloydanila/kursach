#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

MainWindow::MainWindow(const QString& username, QWidget *parent)
    : QMainWindow(parent)
    , currentUsername(username)
    , sidebar(nullptr)
    , avatarLabel(nullptr)
    , usernameLabel(nullptr)
    , profileBtn(nullptr)
    , messagesBtn(nullptr)
    , friendsBtn(nullptr)
    , notificationsBtn(nullptr)
    , playlistBtn(nullptr)
    , mainStack(nullptr)
    , profilePage(nullptr)
    , messagesPage(nullptr)
    , friendsPage(nullptr)
    , notificationsPage(nullptr)
    , playlistPage(nullptr)
{
    setupUI();
    setupConnections();
    
    setWindowTitle("SoundSpace - Главное окно");
    setMinimumSize(1200, 700);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{

    QWidget *centralWidget = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    

    createSidebar();
    mainLayout->addWidget(sidebar);
    

    createPages();
    mainLayout->addWidget(mainStack);
    
    setCentralWidget(centralWidget);
}

void MainWindow::createSidebar()
{
    sidebar = new QWidget();
    sidebar->setFixedWidth(250);
    sidebar->setStyleSheet("QWidget { background-color: #8A2BE2; }"); // Фиолетовый фон
    
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    
// ZAGLUSHKA
    avatarLabel = new QLabel();
    avatarLabel->setFixedSize(80, 80);
    avatarLabel->setStyleSheet(
        "QLabel {"
        "   background-color: white;"
        "   border-radius: 40px;"
        "   border: 2px solid white;"
        "}"
    );
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setText("👤");
    avatarLabel->setStyleSheet(avatarLabel->styleSheet() + "QLabel { font-size: 30px; }");
    

    usernameLabel = new QLabel(currentUsername);
    usernameLabel->setStyleSheet("QLabel { color: white; font-size: 16px; font-weight: bold; }");
    usernameLabel->setAlignment(Qt::AlignCenter);
    
    profileBtn = new QPushButton("Профиль");
    messagesBtn = new QPushButton("Сообщения");
    friendsBtn = new QPushButton("Друзья");
    notificationsBtn = new QPushButton("Уведомления");
    playlistBtn = new QPushButton("Мой плейлист");
    
 
    QString buttonStyle = 
        "QPushButton {"
        "   background-color: transparent;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px;"
        "   text-align: left;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #6A1B9A;"
        "}";
    
    profileBtn->setStyleSheet(buttonStyle);
    messagesBtn->setStyleSheet(buttonStyle);
    friendsBtn->setStyleSheet(buttonStyle);
    notificationsBtn->setStyleSheet(buttonStyle);
    playlistBtn->setStyleSheet(buttonStyle);
    
   
    sidebarLayout->addWidget(avatarLabel);
    sidebarLayout->addWidget(usernameLabel);
    sidebarLayout->addSpacing(20);
    sidebarLayout->addWidget(profileBtn);
    sidebarLayout->addWidget(messagesBtn);
    sidebarLayout->addWidget(friendsBtn);
    sidebarLayout->addWidget(notificationsBtn);
    sidebarLayout->addWidget(playlistBtn);
    sidebarLayout->addStretch();
}

void MainWindow::createPages()
{
    mainStack = new QStackedWidget();
    

    profilePage = new QWidget();
    QVBoxLayout *profileLayout = new QVBoxLayout(profilePage);
    QLabel *profileLabel = new QLabel("Страница профиля\n\nПользователь: " + currentUsername);
    profileLabel->setAlignment(Qt::AlignCenter);
    profileLabel->setStyleSheet("QLabel { font-size: 18px; color: #333; }");
    profileLayout->addWidget(profileLabel);

    messagesPage = new QWidget();
    QVBoxLayout *messagesLayout = new QVBoxLayout(messagesPage);
    QLabel *messagesLabel = new QLabel("Страница сообщений\n\nЗдесь будут ваши сообщения");
    messagesLabel->setAlignment(Qt::AlignCenter);
    messagesLabel->setStyleSheet("QLabel { font-size: 18px; color: #333; }");
    messagesLayout->addWidget(messagesLabel);
    

    friendsPage = new QWidget();
    QVBoxLayout *friendsLayout = new QVBoxLayout(friendsPage);
    QLabel *friendsLabel = new QLabel("Страница друзей\n\nЗдесь будет список ваших друзей");
    friendsLabel->setAlignment(Qt::AlignCenter);
    friendsLabel->setStyleSheet("QLabel { font-size: 18px; color: #333; }");
    friendsLayout->addWidget(friendsLabel);
    

    notificationsPage = new QWidget();
    QVBoxLayout *notificationsLayout = new QVBoxLayout(notificationsPage);
    QLabel *notificationsLabel = new QLabel("Страница уведомлений\n\nЗдесь будут ваши уведомления");
    notificationsLabel->setAlignment(Qt::AlignCenter);
    notificationsLabel->setStyleSheet("QLabel { font-size: 18px; color: #333; }");
    notificationsLayout->addWidget(notificationsLabel);
    
    playlistPage = new QWidget();
    QVBoxLayout *playlistLayout = new QVBoxLayout(playlistPage);
    QLabel *playlistLabel = new QLabel("Мой плейлист\n\nЗдесь будет ваш плейлист");
    playlistLabel->setAlignment(Qt::AlignCenter);
    playlistLabel->setStyleSheet("QLabel { font-size: 18px; color: #333; }");
    playlistLayout->addWidget(playlistLabel);
    

    mainStack->addWidget(profilePage);
    mainStack->addWidget(messagesPage);
    mainStack->addWidget(friendsPage);
    mainStack->addWidget(notificationsPage);
    mainStack->addWidget(playlistPage);
}

void MainWindow::setupConnections()
{
    connect(profileBtn, &QPushButton::clicked, this, &MainWindow::showProfilePage);
    connect(messagesBtn, &QPushButton::clicked, this, &MainWindow::showMessagesPage);
    connect(friendsBtn, &QPushButton::clicked, this, &MainWindow::showFriendsPage);
    connect(notificationsBtn, &QPushButton::clicked, this, &MainWindow::showNotificationsPage);
    connect(playlistBtn, &QPushButton::clicked, this, &MainWindow::showPlaylistPage);
}

void MainWindow::showProfilePage()
{
    mainStack->setCurrentIndex(PROFILE_PAGE);
}

void MainWindow::showMessagesPage()
{
    mainStack->setCurrentIndex(MESSAGES_PAGE);
}

void MainWindow::showFriendsPage()
{
    mainStack->setCurrentIndex(FRIENDS_PAGE);
}

void MainWindow::showNotificationsPage()
{
    mainStack->setCurrentIndex(NOTIFICATIONS_PAGE);
}

void MainWindow::showPlaylistPage()
{
    mainStack->setCurrentIndex(PLAYLIST_PAGE);
}