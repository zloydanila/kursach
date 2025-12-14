#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);

signals:
    void loginSuccess(const QString& username, int userId);
    void switchToRegister();

private slots:
    void onLoginClicked();

private:
    void setupUI();
    bool validateInput();
    
    QLineEdit* m_usernameInput;
    QLineEdit* m_passwordInput;
    QPushButton* m_loginButton;
    QPushButton* m_registerButton;
};

#endif
