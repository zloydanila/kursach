#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class RegisterForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterForm(QWidget *parent = nullptr);

signals:
    void registerSuccess();
    void switchToLogin();

private slots:
    void onRegisterClicked();

private:
    void setupUI();
    bool validateInput();

    QLineEdit* m_usernameInput;
    QLineEdit* m_passwordInput;
    QLineEdit* m_confirmPasswordInput;
    QPushButton* m_registerButton;
    QPushButton* m_loginButton;
};

#endif
