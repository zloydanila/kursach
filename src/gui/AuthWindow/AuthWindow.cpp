#include "AuthWindow.h"
#include "../MainWindow/MainWindow.h"
#include "../../database/DatabaseManager.h"

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
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEasingCurve>

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
    , titleBarWidget(nullptr)
    , m_dragging(false)
{
    setupUI();
    setupConnections();
    
    setWindowTitle("SoundSpace");
    setFixedSize(500, 650);
    setWindowFlags(Qt::FramelessWindowHint);
}

AuthWindow::~AuthWindow()
{
}

bool AuthWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (titleBarWidget && obj == titleBarWidget) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                m_dragging = true;
                m_dragPosition = mouseEvent->globalPos() - frameGeometry().topLeft();
                return true;
            }
        } else if (event->type() == QEvent::MouseMove && m_dragging) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            move(mouseEvent->globalPos() - m_dragPosition);
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_dragging = false;
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void AuthWindow::showMessage(const QString& title, const QString& text, bool isError)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    
    // Устанавливаем темный стиль
    msgBox.setStyleSheet(R"(
        QMessageBox {
            background-color: #1A1A21;
            color: white;
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
        }
        QMessageBox QLabel {
            color: white;
            font-size: 14px;
        }
        QMessageBox QPushButton {
            background-color: #8A2BE2;
            color: white;
            border: none;
            padding: 8px 20px;
            border-radius: 6px;
            font-weight: 500;
            min-width: 80px;
        }
        QMessageBox QPushButton:hover {
            background-color: #9B4BFF;
        }
    )");
    
    if (isError) {
        msgBox.setIcon(QMessageBox::Critical);
    } else {
        msgBox.setIcon(QMessageBox::Information);
    }
    
    msgBox.exec();
}

void AuthWindow::showSqlInjectionError()
{
    QDialog dialog(this);
    dialog.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    dialog.setAttribute(Qt::WA_TranslucentBackground);
    dialog.setFixedSize(500, 500); // Увеличили высоту для изображения
    
    QWidget *content = new QWidget(&dialog);
    content->setStyleSheet(R"(
        QWidget {
            background: #1A1A21;
            border-radius: 15px;
            border: 1px solid rgba(255, 255, 255, 0.1);
        }
    )");
    
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);
    
    QLabel *titleLabel = new QLabel("ОБНАРУЖЕНА SQL ИНЪЕКЦИЯ!");
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #FF4444;
            font-size: 18px;
            font-weight: bold;
            text-align: center;
        }
    )");
    
    // Добавляем изображение
    QLabel *imageLabel = new QLabel();
    QPixmap pixmap("image.png");
    if (pixmap.isNull()) {
        // Если файл не найден, пробуем из ресурсов
        pixmap.load(":/images/image.png");
    }
    
    if (!pixmap.isNull()) {
        pixmap = pixmap.scaled(200, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(pixmap);
        imageLabel->setAlignment(Qt::AlignCenter);
    } else {
        imageLabel->setText("Изображение не найдено");
        imageLabel->setStyleSheet("color: white; text-align: center;");
    }
    
    QLabel *textLabel = new QLabel(
        "Обнаружена попытка SQL инъекции!\n\n"
        "Запрещенные символы:\n"
        "• Кавычки (' \")\n"
        "• Точка с запятой (;)\n"
        "• Комментарии (--, /* */)\n"
        "• Ключевые слова SQL\n\n"
        "Пожалуйста, используйте только буквы, цифры и стандартные символы."
    );
    textLabel->setStyleSheet(R"(
        QLabel {
            color: rgba(255, 255, 255, 0.8);
            font-size: 13px;
            text-align: center;
        }
    )");
    textLabel->setWordWrap(true);
    
    QPushButton *okButton = new QPushButton("ПОНЯТНО");
    okButton->setStyleSheet(R"(
        QPushButton {
            background-color: #FF4444;
            color: white;
            border: none;
            padding: 12px 40px;
            border-radius: 8px;
            font-weight: bold;
            font-size: 14px;
            margin-top: 10px;
        }
        QPushButton:hover {
            background-color: #CC3333;
        }
    )");
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    
    layout->addWidget(titleLabel);
    layout->addWidget(imageLabel);
    layout->addWidget(textLabel);
    layout->addWidget(okButton, 0, Qt::AlignCenter);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(content);
    
    dialog.exec();
}

void AuthWindow::setupUI()
{
    // Основной виджет
    QWidget *central = new QWidget();
    central->setObjectName("centralWidget");
    central->setStyleSheet(R"(
        #centralWidget {
            background: #0F0F14;
            border-radius: 20px;
        }
    )");
    
    setCentralWidget(central);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Панель заголовка для перемещения окна
    central->setMouseTracking(true);
    central->installEventFilter(this);
    
    QWidget *titleBarWidget = new QWidget();
    titleBarWidget->setFixedHeight(50);
    titleBarWidget->setStyleSheet(R"(
        QWidget {
            background: transparent;
            border-top-left-radius: 20px;
            border-top-right-radius: 20px;
        }
    )");
    
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBarWidget);
    titleLayout->setContentsMargins(20, 0, 20, 0);
    
    QLabel *logo = new QLabel("SoundSpace");
    logo->setStyleSheet(R"(
        QLabel {
            color: #8A2BE2;
            font-size: 20px;
            font-weight: 800;
            background: transparent;
        }
    )");
    
    QWidget *windowButtons = new QWidget();
    QHBoxLayout *buttonsLayout = new QHBoxLayout(windowButtons);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(5);
    
    QPushButton *minimizeBtn = new QPushButton("—");
    QPushButton *closeBtn = new QPushButton("×");
    
    QString windowBtnStyle = R"(
        QPushButton {
            background: transparent;
            color: rgba(255, 255, 255, 0.6);
            border: none;
            font-size: 20px;
            padding: 0;
            min-width: 30px;
            min-height: 30px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.1);
            color: white;
        }
        QPushButton#closeBtn:hover {
            background: #FF4444;
            color: white;
        }
    )";
    
    minimizeBtn->setStyleSheet(windowBtnStyle);
    closeBtn->setStyleSheet(windowBtnStyle);
    closeBtn->setObjectName("closeBtn");
    
    buttonsLayout->addWidget(minimizeBtn);
    buttonsLayout->addWidget(closeBtn);
    
    titleLayout->addWidget(logo);
    titleLayout->addStretch();
    titleLayout->addWidget(windowButtons);
    
    // Контентная область
    QWidget *contentWidget = new QWidget();
    contentWidget->setStyleSheet("background: transparent;");
    
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(40, 20, 40, 40);
    contentLayout->setSpacing(30);
    
    stackedWidget = new QStackedWidget();
    stackedWidget->setMaximumWidth(400);
    stackedWidget->setMinimumHeight(400);
    
    // === Страница ЛОГИНА ===
    loginPage = new QWidget();
    loginPage->setObjectName("loginPage");
    QVBoxLayout *loginLayout = new QVBoxLayout(loginPage);
    loginLayout->setContentsMargins(0, 0, 0, 0);
    loginLayout->setSpacing(25);
    
    QLabel *loginTitle = new QLabel("Вход в аккаунт");
    loginTitle->setStyleSheet(R"(
        QLabel {
            color: white;
            font-size: 24px;
            font-weight: bold;
            text-align: center;
            background: transparent;
            border: none;
        }
    )");
    
    QWidget *formWidget = new QWidget();
    formWidget->setObjectName("formWidget");
    QVBoxLayout *formLayout = new QVBoxLayout(formWidget);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->setSpacing(20);
    
    loginUser = new QLineEdit();
    loginUser->setObjectName("loginUser");
    loginUser->setPlaceholderText("Имя пользователя");
    loginUser->setMinimumHeight(45);
    loginUser->setStyleSheet(R"(
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
            background: rgba(138, 43, 226, 0.05);
        }
        QLineEdit::placeholder {
            color: rgba(255, 255, 255, 0.4);
        }
    )");
    
    loginPass = new QLineEdit();
    loginPass->setObjectName("loginPass");
    loginPass->setPlaceholderText("Пароль");
    loginPass->setEchoMode(QLineEdit::Password);
    loginPass->setMinimumHeight(45);
    loginPass->setStyleSheet(loginUser->styleSheet());
    
    formLayout->addWidget(loginUser);
    formLayout->addWidget(loginPass);
    
    loginBtn = new QPushButton("ВОЙТИ");
    loginBtn->setObjectName("loginBtn");
    loginBtn->setMinimumHeight(45);
    loginBtn->setStyleSheet(R"(
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
            transform: translateY(-1px);
        }
        QPushButton:pressed {
            background: #7B1FA2;
            transform: translateY(0px);
        }
    )");
    
    toRegBtn = new QPushButton("Создать новый аккаунт");
    toRegBtn->setObjectName("toRegBtn");
    toRegBtn->setStyleSheet(R"(
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
    
    loginLayout->addWidget(loginTitle);
    loginLayout->addWidget(formWidget);
    loginLayout->addWidget(loginBtn);
    loginLayout->addWidget(toRegBtn);
    loginLayout->addStretch();
    
    // === Страница РЕГИСТРАЦИИ ===
    regPage = new QWidget();
    regPage->setObjectName("regPage");
    QVBoxLayout *regLayout = new QVBoxLayout(regPage);
    regLayout->setContentsMargins(0, 0, 0, 0);
    regLayout->setSpacing(25);
    
    QLabel *regTitle = new QLabel("Создать аккаунт");
    regTitle->setStyleSheet(loginTitle->styleSheet());
    
    QWidget *regFormWidget = new QWidget();
    regFormWidget->setObjectName("regFormWidget");
    QVBoxLayout *regFormLayout = new QVBoxLayout(regFormWidget);
    regFormLayout->setContentsMargins(0, 0, 0, 0);
    regFormLayout->setSpacing(15);
    
    regEmail = new QLineEdit();
    regEmail->setObjectName("regEmail");
    regEmail->setPlaceholderText("Email адрес");
    regEmail->setMinimumHeight(40);
    regEmail->setStyleSheet(loginUser->styleSheet());
    
    regUser = new QLineEdit();
    regUser->setObjectName("regUser");
    regUser->setPlaceholderText("Имя пользователя");
    regUser->setMinimumHeight(40);
    regUser->setStyleSheet(loginUser->styleSheet());
    
    regPass = new QLineEdit();
    regPass->setObjectName("regPass");
    regPass->setPlaceholderText("Пароль");
    regPass->setEchoMode(QLineEdit::Password);
    regPass->setMinimumHeight(40);
    regPass->setStyleSheet(loginUser->styleSheet());
    
    regConfirmPass = new QLineEdit();
    regConfirmPass->setObjectName("regConfirmPass");
    regConfirmPass->setPlaceholderText("Подтвердите пароль");
    regConfirmPass->setEchoMode(QLineEdit::Password);
    regConfirmPass->setMinimumHeight(40);
    regConfirmPass->setStyleSheet(loginUser->styleSheet());
    
    QLabel *passwordRules = new QLabel(
        "✓ Минимум 6 символов\n"
        "✓ Буквы и цифры\n"
        "✓ Без специальных символов"
    );
    passwordRules->setStyleSheet(R"(
        QLabel {
            color: rgba(255, 255, 255, 0.5);
            font-size: 12px;
            background: rgba(255, 255, 255, 0.05);
            padding: 10px;
            border-radius: 8px;
            border: none;
        }
    )");
    passwordRules->setWordWrap(true);
    
    regFormLayout->addWidget(regEmail);
    regFormLayout->addWidget(regUser);
    regFormLayout->addWidget(regPass);
    regFormLayout->addWidget(regConfirmPass);
    regFormLayout->addWidget(passwordRules);
    
    regBtn = new QPushButton("ЗАРЕГИСТРИРОВАТЬСЯ");
    regBtn->setObjectName("regBtn");
    regBtn->setMinimumHeight(45);
    regBtn->setStyleSheet(loginBtn->styleSheet());
    
    toLoginBtn = new QPushButton("Уже есть аккаунт? Войти");
    toLoginBtn->setObjectName("toLoginBtn");
    toLoginBtn->setStyleSheet(toRegBtn->styleSheet());
    
    regLayout->addWidget(regTitle);
    regLayout->addWidget(regFormWidget);
    regLayout->addWidget(regBtn);
    regLayout->addWidget(toLoginBtn);
    regLayout->addStretch();
    
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(regPage);
    
    contentLayout->addWidget(stackedWidget, 0, Qt::AlignCenter);
    
    mainLayout->addWidget(titleBarWidget);
    mainLayout->addWidget(contentWidget, 1);
    
    // Подключаем кнопки управления окном
    connect(minimizeBtn, &QPushButton::clicked, this, &QMainWindow::showMinimized);
    connect(closeBtn, &QPushButton::clicked, this, &QMainWindow::close);
    
    // Устанавливаем event filter для перемещения окна
    central->installEventFilter(this);
    
    stackedWidget->setCurrentIndex(0);
}

void AuthWindow::setupConnections()
{
    connect(toRegBtn, &QPushButton::clicked, this, &AuthWindow::switchToRegister);
    connect(toLoginBtn, &QPushButton::clicked, this, &AuthWindow::switchToLogin);
    
    connect(regBtn, &QPushButton::clicked, this, &AuthWindow::onRegisterClicked);
    connect(loginBtn, &QPushButton::clicked, this, &AuthWindow::onLoginClicked);
}

void AuthWindow::switchToLogin()
{
    stackedWidget->setCurrentIndex(0);
}

void AuthWindow::switchToRegister()
{
    stackedWidget->setCurrentIndex(1);
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
        showMessage("Ошибка", "Некорректный формат email!");
        return;
    }
    
    if (!isValidUsername(username)) {
        showMessage("Ошибка", "Имя пользователя должно содержать минимум 3 символа (только буквы, цифры и подчеркивания)!");
        return;
    }
    
    if (password != confirmPass) {
        showMessage("Ошибка", "Пароли не совпадают!");
        return;
    }
    
    if (!isStrongPassword(password)) {
        showMessage("Ошибка", "Пароль должен содержать минимум 6 символов, буквы и цифры!");
        return;
    }
    
    regBtn->setEnabled(false);
    regBtn->setText("Регистрация...");
    
    QTimer::singleShot(500, [this, username, password]() {
        if (DatabaseManager::instance().registerUser(username, password)) {
            showMessage("Успех", "Регистрация прошла успешно!", false);
            switchToLogin();
        } else {
            showMessage("Ошибка", "Имя пользователя уже занято!");
        }
        
        regBtn->setEnabled(true);
        regBtn->setText("ЗАРЕГИСТРИРОВАТЬСЯ");
    });
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
    
    loginBtn->setEnabled(false);
    loginBtn->setText("Вход...");
    
    QTimer::singleShot(500, [this, username, password]() {
        if (DatabaseManager::instance().authenticateUser(username, password)) {
            showMainWindow(username);
        } else {
            showMessage("Ошибка", "Неверное имя пользователя или пароль!");
            loginBtn->setEnabled(true);
            loginBtn->setText("ВОЙТИ");
        }
    });
}

void AuthWindow::showMainWindow(const QString& username)
{
    int userId = DatabaseManager::instance().getUserId(username);
    
    if (userId == -1) {
        showMessage("Ошибка", "Не удалось получить данные пользователя");
        return;
    }
    
    MainWindow *mainWindow = new MainWindow(username, userId);
    mainWindow->show();
    
    this->close();
}