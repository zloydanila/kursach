#include "AuthWindow.h"
#include "../database/DatabaseManager.h"
#include "../MainWindow/MainWindow.h"
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
#include <QRegularExpression>
#include <QDialog>

using namespace std;

AuthWindow::AuthWindow(QWidget *parent)
    : QMainWindow(parent)
    , stackedWidget(nullptr)
    , regPage(nullptr)
    , regEmail(nullptr)
    , regUser(nullptr)
    , regPass(nullptr)
    , regConfirmPass(nullptr)
    , regBtn(nullptr)
    , toLoginBtn(nullptr)
    , loginPage(nullptr)
    , loginUser(nullptr)
    , loginPass(nullptr)
    , loginBtn(nullptr)
    , toRegBtn(nullptr)
{
    setupUI();
    setupConnections();
    
    setWindowTitle("SoundSpace - Авторизация");
    setFixedSize(400, 500);
    
    this->statusBar()->showMessage("Страница входа");
}

AuthWindow::~AuthWindow()
{
}

void AuthWindow::showMessage(const QString& title, const QString& text, bool isError)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setFixedSize(300, 150);
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QLabel *label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    layout->addWidget(label);
    
    QPushButton *okButton = new QPushButton("OK");
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(okButton);
    
    dialog.exec();
}

void AuthWindow::showSqlInjectionError()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Обнаружена SQL инъекция!");
    dialog.setFixedSize(500, 400);
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QLabel *titleLabel = new QLabel("ОБНАРУЖЕНА SQL ИНЪЕКЦИЯ!");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { color: red; font-size: 16px; font-weight: bold; }");
    layout->addWidget(titleLabel);
    
    QLabel *imageLabel = new QLabel();
    QPixmap pixmap("../src/gui/AuthWindow/image.png");
    if (!pixmap.isNull()) {
        imageLabel->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio));
    } else {
        imageLabel->setText("🚫");
        imageLabel->setStyleSheet("QLabel { font-size: 150px; }");
    }
    imageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(imageLabel);
    
    QLabel *textLabel = new QLabel(
        "Обнаружена попытка SQL инъекции!\n\n"
        "Запрещенные символы:\n"
        "• Кавычки (' \")\n"
        "• Точка с запятой (;)\n"
        "• Комментарии (--, /* */)\n"
        "• Ключевые слова SQL (SELECT, INSERT, DROP и т.д.)\n\n"
        "Пожалуйста, используйте только буквы, цифры и стандартные символы."
    );
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setWordWrap(true);
    textLabel->setStyleSheet("QLabel { font-size: 12px; color: #333; }");
    layout->addWidget(textLabel);
    
    QPushButton *okButton = new QPushButton("Понятно");
    okButton->setStyleSheet("QPushButton { background-color: #ff4444; color: white; padding: 8px; border-radius: 4px; }");
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(okButton);
    
    dialog.exec();
}

void AuthWindow::setupUI()
{
    stackedWidget = new QStackedWidget(this);
    
    regPage = new QWidget();
    QVBoxLayout *regLayout = new QVBoxLayout(regPage);
    
    QFormLayout *regForm = new QFormLayout();
    
    regEmail = new QLineEdit();
    regEmail->setPlaceholderText("example@gmail.com");
    regForm->addRow("Email:", regEmail);

    regUser = new QLineEdit();
    regUser->setPlaceholderText("Введите имя пользователя");
    regForm->addRow("Имя пользователя:", regUser);
    
    regPass = new QLineEdit();
    regPass->setPlaceholderText("Введите пароль");
    regPass->setEchoMode(QLineEdit::Password);
    regForm->addRow("Пароль:", regPass);
    
    regConfirmPass = new QLineEdit();
    regConfirmPass->setPlaceholderText("Повторите пароль");
    regConfirmPass->setEchoMode(QLineEdit::Password);
    regForm->addRow("Подтверждение:", regConfirmPass);

    regLayout->addLayout(regForm);

    QLabel *reqLabel = new QLabel();
    reqLabel->setText(
        "Требования для регистрации:\n"
        "• Email должен быть действительным\n"
        "• Имя пользователя: минимум 3 символа\n"  
        "• Пароль: минимум 6 символов\n"
        "• Пароли должны совпадать"
    );
    reqLabel->setStyleSheet("QLabel { color: #666; font-size: 10px; background-color: #f9f9f9; padding: 8px; border-radius: 4px; }");
    reqLabel->setWordWrap(true);
    regLayout->addWidget(reqLabel);

    regBtn = new QPushButton("Зарегистрироваться");
    regLayout->addWidget(regBtn);
    
    toLoginBtn = new QPushButton("Уже есть аккаунт? Войти");
    toLoginBtn->setStyleSheet("QPushButton { color: blue; text-decoration: underline; border: none; background: transparent; }");
    regLayout->addWidget(toLoginBtn);
    
    regLayout->addStretch();
    
    loginPage = new QWidget();
    QVBoxLayout *loginLayout = new QVBoxLayout(loginPage);
    
    QFormLayout *loginForm = new QFormLayout();
    
    loginUser = new QLineEdit();
    loginUser->setPlaceholderText("Введите имя пользователя");
    loginForm->addRow("Имя пользователя:", loginUser);
    
    loginPass = new QLineEdit();
    loginPass->setPlaceholderText("Введите пароль");
    loginPass->setEchoMode(QLineEdit::Password);
    loginForm->addRow("Пароль:", loginPass);
    
    loginLayout->addLayout(loginForm);
    
    loginBtn = new QPushButton("Войти");
    loginLayout->addWidget(loginBtn);
    
    toRegBtn = new QPushButton("Нет аккаунта? Зарегистрироваться");
    toRegBtn->setStyleSheet("QPushButton { color: blue; text-decoration: underline; border: none; background: transparent; }");
    loginLayout->addWidget(toRegBtn);
    
    loginLayout->addStretch();
    
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(regPage);
    
    setCentralWidget(stackedWidget);
    
    stackedWidget->setCurrentIndex(0);
}

void AuthWindow::setupConnections()
{
    connect(toRegBtn, SIGNAL(clicked()), this, SLOT(switchToRegister()));
    connect(toLoginBtn, SIGNAL(clicked()), this, SLOT(switchToLogin()));
    
    connect(regBtn, SIGNAL(clicked()), this, SLOT(onRegisterClicked()));
    connect(loginBtn, SIGNAL(clicked()), this, SLOT(onLoginClicked()));
}

void AuthWindow::switchToLogin()
{
    stackedWidget->setCurrentIndex(0);
    this->statusBar()->showMessage("Страница входа");
}

void AuthWindow::switchToRegister()
{
    stackedWidget->setCurrentIndex(1);
    this->statusBar()->showMessage("Страница регистрации");
}

bool AuthWindow::isValidEmail(const QString& email)
{
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return emailRegex.match(email).hasMatch();
}

bool AuthWindow::isValidUsername(const QString& username)
{
    QRegularExpression userRegex("^[a-zA-Z0-9_]+$");
    return username.length() >= 3 && userRegex.match(username).hasMatch();
}

bool AuthWindow::isStrongPassword(const QString& password)
{
    if (password.length() < 6) return false;
    
    bool hasLetter = false;
    bool hasDigit = false;
    
    for (const QChar& ch : password) {
        if (ch.isLetter()) hasLetter = true;
        if (ch.isDigit()) hasDigit = true;
    }
    
    return hasLetter && hasDigit;
}

bool AuthWindow::hasSqlInjection(const QString& input)
{
    QStringList sqlKeywords = {"SELECT", "INSERT", "UPDATE", "DELETE", "DROP", "CREATE", 
                              "ALTER", "EXEC", "UNION", "WHERE", "FROM", "TABLE"};
    
    QString upperInput = input.toUpper();
    
    for (const QString& keyword : sqlKeywords) {
        if (upperInput.contains(keyword)) {
            return true;
        }
    }
    
    if (input.contains("'") || input.contains("\"") || input.contains(";") || 
        input.contains("--") || input.contains("/*") || input.contains("*/")) {
        return true;
    }
    
    return false;
}

void AuthWindow::onRegisterClicked()
{
    QString email = regEmail->text().trimmed();
    QString username = regUser->text().trimmed();
    QString password = regPass->text();
    QString confirmPass = regConfirmPass->text();

    if (hasSqlInjection(email) || hasSqlInjection(username) || hasSqlInjection(password)) {
        showSqlInjectionError();
        return;
    }
    
    if (email.isEmpty() || username.isEmpty() || password.isEmpty() || confirmPass.isEmpty()) {
        showMessage("Ошибка", "Все поля должны быть заполнены!");
        return;
    }
    
    if (!isValidEmail(email)) {
        showMessage("Ошибка", 
            "Некорректный формат email!\n"
            "Пример: example@gmail.com");
        regEmail->setFocus();
        regEmail->selectAll();
        return;
    }
    
    if (!isValidUsername(username)) {
        showMessage("Ошибка",
            "Некорректное имя пользователя!\n"
            "• Минимум 3 символа\n"
            "• Только буквы, цифры и подчеркивания");
        regUser->setFocus();
        regUser->selectAll();
        return;
    }
    
    if (password != confirmPass) {
        showMessage("Ошибка", "Пароли не совпадают!");
        regPass->clear();
        regConfirmPass->clear();
        regPass->setFocus();
        return;
    }
    
    if (!isStrongPassword(password)) {
        showMessage("Ошибка",
            "Слабый пароль!\n"
            "• Минимум 6 символов\n"
            "• Должен содержать буквы и цифры");
        regPass->clear();
        regConfirmPass->clear();
        regPass->setFocus();
        return;
    }
    
    QStringList weakPass = {"123456", "password", "qwerty", "111111", "admin123"};
    if (weakPass.contains(password.toLower())) {
        showMessage("Предупреждение", 
            "Этот пароль слишком простой!\n"
            "Выберите более сложный пароль.");
        regPass->setFocus();
        return;
    }
    
    if (password.count(password[0]) == password.length()) {
        showMessage("Ошибка", "Пароль не может состоять из одинаковых символов!");
        regPass->clear();
        regConfirmPass->clear();
        regPass->setFocus();
        return;
    }
    
    this->statusBar()->showMessage("Регистрация...");
    
    if (DatabaseManager::instance().registerUser(username, password)) {
        showMessage("Успех", 
            "Регистрация прошла успешно!\n\n"
            "Email: " + email + "\n" +
            "Имя пользователя: " + username, false);
        this->statusBar()->showMessage("Регистрация успешна");
        
        regEmail->clear();
        regUser->clear();
        regPass->clear();
        regConfirmPass->clear();
        switchToLogin();
    } else {
        showMessage("Ошибка", 
            "Не удалось зарегистрировать!\n"
            "• Имя пользователя уже занято\n"
            "• Проблема с базой данных");
        this->statusBar()->showMessage("Ошибка регистрации");
    }
}

void AuthWindow::onLoginClicked()
{
    QString username = loginUser->text().trimmed();
    QString password = loginPass->text();
    
    if (hasSqlInjection(username) || hasSqlInjection(password)) {
        showSqlInjectionError();
        return;
    }
    
    if (username.isEmpty() || password.isEmpty()) {
        showMessage("Ошибка", "Заполните все поля!");
        return;
    }
    
    if (username.length() > 50 || password.length() > 100) {
        showMessage("Ошибка", "Слишком длинные данные!");
        loginUser->clear();
        loginPass->clear();
        loginUser->setFocus();
        return;
    }
    
    this->statusBar()->showMessage("Авторизация...");
    
    if (DatabaseManager::instance().authenticateUser(username, password)) {
        showMessage("Успех", "Вход выполнен успешно!", false);
        this->statusBar()->showMessage("Авторизация успешна");
        
        qDebug() << "Пользователь" << username << "вошел в систему";

        showMainWindow(username);
        
    } else {
        showMessage("Ошибка", "Неверное имя пользователя или пароль!");
        this->statusBar()->showMessage("Ошибка авторизации");
        loginPass->clear();
        loginPass->setFocus();
    }
}

void AuthWindow::showMainWindow(const QString& username)
{
    // Получаем ID пользователя из БД
    int userId = DatabaseManager::instance().getUserId(username);
    
    if (userId == -1) {
        qDebug() << "❌ Ошибка: не удалось получить ID пользователя" << username;
        QMessageBox::warning(this, "Ошибка", "Не удалось получить данные пользователя");
        return;
    }
    
    qDebug() << "✅ Открываем главное окно для пользователя:" << username << "(ID:" << userId << ")";
    
    // Создаем главное окно
    MainWindow *mainWindow = new MainWindow(username, userId);
    mainWindow->show();
    
    // Закрываем окно авторизации
    this->close();
}