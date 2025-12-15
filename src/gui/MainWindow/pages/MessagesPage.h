#ifndef MESSAGESPAGE_H
#define MESSAGESPAGE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include "network/ChatManager.h"
#include "gui/widgets/ChatWidget.h"

class MessagesPage : public QWidget {
    Q_OBJECT
public slots:
    void reloadDialog(int peerId);
public:
    explicit MessagesPage(int userId, QWidget *parent = nullptr);
    
    void openChat(int friendId, const QString& friendName);
    void refreshChats();

private slots:
    void onChatSelected(QListWidgetItem* item);
    void onBackToList();

private:
    void setupUI();
    void loadChats();
    
    int m_userId;
    ChatManager* m_chatManager;
    
    QStackedWidget* m_stackedWidget;
    QWidget* m_chatListWidget;
    QListWidget* m_chatList;
    ChatWidget* m_currentChatWidget;
    int m_currentPeerId = -1;
    QTimer* m_refreshTimer = nullptr;
};

#endif
