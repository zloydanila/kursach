#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QString>

class NotificationDialog : public QDialog
{
    Q_OBJECT
public:
    enum Type { Success, Error, Info };

    explicit NotificationDialog(const QString &message, Type type = Type::Info, QWidget *parent = nullptr);
    ~NotificationDialog();

private:
    void setupUI(const QString &message, Type type);
};

#endif
