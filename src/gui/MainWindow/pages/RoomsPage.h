#ifndef ROOMSPAGE_H
#define ROOMSPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include "core/models/Room.h"

class RoomManager;
class WebSocketClient;

class RoomsPage : public QWidget
{
    Q_OBJECT

public:
    explicit RoomsPage(int userId, QWidget *parent = nullptr);
    ~RoomsPage();
    
    void loadRooms();

private slots:
    void onCreateRoom();
    void onJoinRoom(int roomId);
    void onLeaveRoom();
    void onSearchRooms(const QString& query);

private:
    void setupUI();
    void displayRooms(const QVector<Room>& rooms);
    void openRoomDialog();
    
    int m_userId;
    int m_currentRoomId;
    RoomManager* m_roomManager;
    WebSocketClient* m_wsClient;
    
    QListWidget* m_roomsList;
    QPushButton* m_createButton;
    QLineEdit* m_searchEdit;
    QLineEdit* m_searchInput;
    QScrollArea* m_scrollArea;
    QWidget* m_roomsContainer;
};

#endif


