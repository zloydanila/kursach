#pragma once
#include <QDialog>

class SqlInjectionDialog : public QDialog {
    Q_OBJECT
public:
    explicit SqlInjectionDialog(QWidget* parent = nullptr);
};
