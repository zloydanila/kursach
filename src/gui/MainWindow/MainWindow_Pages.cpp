#include "gui/MainWindow/MainWindow.h"
#include "audio/AudioPlayer.h"

#include "gui/MainWindow/pages/FriendsPage.h"
#include "gui/MainWindow/pages/MessagesPage.h"
#include "gui/MainWindow/pages/MyMusicPage.h"
#include "gui/MainWindow/pages/SearchMusicPage.h"

#include <QStackedWidget>
#include <QPushButton>

void MainWindow::createPages()
{
    mainStack = new QStackedWidget();
    mainStack->setStyleSheet("QStackedWidget { background: #0F0F14; }");

    auto *my = new MyMusicPage(currentUserId, this);
    auto *search = new SearchMusicPage(currentUserId, audioPlayer, this);
    auto *friends = new FriendsPage(currentUserId, this);
    auto *messages = new MessagesPage(currentUserId, this);

    myMusicPage = my;
    musicPage = search;
    friendsPage = friends;
    messagesPage = messages;

    profilePage = nullptr;
    notificationsPage = nullptr;
    roomsPage = nullptr;

    mainStack->addWidget(myMusicPage);
    mainStack->addWidget(musicPage);
    mainStack->addWidget(friendsPage);
    mainStack->addWidget(messagesPage);

    connect(friendsPage, &FriendsPage::openChatWithFriend, this, [this](int friendId, const QString &friendName) {
        messagesPage->openChat(friendId, friendName);
        mainStack->setCurrentWidget(messagesPage);
    });

    connect(search, &SearchMusicPage::stationAdded, this, [this]() {
        auto *p = qobject_cast<MyMusicPage*>(myMusicPage);
        if (p) p->reloadStations();
    });

    connect(my, &MyMusicPage::radioPlaylistChanged, this, [this](const QVector<TrackData>& stations) {
        QVector<RadioItem> list;
        list.reserve(stations.size());
        for (const auto& s : stations) {
            RadioItem r;
            r.url = s.streamUrl;
            r.title = s.title;
            r.artist = s.artist;
            list.push_back(r);
        }
        audioPlayer->setRadioPlaylist(list);
    });

    connect(my, &MyMusicPage::currentRadioIndexChanged, this, [this](int idx) {
        audioPlayer->setCurrentRadioIndex(idx);
    });

    connect(my, &MyMusicPage::playRadioRequested, this, [this](const QString& url, const QString& title, const QString& country) {
        audioPlayer->playRadio(url, title, country);
    });

    if (myMusicBtn) connect(myMusicBtn, &QPushButton::clicked, this, &MainWindow::showMyMusicPage);
    if (musicSearchBtn) connect(musicSearchBtn, &QPushButton::clicked, this, &MainWindow::showSearchRadioPage);
    if (friendsBtn) connect(friendsBtn, &QPushButton::clicked, this, &MainWindow::showFriendsPage);
    if (messagesBtn) connect(messagesBtn, &QPushButton::clicked, this, &MainWindow::showMessagesPage);

    mainStack->setCurrentWidget(myMusicPage);
}

QWidget* MainWindow::createSimplePage(const QString&, const QString&)
{
    return new QWidget();
}
