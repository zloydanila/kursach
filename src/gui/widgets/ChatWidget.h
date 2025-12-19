#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>

#include "network/ChatManager.h"

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(int userId, int friendId, const QString& friendName, QWidget *parent = nullptr);

signals:
    void backClicked();

private slots:
    void onSendMessage();

private:
    void setupUI();
    void loadMessages();

    int m_userId;
    int m_friendId;
    QString m_friendName;
    ChatManager* m_chatManager;

    QScrollArea* m_messagesArea;
    QVBoxLayout* m_messagesLayout;
    QLineEdit* m_messageInput;

    QTimer* m_pollTimer = nullptr;
};

#endif
