#include "MessagesPage.h"
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>

MessagesPage::MessagesPage(int userId, QWidget *parent)
    : QWidget(parent)
    , m_userId(userId)
    , m_chatManager(new ChatManager(userId, this))
    , m_currentChatWidget(nullptr)
{
    setupUI();
    loadChats();
}

void MessagesPage::setupUI()
{

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_stackedWidget = new QStackedWidget();
    
    m_chatListWidget = new QWidget();
    QVBoxLayout* listLayout = new QVBoxLayout(m_chatListWidget);
    listLayout->setContentsMargins(40, 40, 40, 40);
    listLayout->setSpacing(20);
    
    QLabel* titleLabel = new QLabel("Сообщения");
    titleLabel->setStyleSheet(R"(
        color: white;
        font-size: 32px;
        font-weight: bold;
    )");
    
    m_chatList = new QListWidget();
    m_chatList->setStyleSheet(R"(
        QListWidget {
            background: rgba(255, 255, 255, 0.03);
            border: 2px solid rgba(255, 255, 255, 0.05);
            border-radius: 15px;
            color: white;
            font-size: 14px;
            outline: none;
            padding: 10px;
        }
        QListWidget::item {
            background: transparent;
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
            padding: 15px;
            border-radius: 8px;
            margin: 2px;
        }
        QListWidget::item:selected {
            background: rgba(138, 43, 226, 0.15);
            border: 1px solid rgba(138, 43, 226, 0.3);
        }
        QListWidget::item:hover {
            background: rgba(255, 255, 255, 0.05);
        }
    )");
listLayout->addWidget(titleLabel);
    listLayout->addWidget(m_chatList);
    
    m_stackedWidget->addWidget(m_chatListWidget);
    
    mainLayout->addWidget(m_stackedWidget);
    
    connect(m_chatList, &QListWidget::itemClicked, this, &MessagesPage::onChatSelected);
}

void MessagesPage::loadChats()
{
    m_chatList->clear();
    
    QVector<ChatPreview> chats = m_chatManager->getChatList();
    
    if (chats.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem("Нет активных чатов");
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        m_chatList->addItem(item);
    } else {
        for (const ChatPreview& chat : chats) {
            QString itemText = QString("%1\n%2")
                .arg(chat.username)
                .arg(chat.lastMessage.left(50));
                
            QListWidgetItem* item = new QListWidgetItem(itemText);
            item->setData(Qt::UserRole, chat.userId);
            item->setData(Qt::UserRole + 1, chat.username);
            
            if (chat.unreadCount > 0) {
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
                itemText += QString(" (%1)").arg(chat.unreadCount);
                item->setText(itemText);
            }
            
            m_chatList->addItem(item);
        }
    }
}

void MessagesPage::onChatSelected(QListWidgetItem* item)
{
    if (!item) return;
    
    int friendId = item->data(Qt::UserRole).toInt();
    QString friendName = item->data(Qt::UserRole + 1).toString();
    
    openChat(friendId, friendName);
}

void MessagesPage::openChat(int friendId, const QString& friendName)
{
    if (m_currentChatWidget) {
        m_stackedWidget->removeWidget(m_currentChatWidget);
        m_currentChatWidget->deleteLater();
    }
    
    m_currentPeerId = friendId;
    m_currentChatWidget = new ChatWidget(m_userId, friendId, friendName);
    m_stackedWidget->addWidget(m_currentChatWidget);
    m_stackedWidget->setCurrentWidget(m_currentChatWidget);
    
    connect(m_currentChatWidget, &ChatWidget::backClicked, this, &MessagesPage::onBackToList);
}

void MessagesPage::onBackToList()
{
    m_stackedWidget->setCurrentWidget(m_chatListWidget);
    loadChats();
}

void MessagesPage::refreshChats()
{
    if (m_stackedWidget->currentWidget() == m_chatListWidget) {
        loadChats();
    }
}

void MessagesPage::reloadDialog(int peerId)
{
    if (peerId <= 0) return;
    // Очистить текущий вид, подтянуть последние N сообщений из DB и отрисовать
    // Пример:
    // auto msgs = DatabaseManager::instance().getMessages(currentUserId, peerId, 200);
    // renderMessages(msgs);
}
