#include "AuthWindow.h"
#include "../database/DatabaseManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDebug>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStatusBar>

AuthWindow::AuthWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(nullptr)
    , m_registerPage(nullptr)
    , m_registerUsername(nullptr)
    , m_registerPassword(nullptr)
    , m_registerConfirmPassword(nullptr)
    , m_registerButton(nullptr)
    , m_switchToLoginButton(nullptr)
    , m_loginPage(nullptr)
    , m_loginUsername(nullptr)
    , m_loginPassword(nullptr)
    , m_loginButton(nullptr)
    , m_switchToRegisterButton(nullptr)
    , m_statusBar(nullptr)
{
    setupUI();
    setupConnections();
    
    setWindowTitle("SoundSpace - Авторизация");
    setFixedSize(640, 480);
    
    m_statusBar = statusBar();
    m_statusBar->showMessage("Готов к работе");
}

AuthWindow::~AuthWindow()
{
}

void AuthWindow::setupUI()
{
    m_stackedWidget = new QStackedWidget(this);
    
    // ===== СТРАНИЦА РЕГИСТРАЦИИ =====
    m_registerPage = new QWidget();
    QVBoxLayout *registerLayout = new QVBoxLayout(m_registerPage);
    
    QFormLayout *registerForm = new QFormLayout();
    
    m_registerUsername = new QLineEdit();
    m_registerUsername->setPlaceholderText("Введите имя пользователя");
    registerForm->addRow("Имя пользователя:", m_registerUsername);
    
    m_registerPassword = new QLineEdit();
    m_registerPassword->setPlaceholderText("Введите пароль");
    m_registerPassword->setEchoMode(QLineEdit::Password);
    registerForm->addRow("Пароль:", m_registerPassword);
    
    m_registerConfirmPassword = new QLineEdit();
    m_registerConfirmPassword->setPlaceholderText("Повторите пароль");
    m_registerConfirmPassword->setEchoMode(QLineEdit::Password);
    registerForm->addRow("Подтверждение:", m_registerConfirmPassword);
    
    registerLayout->addLayout(registerForm);

    // Условия пароля

    QLabel *requirementsLabel = new QLabel();

    requirementsLabel -> setText (
        "Длина пароля должны быть больше 6 символов\n"
        "Длина имени пользователя больше 3 символов\n"
    );
    requirementsLabel -> setStyleSheet ("QLabel { color: blue; text-decoration: underline; border: none; }");
    requirementsLabel -> setWordWrap(true);
    registerLayout -> addWidget (requirementsLabel);

    
    m_registerButton = new QPushButton("Зарегистрироваться");
    registerLayout->addWidget(m_registerButton);
    
    m_switchToLoginButton = new QPushButton("Уже есть аккаунт? Войти");
    m_switchToLoginButton->setStyleSheet("QPushButton { color: blue; text-decoration: underline; border: none; }");
    registerLayout->addWidget(m_switchToLoginButton);
    
    registerLayout->addStretch();
    
    // ===== СТРАНИЦА ВХОДА =====
    m_loginPage = new QWidget();
    QVBoxLayout *loginLayout = new QVBoxLayout(m_loginPage);
    
    QFormLayout *loginForm = new QFormLayout();
    
    m_loginUsername = new QLineEdit();
    m_loginUsername->setPlaceholderText("Введите имя пользователя");
    loginForm->addRow("Имя пользователя:", m_loginUsername);
    
    m_loginPassword = new QLineEdit();
    m_loginPassword->setPlaceholderText("Введите пароль");
    m_loginPassword->setEchoMode(QLineEdit::Password);
    loginForm->addRow("Пароль:", m_loginPassword);
    
    loginLayout->addLayout(loginForm);
    
    m_loginButton = new QPushButton("Войти");
    loginLayout->addWidget(m_loginButton);
    
    m_switchToRegisterButton = new QPushButton("Нет аккаунта? Зарегистрироваться");
    m_switchToRegisterButton->setStyleSheet("QPushButton { color: blue; text-decoration: underline; border: none; }");
    loginLayout->addWidget(m_switchToRegisterButton);
    
    loginLayout->addStretch();
    
    // Добавляем страницы в stacked widget
    m_stackedWidget->addWidget(m_loginPage);
    m_stackedWidget->addWidget(m_registerPage);
    
    setCentralWidget(m_stackedWidget);
    
    // По умолчанию показываем страницу входа
    m_stackedWidget->setCurrentIndex(0);
}

void AuthWindow::setupConnections()
{
    // Кнопки переключения между страницами
    connect(m_switchToRegisterButton, SIGNAL(clicked()), this, SLOT(switchToRegister()));
    connect(m_switchToLoginButton, SIGNAL(clicked()), this, SLOT(switchToLogin()));
    
    // Кнопки действий
    connect(m_registerButton, SIGNAL(clicked()), this, SLOT(onRegisterClicked()));
    connect(m_loginButton, SIGNAL(clicked()), this, SLOT(onLoginClicked()));
}

void AuthWindow::switchToLogin()
{
    m_stackedWidget->setCurrentIndex(0);
    m_statusBar->showMessage("Страница входа");
}

void AuthWindow::switchToRegister()
{
    m_stackedWidget->setCurrentIndex(1);
    m_statusBar->showMessage("Страница регистрации");
}

void AuthWindow::onRegisterClicked()
{
    QString username = m_registerUsername->text().trimmed();
    QString password = m_registerPassword->text();
    QString confirmPassword = m_registerConfirmPassword->text();
    m_statusBar->showMessage("Условия регистрации:");
    
    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены");
        return;
    }
    
    if (password != confirmPassword) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
        m_registerPassword->clear();
        m_registerConfirmPassword->clear();
        m_registerPassword->setFocus();
        return;
    }
    
    if (username.length() < 3) {
        QMessageBox::warning(this, "Ошибка", "Имя пользователя должно содержать минимум 3 символа");
        m_registerUsername->setFocus();
        return;
    }
    
    if (password.length() < 6) {
        QMessageBox::warning(this, "Ошибка", "Пароль должен содержать минимум 6 символов");
        m_registerPassword->setFocus();
        return;
    }
    
    m_statusBar->showMessage("Регистрация...");
    
    // Регистрация в БД
    if (DatabaseManager::instance().registerUser(username, password)) {
        QMessageBox::information(this, "Успех", "Регистрация прошла успешно!");
        m_statusBar->showMessage("Регистрация успешна");
        
        // Очищаем поля и переключаемся на вход
        m_registerUsername->clear();
        m_registerPassword->clear();
        m_registerConfirmPassword->clear();
        switchToLogin();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось зарегистрировать пользователя. Возможно, имя уже занято.");
        m_statusBar->showMessage("Ошибка регистрации");
    }
}

void AuthWindow::onLoginClicked()
{
    QString username = m_loginUsername->text().trimmed();
    QString password = m_loginPassword->text();
    
    // Валидация
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля");
        return;
    }
    
    m_statusBar->showMessage("Авторизация...");
    
    // Аутентификация в БД
    if (DatabaseManager::instance().authenticateUser(username, password)) {
        QMessageBox::information(this, "Успех", "Вход выполнен успешно!");
        m_statusBar->showMessage("Авторизация успешна");
        
        // Здесь будет переход в основное приложение
        qDebug() << "Пользователь" << username << "вошел в систему";
        
        // Очищаем поля
        m_loginUsername->clear();
        m_loginPassword->clear();
        
    } else {
        QMessageBox::critical(this, "Ошибка", "Неверное имя пользователя или пароль");
        m_statusBar->showMessage("Ошибка авторизации");
        m_loginPassword->clear();
        m_loginPassword->setFocus();
    }
}
