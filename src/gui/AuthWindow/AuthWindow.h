#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H


#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>  // Добавьте если используется
#include <QStatusBar>   // Добавьте если используется
#include <QRegularExpression>  // Добавьте если используется
#include <QDialog>      // Добавьте если используется
#include <QPixmap>      // Добавьте если используется

class AuthWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();

private slots:
    void switchToLogin();
    void switchToRegister();
    void onRegisterClicked();
    void onLoginClicked();
    void showMainWindow(const QString& username);

private:
    void setupUI();
    void setupConnections();
    void showMessage(const QString& title, const QString& text, bool isError = true);
    void showSqlInjectionError();
    
    // Валидация
    bool isValidEmail(const QString& email);
    bool isValidUsername(const QString& username);
    bool isStrongPassword(const QString& password);
    bool hasSqlInjection(const QString& input);

private:
    // UI элементы
    QStackedWidget *stackedWidget;
    
    // Страница регистрации
    QWidget *regPage;
    QLineEdit *regEmail;
    QLineEdit *regUser;
    QLineEdit *regPass;
    QLineEdit *regConfirmPass;
    QPushButton *regBtn;
    QPushButton *toLoginBtn;
    
    // Страница входа
    QWidget *loginPage;
    QLineEdit *loginUser;
    QLineEdit *loginPass;
    QPushButton *loginBtn;
    QPushButton *toRegBtn;
};

#endif // AUTHWINDOW_H