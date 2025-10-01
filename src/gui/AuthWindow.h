#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QLineEdit;
class QPushButton;
class QLabel;
class QStatusBar;

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
    
    // Основные виджеты
    QStackedWidget *m_stackedWidget;
    
    // Страница регистрации
    QWidget *m_registerPage;
    QLineEdit *m_registerUsername;
    QLineEdit *m_registerPassword;
    QLineEdit *m_registerConfirmPassword;
    QPushButton *m_registerButton;
    QPushButton *m_switchToLoginButton;
    
    // Страница входа
    QWidget *m_loginPage;
    QLineEdit *m_loginUsername;
    QLineEdit *m_loginPassword;
    QPushButton *m_loginButton;
    QPushButton *m_switchToRegisterButton;
    
    QStatusBar *m_statusBar;
};

#endif
