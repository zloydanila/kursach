#include "RegisterForm.h"
#include "database/DatabaseManager.h"
#include "gui/dialogs/NotificationDialog.h"

#include <QVBoxLayout>
#include <QLabel>

RegisterForm::RegisterForm(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void RegisterForm::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);

    QLabel* titleLabel = new QLabel("Создать аккаунт");
    titleLabel->setStyleSheet("color: white; font-size: 24px; font-weight: bold; text-align: center;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QString inputStyle = R"(
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
    )";

    m_usernameInput = new QLineEdit();
    m_usernameInput->setPlaceholderText("Имя пользователя");
    m_usernameInput->setMinimumHeight(40);
    m_usernameInput->setStyleSheet(inputStyle);

    m_passwordInput = new QLineEdit();
    m_passwordInput->setPlaceholderText("Пароль");
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setMinimumHeight(40);
    m_passwordInput->setStyleSheet(inputStyle);

    m_confirmPasswordInput = new QLineEdit();
    m_confirmPasswordInput->setPlaceholderText("Подтвердите пароль");
    m_confirmPasswordInput->setEchoMode(QLineEdit::Password);
    m_confirmPasswordInput->setMinimumHeight(40);
    m_confirmPasswordInput->setStyleSheet(inputStyle);

    QLabel* rulesLabel = new QLabel(
        "• Минимум 6 символов\n"
        "• Буквы и цифры\n"
        "• Без специальных символов"
    );
    rulesLabel->setStyleSheet(R"(
        color: rgba(255, 255, 255, 0.5);
        font-size: 12px;
        background: rgba(255, 255, 255, 0.05);
        padding: 10px;
        border-radius: 8px;
    )");

    m_registerButton = new QPushButton("ЗАРЕГИСТРИРОВАТЬСЯ");
    m_registerButton->setMinimumHeight(45);
    m_registerButton->setStyleSheet(R"(
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

    m_loginButton = new QPushButton("Уже есть аккаунт? Войти");
    m_loginButton->setStyleSheet(R"(
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
    mainLayout->addWidget(m_usernameInput);
    mainLayout->addWidget(m_passwordInput);
    mainLayout->addWidget(m_confirmPasswordInput);
    mainLayout->addWidget(rulesLabel);
    mainLayout->addWidget(m_registerButton);
    mainLayout->addWidget(m_loginButton);
    mainLayout->addStretch();

    connect(m_registerButton, &QPushButton::clicked, this, &RegisterForm::onRegisterClicked);
    connect(m_loginButton, &QPushButton::clicked, this, &RegisterForm::switchToLogin);
}

bool RegisterForm::validateInput()
{
    QString username = m_usernameInput->text().trimmed();
    QString password = m_passwordInput->text();
    QString confirmPassword = m_confirmPasswordInput->text();

    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        NotificationDialog dialog("Все поля должны быть заполнены", NotificationDialog::Error, this);
        dialog.exec();
        return false;
    }

    if (username.length() < 3) {
        NotificationDialog dialog("Имя пользователя должно содержать минимум 3 символа", NotificationDialog::Error, this);
        dialog.exec();
        return false;
    }

    if (password.length() < 6) {
        NotificationDialog dialog("Пароль должен содержать минимум 6 символов", NotificationDialog::Error, this);
        dialog.exec();
        return false;
    }

    if (password != confirmPassword) {
        NotificationDialog dialog("Пароли не совпадают", NotificationDialog::Error, this);
        dialog.exec();
        return false;
    }

    return true;
}

void RegisterForm::onRegisterClicked()
{
    if (!validateInput()) return;

    QString username = m_usernameInput->text().trimmed();
    QString password = m_passwordInput->text();

    if (DatabaseManager::instance().registerUser(username, password)) {
        NotificationDialog dialog("Регистрация прошла успешно!", NotificationDialog::Success, this);
        dialog.exec();
        emit registerSuccess();
    } else {
        NotificationDialog dialog("Имя пользователя уже занято", NotificationDialog::Error, this);
        dialog.exec();
    }
}
