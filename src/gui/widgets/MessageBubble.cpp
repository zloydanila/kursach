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
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 4, 10, 4);
    mainLayout->setSpacing(0);

    if (isOwn) mainLayout->addStretch(1);

    QWidget* bubbleWidget = new QWidget(this);
    bubbleWidget->setObjectName(isOwn ? "ownBubble" : "otherBubble");
    bubbleWidget->setMaximumWidth(520);

    bubbleWidget->setStyleSheet(R"(
        QWidget#ownBubble {
            background: #8A2BE2;
            border-radius: 16px;
        }
        QWidget#otherBubble {
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.06);
            border-radius: 16px;
        }
    )");

    auto* bubbleLayout = new QVBoxLayout(bubbleWidget);
    bubbleLayout->setContentsMargins(14, 10, 14, 8);
    bubbleLayout->setSpacing(6);

    if (!isOwn) {
        QLabel* senderLabel = new QLabel(message.senderUsername, bubbleWidget);
        senderLabel->setStyleSheet("color: #9B4BFF; font-size: 12px; font-weight: 700; background: transparent;");
        bubbleLayout->addWidget(senderLabel);
    }

    QLabel* contentLabel = new QLabel(message.content, bubbleWidget);
    contentLabel->setWordWrap(true);
    contentLabel->setStyleSheet("color: rgba(255,255,255,0.95); font-size: 14px; background: transparent;");
    bubbleLayout->addWidget(contentLabel);

    QLabel* timeLabel = new QLabel(message.timestamp.toString("hh:mm"), bubbleWidget);
    timeLabel->setStyleSheet("color: rgba(255,255,255,0.55); font-size: 11px; background: transparent;");
    timeLabel->setAlignment(isOwn ? Qt::AlignRight : Qt::AlignLeft);
    bubbleLayout->addWidget(timeLabel);

    mainLayout->addWidget(bubbleWidget);

    if (!isOwn) mainLayout->addStretch(1);
}
