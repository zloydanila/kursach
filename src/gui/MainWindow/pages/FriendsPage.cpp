#include "FriendsPage.h"
#include "gui/widgets/FriendCard.h"
#include "gui/dialogs/NotificationDialog.h"

#include <QScrollArea>
#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QFrame>

FriendsPage::FriendsPage(int userId, QWidget *parent)
    : QWidget(parent)
    , m_userId(userId)
    , m_friendManager(new FriendManager(userId, this))
{
    setupUI();
    loadFriends();
}

void FriendsPage::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(25);

    QLabel* titleLabel = new QLabel("Друзья");
    titleLabel->setStyleSheet("QLabel { color: #FFFFFF; font-size: 32px; font-weight: 800; background: transparent; }");

    QWidget* searchWidget = new QWidget();
    searchWidget->setStyleSheet("QWidget { background: transparent; }");
    QHBoxLayout* searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(15);

    m_searchInput = new QLineEdit();
    m_searchInput->setPlaceholderText("Поиск пользователей...");
    m_searchInput->setMinimumHeight(50);

    QPushButton* searchBtn = new QPushButton("Поиск");
    searchBtn->setObjectName("accentButton");
    searchBtn->setCursor(Qt::PointingHandCursor);
    searchBtn->setMinimumHeight(50);
    searchBtn->setFixedWidth(120);

    searchLayout->addWidget(m_searchInput, 1);
    searchLayout->addWidget(searchBtn);

    QWidget* tabsWidget = new QWidget();
    tabsWidget->setStyleSheet("QWidget { background: transparent; }");
    QHBoxLayout* tabsLayout = new QHBoxLayout(tabsWidget);
    tabsLayout->setContentsMargins(0, 0, 0, 0);
    tabsLayout->setSpacing(10);

    m_friendsBtn = new QPushButton("Мои друзья");
    m_requestsBtn = new QPushButton("Запросы");

    m_friendsBtn->setCheckable(true);
    m_requestsBtn->setCheckable(true);
    m_friendsBtn->setChecked(true);

    m_friendsBtn->setProperty("tab", true);
    m_requestsBtn->setProperty("tab", true);
    m_friendsBtn->setCursor(Qt::PointingHandCursor);
    m_requestsBtn->setCursor(Qt::PointingHandCursor);

    tabsLayout->addWidget(m_friendsBtn);
    tabsLayout->addWidget(m_requestsBtn);
    tabsLayout->addStretch();

    m_stackedWidget = new QStackedWidget();

    QWidget* friendsScroll = new QWidget();
    m_friendsLayout = new QVBoxLayout(friendsScroll);
    m_friendsLayout->setContentsMargins(0, 0, 0, 0);
    m_friendsLayout->setSpacing(15);

    QWidget* requestsScroll = new QWidget();
    m_requestsLayout = new QVBoxLayout(requestsScroll);
    m_requestsLayout->setContentsMargins(0, 0, 0, 0);
    m_requestsLayout->setSpacing(15);

    QWidget* searchScroll = new QWidget();
    m_searchLayout = new QVBoxLayout(searchScroll);
    m_searchLayout->setContentsMargins(0, 0, 0, 0);
    m_searchLayout->setSpacing(15);

    m_friendsWidget = new QScrollArea();
    m_friendsWidget->setWidgetResizable(true);
    m_friendsWidget->setFrameShape(QFrame::NoFrame);
    m_friendsWidget->setWidget(friendsScroll);

    m_requestsWidget = new QScrollArea();
    m_requestsWidget->setWidgetResizable(true);
    m_requestsWidget->setFrameShape(QFrame::NoFrame);
    m_requestsWidget->setWidget(requestsScroll);

    m_searchWidget = new QScrollArea();
    m_searchWidget->setWidgetResizable(true);
    m_searchWidget->setFrameShape(QFrame::NoFrame);
    m_searchWidget->setWidget(searchScroll);

    m_stackedWidget->addWidget(m_friendsWidget);
    m_stackedWidget->addWidget(m_requestsWidget);
    m_stackedWidget->addWidget(m_searchWidget);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(searchWidget);
    mainLayout->addWidget(tabsWidget);
    mainLayout->addWidget(m_stackedWidget, 1);

    connect(searchBtn, &QPushButton::clicked, this, &FriendsPage::onSearchClicked);
    connect(m_searchInput, &QLineEdit::returnPressed, this, &FriendsPage::onSearchClicked);
    connect(m_friendsBtn, &QPushButton::clicked, this, &FriendsPage::onShowFriends);
    connect(m_requestsBtn, &QPushButton::clicked, this, &FriendsPage::onShowRequests);
}

static void clearLayout(QVBoxLayout* lay)
{
    while (lay->count() > 0) {
        QLayoutItem* item = lay->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
}

void FriendsPage::loadFriends()
{
    clearLayout(m_friendsLayout);

    QVector<User> friends = m_friendManager->getFriends();

    if (friends.isEmpty()) {
        QLabel* emptyLabel = new QLabel("У вас пока нет друзей");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("QLabel { color: rgba(255,255,255,0.6); background: transparent; }");
        m_friendsLayout->addWidget(emptyLabel);
    } else {
        for (const User& user : friends) {
            auto* card = new FriendCard(user, false, false, this);
            connect(card, &FriendCard::messageClicked, this, [this, user]() { onMessageFriend(user.id); });
            connect(card, &FriendCard::removeClicked, this, [this, user]() { onRemoveFriend(user.id); });
            m_friendsLayout->addWidget(card);
        }
    }

    m_friendsLayout->addStretch();
}

void FriendsPage::loadRequests()
{
    clearLayout(m_requestsLayout);

    QVector<User> requests = m_friendManager->getPendingRequests();

    if (requests.isEmpty()) {
        QLabel* emptyLabel = new QLabel("Нет новых запросов в друзья");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("QLabel { color: rgba(255,255,255,0.6); background: transparent; }");
        m_requestsLayout->addWidget(emptyLabel);
    } else {
        for (const User& user : requests) {
            auto* card = new FriendCard(user, true, false, this);
            connect(card, &FriendCard::acceptClicked, this, [this, user]() { onAcceptRequest(user.id); });
            connect(card, &FriendCard::rejectClicked, this, [this, user]() { onRejectRequest(user.id); });
            m_requestsLayout->addWidget(card);
        }
    }

    m_requestsLayout->addStretch();
}

void FriendsPage::searchUsers()
{
    QString query = m_searchInput->text().trimmed();
    if (query.isEmpty()) {
        NotificationDialog dialog("Введите имя пользователя для поиска", NotificationDialog::Error, this);
        dialog.exec();
        return;
    }
    QVector<User> results = m_friendManager->searchUsers(query);
    displaySearchResults(results);
}

void FriendsPage::displaySearchResults(const QVector<User>& users)
{
    clearLayout(m_searchLayout);

    if (users.isEmpty()) {
        QLabel* emptyLabel = new QLabel("Пользователи не найдены");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("QLabel { color: rgba(255,255,255,0.6); background: transparent; }");
        m_searchLayout->addWidget(emptyLabel);
    } else {
        for (const User& user : users) {
            auto* card = new FriendCard(user, false, true, this);
            connect(card, &FriendCard::addFriendClicked, this, [this, user]() { onAddFriend(user.id); });
            m_searchLayout->addWidget(card);
        }
    }

    m_searchLayout->addStretch();
    m_stackedWidget->setCurrentWidget(m_searchWidget);
}

void FriendsPage::onSearchClicked() { searchUsers(); }

void FriendsPage::onShowFriends()
{
    m_friendsBtn->setChecked(true);
    m_requestsBtn->setChecked(false);
    m_stackedWidget->setCurrentWidget(m_friendsWidget);
    loadFriends();
}

void FriendsPage::onShowRequests()
{
    m_friendsBtn->setChecked(false);
    m_requestsBtn->setChecked(true);
    m_stackedWidget->setCurrentWidget(m_requestsWidget);
    loadRequests();
}

void FriendsPage::onAddFriend(int userId)
{
    if (m_friendManager->sendFriendRequest(userId)) {
        NotificationDialog dialog("Запрос в друзья отправлен!", NotificationDialog::Success, this);
        dialog.exec();
    } else {
        NotificationDialog dialog("Не удалось отправить запрос", NotificationDialog::Error, this);
        dialog.exec();
    }
}

void FriendsPage::onAcceptRequest(int userId)
{
    if (m_friendManager->acceptFriendRequest(userId)) {
        NotificationDialog dialog("Запрос принят!", NotificationDialog::Success, this);
        dialog.exec();
        loadRequests();
        loadFriends();
    } else {
        NotificationDialog dialog("Не удалось принять запрос", NotificationDialog::Error, this);
        dialog.exec();
    }
}

void FriendsPage::onRejectRequest(int userId)
{
    if (m_friendManager->rejectFriendRequest(userId)) {
        loadRequests();
    } else {
        NotificationDialog dialog("Не удалось отклонить запрос", NotificationDialog::Error, this);
        dialog.exec();
    }
}

void FriendsPage::onRemoveFriend(int userId)
{
    if (m_friendManager->removeFriend(userId)) {
        loadFriends();
    } else {
        NotificationDialog dialog("Не удалось удалить друга", NotificationDialog::Error, this);
        dialog.exec();
    }
}

void FriendsPage::onMessageFriend(int userId)
{
    emit openChatWithFriend(userId, QString());
}

void FriendsPage::refreshData()
{
    loadFriends();
    loadRequests();
}
