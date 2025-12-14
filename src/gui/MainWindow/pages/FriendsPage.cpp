#include "FriendsPage.h"
#include "gui/widgets/FriendCard.h"
#include <QMessageBox>
#include <QScrollArea>
#include <QLabel>
#include <QDebug>

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
    
    QLabel* titleLabel = new QLabel("👥 Друзья");
    titleLabel->setStyleSheet(R"(
        color: white;
        font-size: 32px;
        font-weight: bold;
    )");
    
    QWidget* searchWidget = new QWidget();
    searchWidget->setStyleSheet("background: transparent;");
    QHBoxLayout* searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(15);
    
    m_searchInput = new QLineEdit();
    m_searchInput->setPlaceholderText("Поиск пользователей...");
    m_searchInput->setMinimumHeight(50);
    m_searchInput->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 12px;
            padding: 0 20px;
            color: white;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 2px solid #8A2BE2;
        }
    )");
    
    QPushButton* searchBtn = new QPushButton("🔍 Поиск");
    searchBtn->setMinimumHeight(50);
    searchBtn->setFixedWidth(120);
    searchBtn->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 12px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
    )");
    
    searchLayout->addWidget(m_searchInput, 1);
    searchLayout->addWidget(searchBtn);
    
    QWidget* tabsWidget = new QWidget();
    tabsWidget->setStyleSheet("background: transparent;");
    QHBoxLayout* tabsLayout = new QHBoxLayout(tabsWidget);
    tabsLayout->setContentsMargins(0, 0, 0, 0);
    tabsLayout->setSpacing(10);
    
    m_friendsBtn = new QPushButton("Мои друзья");
    m_requestsBtn = new QPushButton("Запросы");
    
    QString tabStyle = R"(
        QPushButton {
            background: rgba(255, 255, 255, 0.05);
            color: rgba(255, 255, 255, 0.7);
            border: none;
            border-radius: 8px;
            padding: 12px 24px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.1);
            color: white;
        }
        QPushButton:checked {
            background: #8A2BE2;
            color: white;
        }
    )";
    
    m_friendsBtn->setCheckable(true);
    m_friendsBtn->setChecked(true);
    m_friendsBtn->setStyleSheet(tabStyle);
    
    m_requestsBtn->setCheckable(true);
    m_requestsBtn->setStyleSheet(tabStyle);
    
    tabsLayout->addWidget(m_friendsBtn);
    tabsLayout->addWidget(m_requestsBtn);
    tabsLayout->addStretch();
    
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->setStyleSheet("background: transparent;");
    
    QWidget* friendsScroll = new QWidget();
    friendsScroll->setStyleSheet("background: transparent;");
    m_friendsLayout = new QVBoxLayout(friendsScroll);
    m_friendsLayout->setContentsMargins(0, 0, 0, 0);
    m_friendsLayout->setSpacing(15);
    
    QWidget* requestsScroll = new QWidget();
    requestsScroll->setStyleSheet("background: transparent;");
    m_requestsLayout = new QVBoxLayout(requestsScroll);
    m_requestsLayout->setContentsMargins(0, 0, 0, 0);
    m_requestsLayout->setSpacing(15);
    
    QWidget* searchScroll = new QWidget();
    searchScroll->setStyleSheet("background: transparent;");
    m_searchLayout = new QVBoxLayout(searchScroll);
    m_searchLayout->setContentsMargins(0, 0, 0, 0);
    m_searchLayout->setSpacing(15);
    
    m_friendsWidget = new QScrollArea();
    m_friendsWidget->setWidgetResizable(true);
    m_friendsWidget->setWidget(friendsScroll);
    m_friendsWidget->setStyleSheet("QScrollArea { background: transparent; border: none; } QWidget { background: transparent; }");
    
    m_requestsWidget = new QScrollArea();
    m_requestsWidget->setWidgetResizable(true);
    m_requestsWidget->setWidget(requestsScroll);
    m_requestsWidget->setStyleSheet("QScrollArea { background: transparent; border: none; } QWidget { background: transparent; }");
    
    m_searchWidget = new QScrollArea();
    m_searchWidget->setWidgetResizable(true);
    m_searchWidget->setWidget(searchScroll);
    m_searchWidget->setStyleSheet("QScrollArea { background: transparent; border: none; } QWidget { background: transparent; }");
    
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

void FriendsPage::loadFriends()
{
    qDebug() << "=== Loading friends ===";
    
    while (m_friendsLayout->count() > 0) {
        QLayoutItem* item = m_friendsLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    QVector<User> friends = m_friendManager->getFriends();
    qDebug() << "Found" << friends.size() << "friends";
    
    if (friends.isEmpty()) {
        QLabel* emptyLabel = new QLabel("У вас пока нет друзей 😔");
        emptyLabel->setStyleSheet("color: rgba(255, 255, 255, 0.7); font-size: 16px; padding: 20px;");
        emptyLabel->setAlignment(Qt::AlignCenter);
        m_friendsLayout->addWidget(emptyLabel);
        qDebug() << "Added empty label";
    } else {
        for (const User& user : friends) {
            qDebug() << "Adding friend card for:" << user.username;
            FriendCard* card = new FriendCard(user, false, false);
            card->setFixedHeight(90);
            card->setStyleSheet(R"(
                FriendCard {
                    background: rgba(255, 255, 255, 0.03);
                    border: 1px solid rgba(255, 255, 255, 0.05);
                    border-radius: 12px;
                }
                FriendCard:hover {
                    background: rgba(255, 255, 255, 0.05);
                    border: 1px solid rgba(138, 43, 226, 0.3);
                }
            )");
            
            connect(card, &FriendCard::messageClicked, this, [this, user]() {
                onMessageFriend(user.id);
            });
            
            connect(card, &FriendCard::removeClicked, this, [this, user]() {
                onRemoveFriend(user.id);
            });
            
            m_friendsLayout->addWidget(card);
        }
    }
    
    m_friendsLayout->addStretch();
    qDebug() << "=== Friends loading complete ===";
}

void FriendsPage::loadRequests()
{
    qDebug() << "=== Loading requests ===";
    
    while (m_requestsLayout->count() > 0) {
        QLayoutItem* item = m_requestsLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    QVector<User> requests = m_friendManager->getPendingRequests();
    qDebug() << "Found" << requests.size() << "requests";
    
    if (requests.isEmpty()) {
        QLabel* emptyLabel = new QLabel("Нет новых запросов в друзья");
        emptyLabel->setStyleSheet("color: rgba(255, 255, 255, 0.7); font-size: 16px; padding: 20px;");
        emptyLabel->setAlignment(Qt::AlignCenter);
        m_requestsLayout->addWidget(emptyLabel);
    } else {
        for (const User& user : requests) {
            FriendCard* card = new FriendCard(user, true, false);
            card->setFixedHeight(90);
            card->setStyleSheet(R"(
                FriendCard {
                    background: rgba(255, 255, 255, 0.03);
                    border: 1px solid rgba(255, 255, 255, 0.05);
                    border-radius: 12px;
                }
                FriendCard:hover {
                    background: rgba(255, 255, 255, 0.05);
                    border: 1px solid rgba(138, 43, 226, 0.3);
                }
            )");
            
            connect(card, &FriendCard::acceptClicked, this, [this, user]() {
                onAcceptRequest(user.id);
            });
            
            connect(card, &FriendCard::rejectClicked, this, [this, user]() {
                onRejectRequest(user.id);
            });
            
            m_requestsLayout->addWidget(card);
        }
    }
    
    m_requestsLayout->addStretch();
}

void FriendsPage::searchUsers()
{
    QString query = m_searchInput->text().trimmed();
    qDebug() << "=== Searching for:" << query << "===";
    
    if (query.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите имя пользователя для поиска");
        return;
    }
    
    QVector<User> results = m_friendManager->searchUsers(query);
    qDebug() << "Search returned" << results.size() << "results";
    
    for (const User& user : results) {
        qDebug() << "  - User:" << user.username << "ID:" << user.id;
    }
    
    displaySearchResults(results);
}

void FriendsPage::displaySearchResults(const QVector<User>& users)
{
    qDebug() << "=== Displaying" << users.size() << "search results ===";
    
    while (m_searchLayout->count() > 0) {
        QLayoutItem* item = m_searchLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    if (users.isEmpty()) {
        QLabel* emptyLabel = new QLabel("Пользователи не найдены");
        emptyLabel->setStyleSheet("color: rgba(255, 255, 255, 0.7); font-size: 16px; padding: 20px;");
        emptyLabel->setAlignment(Qt::AlignCenter);
        m_searchLayout->addWidget(emptyLabel);
        qDebug() << "Added 'no results' label";
    } else {
        for (const User& user : users) {
            qDebug() << "Creating search result card for:" << user.username;
            FriendCard* card = new FriendCard(user, false, true);
            card->setFixedHeight(90);
            card->setStyleSheet(R"(
                FriendCard {
                    background: rgba(255, 255, 255, 0.03);
                    border: 1px solid rgba(255, 255, 255, 0.05);
                    border-radius: 12px;
                }
                FriendCard:hover {
                    background: rgba(255, 255, 255, 0.05);
                    border: 1px solid rgba(138, 43, 226, 0.3);
                }
            )");
            
            connect(card, &FriendCard::addFriendClicked, this, [this, user]() {
                qDebug() << "Add friend clicked for:" << user.username;
                onAddFriend(user.id);
            });
            
            m_searchLayout->addWidget(card);
            qDebug() << "Card added to layout";
        }
    }
    
    m_searchLayout->addStretch();
    m_stackedWidget->setCurrentWidget(m_searchWidget);
    qDebug() << "Switched to search widget";
    qDebug() << "=== Display complete ===";
}

void FriendsPage::onSearchClicked()
{
    qDebug() << "Search button clicked";
    searchUsers();
}

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
    qDebug() << "Sending friend request to user:" << userId;
    if (m_friendManager->sendFriendRequest(userId)) {
        QMessageBox::information(this, "Успех", "Запрос в друзья отправлен!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось отправить запрос");
    }
}

void FriendsPage::onAcceptRequest(int userId)
{
    if (m_friendManager->acceptFriendRequest(userId)) {
        QMessageBox::information(this, "Успех", "Запрос принят!");
        loadRequests();
        loadFriends();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось принять запрос");
    }
}

void FriendsPage::onRejectRequest(int userId)
{
    if (m_friendManager->rejectFriendRequest(userId)) {
        loadRequests();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось отклонить запрос");
    }
}

void FriendsPage::onRemoveFriend(int userId)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "Удаление из друзей", 
        "Вы уверены, что хотите удалить пользователя из друзей?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (m_friendManager->removeFriend(userId)) {
            loadFriends();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить друга");
        }
    }
}

void FriendsPage::onMessageFriend(int userId)
{
    QVector<User> friends = m_friendManager->getFriends();
    for (const User& user : friends) {
        if (user.id == userId) {
            emit openChatWithFriend(userId, user.username);
            break;
        }
    }
}

void FriendsPage::refreshData()
{
    if (m_stackedWidget->currentWidget() == m_friendsWidget) {
        loadFriends();
    } else if (m_stackedWidget->currentWidget() == m_requestsWidget) {
        loadRequests();
    }
}
