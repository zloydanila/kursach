#include "MessageBubble.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

MessageBubble::MessageBubble(const Message& message, bool isOwn, QWidget *parent)
    : QWidget(parent)
{
    setupUI(message, isOwn);
}

void MessageBubble::setupUI(const Message& message, bool isOwn)
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    
    if (isOwn) {
        mainLayout->addStretch();
    }
    
    QWidget* bubbleWidget = new QWidget();
    bubbleWidget->setMaximumWidth(400);
    
    QString bubbleStyle;
    if (isOwn) {
        bubbleStyle = "QWidget { background: #8A2BE2; border-radius: 15px; padding: 10px 15px; }";
    } else {
        bubbleStyle = "QWidget { background: rgba(255, 255, 255, 0.1); border-radius: 15px; padding: 10px 15px; }";
    }
    bubbleWidget->setStyleSheet(bubbleStyle);
    
    QVBoxLayout* bubbleLayout = new QVBoxLayout(bubbleWidget);
    bubbleLayout->setContentsMargins(5, 5, 5, 5);
    bubbleLayout->setSpacing(5);
    
    if (!isOwn) {
        QLabel* senderLabel = new QLabel(message.senderUsername);
        senderLabel->setStyleSheet("color: #8A2BE2; font-size: 12px; font-weight: 600;");
        bubbleLayout->addWidget(senderLabel);
    }
    
    QLabel* contentLabel = new QLabel(message.content);
    contentLabel->setWordWrap(true);
    contentLabel->setStyleSheet("color: white; font-size: 14px;");
    bubbleLayout->addWidget(contentLabel);
    
    QLabel* timeLabel = new QLabel(message.timestamp.toString("hh:mm"));
    timeLabel->setStyleSheet("color: rgba(255, 255, 255, 0.5); font-size: 11px;");
    timeLabel->setAlignment(isOwn ? Qt::AlignRight : Qt::AlignLeft);
    bubbleLayout->addWidget(timeLabel);
    
    mainLayout->addWidget(bubbleWidget);
    
    if (!isOwn) {
        mainLayout->addStretch();
    }
}
