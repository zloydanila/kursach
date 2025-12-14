#include "Sidebar.h"
#include <QLabel>

Sidebar::Sidebar(const QString& username, QWidget *parent)
    : QWidget(parent)
    , m_username(username)
    , m_currentPage(0)
{
    setupUI();
}

void Sidebar::setupUI()
{
    setFixedWidth(250);
    setStyleSheet(R"(
        QWidget {
            background: rgba(0, 0, 0, 0.4);
            border-right: 1px solid rgba(255, 255, 255, 0.05);
        }
    )");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    QWidget* headerWidget = new QWidget();
    headerWidget->setFixedHeight(80);
    QVBoxLayout* headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel* logoLabel = new QLabel("SoundSpace");
    logoLabel->setStyleSheet("color: #8A2BE2; font-size: 24px; font-weight: 800;");
    
    QLabel* usernameLabel = new QLabel(m_username);
    usernameLabel->setStyleSheet("color: rgba(255, 255, 255, 0.7); font-size: 13px;");
    
    headerLayout->addWidget(logoLabel);
    headerLayout->addWidget(usernameLabel);
    
    mainLayout->addWidget(headerWidget);
    
    QWidget* navWidget = new QWidget();
    m_navLayout = new QVBoxLayout(navWidget);
    m_navLayout->setContentsMargins(15, 20, 15, 20);
    m_navLayout->setSpacing(8);
    
    createNavButton("🏠", "Главная", 0);
    createNavButton("🎵", "Моя музыка", 1);
    createNavButton("🔍", "Поиск музыки", 2);
    createNavButton("📋", "Плейлисты", 3);
    createNavButton("👥", "Друзья", 4);
    createNavButton("💬", "Сообщения", 5);
    createNavButton("🎧", "Комнаты", 6);
    createNavButton("👤", "Профиль", 7);
    
    mainLayout->addWidget(navWidget);
    mainLayout->addStretch();
    
    QWidget* footerWidget = new QWidget();
    QVBoxLayout* footerLayout = new QVBoxLayout(footerWidget);
    footerLayout->setContentsMargins(15, 20, 15, 20);
    
    QPushButton* logoutBtn = new QPushButton("🚪 Выход");
    logoutBtn->setStyleSheet(R"(
        QPushButton {
            background: rgba(255, 68, 68, 0.1);
            border: 1px solid rgba(255, 68, 68, 0.3);
            color: #FF4444;
            padding: 12px;
            border-radius: 10px;
            text-align: left;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background: rgba(255, 68, 68, 0.2);
        }
    )");
    
    connect(logoutBtn, &QPushButton::clicked, this, &Sidebar::logoutRequested);
    
    footerLayout->addWidget(logoutBtn);
    mainLayout->addWidget(footerWidget);
    
    if (!m_navButtons.isEmpty()) {
        m_navButtons[0]->setChecked(true);
    }
}

void Sidebar::createNavButton(const QString& icon, const QString& text, int index)
{
    QPushButton* btn = new QPushButton(QString("%1  %2").arg(icon, text));
    btn->setCheckable(true);
    btn->setFixedHeight(50);
    btn->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            color: rgba(255, 255, 255, 0.7);
            padding: 10px 20px;
            border-radius: 10px;
            text-align: left;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.05);
            color: white;
        }
        QPushButton:checked {
            background: rgba(138, 43, 226, 0.2);
            color: #8A2BE2;
            font-weight: 600;
        }
    )");
    
    connect(btn, &QPushButton::clicked, [this, index]() {
        for (QPushButton* button : m_navButtons) {
            button->setChecked(false);
        }
        m_navButtons[index]->setChecked(true);
        m_currentPage = index;
        emit pageChanged(index);
    });
    
    m_navButtons.append(btn);
    m_navLayout->addWidget(btn);
}

void Sidebar::setActivePage(int index)
{
    if (index >= 0 && index < m_navButtons.size()) {
        for (QPushButton* button : m_navButtons) {
            button->setChecked(false);
        }
        m_navButtons[index]->setChecked(true);
        m_currentPage = index;
    }
}
