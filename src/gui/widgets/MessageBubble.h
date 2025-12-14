
#ifndef MESSAGEBUBBLE_H
#define MESSAGEBUBBLE_H

#include <QWidget>
#include <QLabel>
#include "core/models/Message.h"

class MessageBubble : public QWidget
{
    Q_OBJECT

public:
    explicit MessageBubble(const Message& message, bool isOwn, QWidget *parent = nullptr);

private:
    void setupUI(const Message& message, bool isOwn);
};

#endif
