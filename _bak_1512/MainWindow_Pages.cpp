#include "gui/MainWindow/MainWindow.h"
#include "gui/MainWindow/pages/FriendsPage.h"
#include "gui/MainWindow/pages/MessagesPage.h"
#include "gui/MainWindow/pages/MyMusicPage.h"
#include "gui/MainWindow/pages/SearchMusicPage.h"
#include "gui/MainWindow/pages/RoomsPage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedWidget>

void MainWindow::createPages()
{
    mainStack = new QStackedWidget();
    mainStack->setStyleSheet("QStackedWidget { background: #0F0F14; }");

    auto *my = new MyMusicPage(currentUserId, this);
    auto *search = new SearchMusicPage(currentUserId, audioPlayer, this);
    auto *friends = new FriendsPage(currentUserId, this);
    auto *messages = new MessagesPage(currentUserId, this);
    auto *rooms = new RoomsPage(currentUserId, this);

    myMusicPage = my;
    musicPage = search;
    friendsPage = friends;
    messagesPage = messages;
    roomsPage = rooms;

    profilePage = createSimplePage("Профиль", "Здесь будет информация профиля");
    notificationsPage = createSimplePage("Уведомления", "Здесь будут уведомления");

    mainStack->addWidget(myMusicPage);
    mainStack->addWidget(profilePage);
    mainStack->addWidget(musicPage);         // Поиск радио
    mainStack->addWidget(friendsPage);       // Друзья
    mainStack->addWidget(messagesPage);      // Сообщения
    mainStack->addWidget(notificationsPage); // Уведомления
    mainStack->addWidget(roomsPage);         // Комнаты

    connect(friendsPage, &FriendsPage::openChatWithFriend, this, [this](int friendId, const QString &friendName) {
        messagesPage->openChat(friendId, friendName);
        mainStack->setCurrentWidget(messagesPage);
    });

    connect(search, &SearchMusicPage::stationAdded, this, &MainWindow::onRadioStationAdded);

    connect(my, &MyMusicPage::playRadioRequested, this, [this](const QString& url, const QString& title, const QString& country) {
        audioPlayer->playRadio(url, title, country);
    });
// Главное: не по индексам, а по конкретным страницам [web:75]
    if (myMusicBtn) connect(myMusicBtn, &QPushButton::clicked, this, &MainWindow::showMyMusicPage);
    if (profileBtn) connect(profileBtn, &QPushButton::clicked, this, &MainWindow::showProfilePage);
    if (musicSearchBtn) connect(musicSearchBtn, &QPushButton::clicked, this, &MainWindow::showSearchRadioPage);
    if (friendsBtn) connect(friendsBtn, &QPushButton::clicked, this, &MainWindow::showFriendsPage);
    if (messagesBtn) connect(messagesBtn, &QPushButton::clicked, this, &MainWindow::showMessagesPage);
    if (notificationsBtn) connect(notificationsBtn, &QPushButton::clicked, this, &MainWindow::showNotificationsPage);
    if (roomsBtn) connect(roomsBtn, &QPushButton::clicked, this, &MainWindow::showRoomsPage);

    mainStack->setCurrentWidget(myMusicPage);
}

QWidget* MainWindow::createSimplePage(const QString &title, const QString &description)
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(40, 40, 40, 40);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("QLabel { color: #FFFFFF; font-size: 32px; font-weight: 800; background: transparent; border: none; }");

    QLabel *descLabel = new QLabel(description);
    descLabel->setStyleSheet("QLabel { color: rgba(255, 255, 255, 0.55); font-size: 16px; background: transparent; border: none; }");

    layout->addStretch();
    layout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    layout->addWidget(descLabel, 0, Qt::AlignHCenter);
    layout->addStretch();
    return page;
}
