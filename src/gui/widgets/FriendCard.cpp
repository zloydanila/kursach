#include "FriendCard.h"
#include "UserAvatar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

FriendCard::FriendCard(const User& user, bool isRequest, bool isSearchResult, QWidget *parent)
    : QWidget(parent)
{
    setupUI(user, isRequest, isSearchResult);
}

void FriendCard::setupUI(const User& user, bool isRequest, bool isSearchResult)
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    // Avatar (shared via DB avatar_data inside UserAvatar)
    auto *avatarWidget = new UserAvatar(60, this);
    avatarWidget->setUser(user);

    // Info
    QWidget* infoWidget = new QWidget();
    infoWidget->setStyleSheet("background: transparent;");
    QVBoxLayout* infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(4);

    m_usernameLabel = new QLabel(user.username);
    m_usernameLabel->setStyleSheet("color: white; font-size: 16px; font-weight: 600; background: transparent;");

    m_statusLabel = new QLabel();
    m_statusLabel->setStyleSheet("color: rgba(255,255,255,0.6); font-size: 13px; background: transparent;");
    if (!getStatusText(user.status).isEmpty()) {
        m_statusLabel->setText(getStatusText(user.status));
        m_statusLabel->setStyleSheet(QString("color: %1; font-size: 13px; background: transparent;").arg(getStatusColor(user.status)));
    }

    infoLayout->addWidget(m_usernameLabel);
    infoLayout->addWidget(m_statusLabel);

    mainLayout->addWidget(avatarWidget);
    mainLayout->addWidget(infoWidget, 1);

    // Buttons
    if (isRequest) {
        QPushButton* acceptBtn = new QPushButton("Принять");
        acceptBtn->setStyleSheet("QPushButton { background: #4CAF50; color: white; border: none; border-radius: 8px; padding: 8px 20px; font-weight: 600; } QPushButton:hover { background: #45a049; }");
        connect(acceptBtn, &QPushButton::clicked, this, &FriendCard::acceptClicked);

        QPushButton* rejectBtn = new QPushButton("Отклонить");
        rejectBtn->setStyleSheet("QPushButton { background: #f44336; color: white; border: none; border-radius: 8px; padding: 8px 20px; font-weight: 600; } QPushButton:hover { background: #da190b; }");
        connect(rejectBtn, &QPushButton::clicked, this, &FriendCard::rejectClicked);

        mainLayout->addWidget(acceptBtn);
        mainLayout->addWidget(rejectBtn);
    } else if (isSearchResult) {
        QPushButton* addBtn = new QPushButton("Добавить");
        addBtn->setStyleSheet("QPushButton { background: #8A2BE2; color: white; border: none; border-radius: 8px; padding: 8px 20px; font-weight: 600; } QPushButton:hover { background: #9B4BFF; }");
        connect(addBtn, &QPushButton::clicked, this, &FriendCard::addFriendClicked);
        mainLayout->addWidget(addBtn);
    } else {
        QPushButton* messageBtn = new QPushButton("Написать");
        messageBtn->setStyleSheet("QPushButton { background: #8A2BE2; color: white; border: none; border-radius: 8px; padding: 8px 20px; font-weight: 600; } QPushButton:hover { background: #9B4BFF; }");
        connect(messageBtn, &QPushButton::clicked, this, &FriendCard::messageClicked);

        QPushButton* removeBtn = new QPushButton("Удалить");
        removeBtn->setStyleSheet("QPushButton { background: rgba(244, 67, 54, 0.2); color: #f44336; border: 1px solid #f44336; border-radius: 8px; padding: 8px 20px; font-weight: 600; } QPushButton:hover { background: rgba(244, 67, 54, 0.3); }");
        connect(removeBtn, &QPushButton::clicked, this, &FriendCard::removeClicked);

        mainLayout->addWidget(messageBtn);
        mainLayout->addWidget(removeBtn);
    }
}

QString FriendCard::getStatusColor(UserStatus status)
{
    switch (status) {
        case UserStatus::Online: return "#4CAF50";
        case UserStatus::Away: return "#FFC107";
        case UserStatus::DoNotDisturb: return "#F44336";
        case UserStatus::Invisible:
        case UserStatus::Offline: return "#9E9E9E";
        default: return "rgba(255,255,255,0.6)";
    }
}

QString FriendCard::getStatusText(UserStatus status)
{
    switch (status) {
        case UserStatus::Online: return "Онлайн";
        case UserStatus::Away: return "Нет на месте";
        case UserStatus::DoNotDisturb: return "Не беспокоить";
        case UserStatus::Invisible: return "Невидимка";
        case UserStatus::Offline: return "Не в сети";
        default: return "";
    }
}
