#include "gui/MainWindow/MainWindow.h"
#include "gui/MainWindow/pages/FriendsPage.h"
#include "gui/MainWindow/pages/MessagesPage.h"
#include "gui/MainWindow/pages/MyMusicPage.h"
#include "gui/MainWindow/pages/SearchMusicPage.h"
#include "gui/MainWindow/pages/RoomsPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QDebug>

void MainWindow::createPages()
{
    mainStack = new QStackedWidget();
    mainStack->setStyleSheet(R"(
        QStackedWidget {
            background: #0F0F14;
        }
    )");

    MyMusicPage *myMusicPage = new MyMusicPage(currentUserId, this);
    SearchMusicPage *musicSearchPage = new SearchMusicPage(currentUserId, audioPlayer, this);
    FriendsPage *friendsPage = new FriendsPage(currentUserId, this);
    MessagesPage *messagesPage = new MessagesPage(currentUserId, this);

    MessagesPage *messagesPagePtr = dynamic_cast<MessagesPage*>(messagesPage);
    FriendsPage *friendsPagePtr = dynamic_cast<FriendsPage*>(friendsPage);

    if (friendsPagePtr && messagesPagePtr) {
        connect(friendsPagePtr, &FriendsPage::openChatWithFriend, this, [this, messagesPagePtr](int friendId, const QString &friendName) {
            messagesPagePtr->openChat(friendId, friendName);
            mainStack->setCurrentWidget(messagesPagePtr);
        });
    }

    QWidget *profilePage = createSimplePage("Профиль", "Здесь будет информация профиля");
    QWidget *notificationsPage = createSimplePage("Уведомления", "Здесь будут уведомления");
    QWidget *playlistPage = createSimplePage("Плейлисты", "Здесь будут плейлисты");
    RoomsPage *roomsPage = new RoomsPage(currentUserId, this);

    mainStack->addWidget(myMusicPage);
    mainStack->addWidget(profilePage);
    mainStack->addWidget(messagesPage);
    mainStack->addWidget(friendsPage);
    mainStack->addWidget(notificationsPage);
    mainStack->addWidget(playlistPage);
    mainStack->addWidget(musicSearchPage);
    mainStack->addWidget(roomsPage);

    connect(musicSearchPage, &SearchMusicPage::stationAdded, this, &MainWindow::onRadioStationAdded);

    qDebug() << "createPages: Все страницы созданы и подключены для userId:" << currentUserId;
}

QWidget* MainWindow::createSimplePage(const QString &title, const QString &description)
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(40, 40, 40, 40);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #FFFFFF;
            font-size: 32px;
            font-weight: bold;
            text-align: center;
            margin-bottom: 20px;
            background: transparent;
            border: none;
        }
    )");

    QLabel *descLabel = new QLabel(description);
    descLabel->setStyleSheet(R"(
        QLabel {
            color: rgba(255, 255, 255, 0.5);
            font-size: 16px;
            text-align: center;
            background: transparent;
            border: none;
        }
    )");

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(descLabel);
    layout->addStretch();

    return page;
}
