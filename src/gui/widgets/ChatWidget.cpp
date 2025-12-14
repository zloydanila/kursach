#include "ChatWidget.h"
#include "MessageBubble.h"
#include "network/ChatManager.h"
#include <QScrollBar>
#include <QMessageBox>

ChatWidget::ChatWidget(int userId, int friendId, const QString& friendName, QWidget *parent)
    : QWidget(parent)
    , m_userId(userId)
    , m_friendId(friendId)
    , m_friendName(friendName)
    , m_chatManager(new ChatManager(userId, this))
{
    setupUI();
    loadMessages();
}

void ChatWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    QWidget* headerWidget = new QWidget();
    headerWidget->setFixedHeight(70);
    headerWidget->setStyleSheet(R"(
        QWidget {
            background: rgba(0, 0, 0, 0.3);
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }
    )");
    
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 0, 20, 0);
    
    QPushButton* backBtn = new QPushButton("← Назад");
    backBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            color: #8A2BE2;
            border: none;
            font-size: 14px;
            font-weight: 600;
            padding: 10px;
        }
        QPushButton:hover {
            color: #9B4BFF;
        }
    )");
    
    QLabel* nameLabel = new QLabel(m_friendName);
    nameLabel->setStyleSheet(R"(
        color: white;
        font-size: 18px;
        font-weight: 600;
    )");
    
    headerLayout->addWidget(backBtn);
    headerLayout->addWidget(nameLabel);
    headerLayout->addStretch();
    
    m_messagesArea = new QScrollArea();
    m_messagesArea->setWidgetResizable(true);
    m_messagesArea->setStyleSheet(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
    )");
    
    QWidget* messagesContent = new QWidget();
    m_messagesLayout = new QVBoxLayout(messagesContent);
    m_messagesLayout->setContentsMargins(20, 20, 20, 20);
    m_messagesLayout->setSpacing(10);
    m_messagesLayout->addStretch();
    m_messagesArea->setWidget(messagesContent);
    
    QWidget* inputWidget = new QWidget();
    inputWidget->setFixedHeight(80);
    inputWidget->setStyleSheet(R"(
        QWidget {
            background: rgba(0, 0, 0, 0.3);
            border-top: 1px solid rgba(255, 255, 255, 0.1);
        }
    )");
    
    QHBoxLayout* inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setContentsMargins(20, 15, 20, 15);
    inputLayout->setSpacing(15);
    
    m_messageInput = new QLineEdit();
    m_messageInput->setPlaceholderText("Введите сообщение...");
    m_messageInput->setStyleSheet(R"(
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
    
    QPushButton* sendBtn = new QPushButton("Отправить");
    sendBtn->setFixedWidth(120);
    sendBtn->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 12px;
            font-size: 14px;
            font-weight: 600;
            padding: 12px;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
    )");
    
    inputLayout->addWidget(m_messageInput);
    inputLayout->addWidget(sendBtn);
    
    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(m_messagesArea, 1);
    mainLayout->addWidget(inputWidget);
    
    connect(backBtn, &QPushButton::clicked, this, &ChatWidget::backClicked);
    connect(sendBtn, &QPushButton::clicked, this, &ChatWidget::onSendMessage);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &ChatWidget::onSendMessage);
}

void ChatWidget::loadMessages()
{
    while (m_messagesLayout->count() > 1) {
        QLayoutItem* item = m_messagesLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    QVector<Message> messages = m_chatManager->getMessages(m_friendId, 50);
    
    for (int i = messages.size() - 1; i >= 0; --i) {
        const Message& msg = messages[i];
        bool isOwn = (msg.senderId == m_userId);
        
        MessageBubble* bubble = new MessageBubble(msg, isOwn);
        m_messagesLayout->insertWidget(m_messagesLayout->count() - 1, bubble);
        
        if (!isOwn && !msg.isRead) {
            m_chatManager->markAsRead(msg.id);
        }
    }
    
    QTimer::singleShot(100, this, [this]() {
        m_messagesArea->verticalScrollBar()->setValue(
            m_messagesArea->verticalScrollBar()->maximum()
        );
    });
}

void ChatWidget::onSendMessage()
{
    QString text = m_messageInput->text().trimmed();
    
    if (text.isEmpty()) {
        return;
    }
    
    if (m_chatManager->sendMessage(m_friendId, text)) {
        m_messageInput->clear();
        loadMessages();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось отправить сообщение");
    }
}
