#ifndef ROOMCARD_H
#define ROOMCARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "core/models/Room.h"

class RoomCard : public QWidget
{
    Q_OBJECT

public:
    explicit RoomCard(const Room& room, QWidget *parent = nullptr);

signals:
    void joinClicked(int roomId);

private:
    void setupUI(const Room& room);
    
    int m_roomId;
    QLabel* m_nameLabel;
    QLabel* m_genreLabel;
    QLabel* m_trackLabel;
    QLabel* m_membersLabel;
    QPushButton* m_joinButton;
};

#endif
