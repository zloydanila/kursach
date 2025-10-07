#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QLineEdit;
class QPushButton;
class QLabel;

class AuthWindow : public QMainWindow
{
    Q_OBJECT

public:
    AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();

private slots:
    void onRegisterClicked();
    void onLoginClicked();
    void switchToLogin();
    void switchToRegister();

private:
    void setupUI();
    void setupConnections();
    void showMessage(const QString& title, const QString& text, bool isError = true);
    
    bool isValidEmail(const QString& email);
    bool isValidUsername(const QString& username);
    bool isStrongPassword(const QString& password);
    
    QStackedWidget *stackedWidget;
    QWidget *regPage;
    QLineEdit *regEmail;
    QLineEdit *regUser;
    QLineEdit *regPass;
    QLineEdit *regConfirmPass;
    QPushButton *regBtn;
    QPushButton *toLoginBtn;
    QWidget *loginPage;
    QLineEdit *loginUser;
    QLineEdit *loginPass;
    QPushButton *loginBtn;
    QPushButton *toRegBtn;
};

#endif