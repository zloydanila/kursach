#include "LoginForm.h"
#include "database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void LoginForm::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(25);
    
    QLabel* titleLabel = new QLabel("Вход в аккаунт");
    titleLabel->setStyleSheet("color: white; font-size: 24px; font-weight: bold; text-align: center;");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    m_usernameInput = new QLineEdit();
    m_usernameInput->setPlaceholderText("Имя пользователя");
    m_usernameInput->setMinimumHeight(45);
    m_usernameInput->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.08);
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 0 15px;
            color: white;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 2px solid #8A2BE2;
        }
    )");
    
    m_passwordInput = new QLineEdit();
    m_passwordInput->setPlaceholderText("Пароль");
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setMinimumHeight(45);
    m_passwordInput->setStyleSheet(m_usernameInput->styleSheet());
    
    m_loginButton = new QPushButton("ВОЙТИ");
    m_loginButton->setMinimumHeight(45);
    m_loginButton->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
    )");
    
    m_registerButton = new QPushButton("Создать новый аккаунт");
    m_registerButton->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            color: #8A2BE2;
            border: 2px solid #8A2BE2;
            border-radius: 10px;
            font-size: 14px;
            padding: 12px;
            font-weight: 500;
        }
        QPushButton:hover {
            background: rgba(138, 43, 226, 0.1);
        }
    )");
    
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_usernameInput);
    mainLayout->addWidget(m_passwordInput);
    mainLayout->addWidget(m_loginButton);
    mainLayout->addWidget(m_registerButton);
    mainLayout->addStretch();
    
    connect(m_loginButton, &QPushButton::clicked, this, &LoginForm::onLoginClicked);
    connect(m_registerButton, &QPushButton::clicked, this, &LoginForm::switchToRegister);
    connect(m_passwordInput, &QLineEdit::returnPressed, this, &LoginForm::onLoginClicked);
}

bool LoginForm::validateInput()
{
    if (m_usernameInput->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите имя пользователя");
        return false;
    }
    
    if (m_passwordInput->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите пароль");
        return false;
    }
    
    return true;
}

void LoginForm::onLoginClicked()
{
    if (!validateInput()) return;
    
    QString username = m_usernameInput->text().trimmed();
    QString password = m_passwordInput->text();
    
    if (DatabaseManager::instance().authenticateUser(username, password)) {
        int userId = DatabaseManager::instance().getUserId(username);
        emit loginSuccess(username, userId);
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверное имя пользователя или пароль");
    }
}
