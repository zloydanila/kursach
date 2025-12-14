#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

class LoginForm;
class RegisterForm;

class AuthWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onLoginSuccess(const QString& username, int userId);
    void onRegisterSuccess();
    void switchToLogin();
    void switchToRegister();

private:
    void setupUI();
    void showMainWindow(const QString& username, int userId);
    
    QStackedWidget* m_stackedWidget;
    LoginForm* m_loginForm;
    RegisterForm* m_registerForm;
    
    QWidget* m_titleBarWidget;
    bool m_dragging;
    QPoint m_dragPosition;
};

#endif
