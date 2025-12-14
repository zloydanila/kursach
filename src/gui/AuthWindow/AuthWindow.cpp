#include "AuthWindow.h"
#include "gui/AuthWindow/widgets/LoginForm.h"
#include "gui/AuthWindow/widgets/RegisterForm.h"
#include "gui/MainWindow/MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>

AuthWindow::AuthWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(new QStackedWidget(this))
    , m_dragging(false)
{
    setupUI();
    
    setWindowTitle("Chorus");
    setFixedSize(500, 650);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
}

AuthWindow::~AuthWindow()
{
}

void AuthWindow::setupUI()
{
    QWidget* central = new QWidget();
    central->setObjectName("centralWidget");
    central->setStyleSheet(R"(
        #centralWidget {
            background: #0F0F14;
            border-radius: 20px;
        }
    )");
    
    setCentralWidget(central);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_titleBarWidget = new QWidget();
    m_titleBarWidget->setFixedHeight(50);
    m_titleBarWidget->setStyleSheet("background: transparent;");
    m_titleBarWidget->installEventFilter(this);
    
    QHBoxLayout* titleLayout = new QHBoxLayout(m_titleBarWidget);
    titleLayout->setContentsMargins(20, 0, 20, 0);
    
    QLabel* logo = new QLabel("Chorus");
    logo->setStyleSheet("color: #8A2BE2; font-size: 20px; font-weight: 800;");
    
    QWidget* windowButtons = new QWidget();
    QHBoxLayout* buttonsLayout = new QHBoxLayout(windowButtons);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(5);
    
    QPushButton* minimizeBtn = new QPushButton("—");
    QPushButton* closeBtn = new QPushButton("×");
    
    QString windowBtnStyle = R"(
        QPushButton {
            background: transparent;
            color: rgba(255, 255, 255, 0.6);
            border: none;
            font-size: 20px;
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
    
    connect(minimizeBtn, &QPushButton::clicked, this, &QMainWindow::showMinimized);
    connect(closeBtn, &QPushButton::clicked, this, &QMainWindow::close);
    
    QWidget* contentWidget = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(40, 20, 40, 40);
    
    m_loginForm = new LoginForm();
    m_registerForm = new RegisterForm();
    
    m_stackedWidget->addWidget(m_loginForm);
    m_stackedWidget->addWidget(m_registerForm);
    
    contentLayout->addWidget(m_stackedWidget);
    
    mainLayout->addWidget(m_titleBarWidget);
    mainLayout->addWidget(contentWidget, 1);
    
    connect(m_loginForm, &LoginForm::loginSuccess, this, &AuthWindow::onLoginSuccess);
    connect(m_loginForm, &LoginForm::switchToRegister, this, &AuthWindow::switchToRegister);
    connect(m_registerForm, &RegisterForm::registerSuccess, this, &AuthWindow::onRegisterSuccess);
    connect(m_registerForm, &RegisterForm::switchToLogin, this, &AuthWindow::switchToLogin);
}

bool AuthWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_titleBarWidget) {
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

void AuthWindow::switchToLogin()
{
    m_stackedWidget->setCurrentIndex(0);
}

void AuthWindow::switchToRegister()
{
    m_stackedWidget->setCurrentIndex(1);
}

void AuthWindow::onLoginSuccess(const QString& username, int userId)
{
    showMainWindow(username, userId);
}

void AuthWindow::onRegisterSuccess()
{
    switchToLogin();
}

void AuthWindow::showMainWindow(const QString& username, int userId)
{
    MainWindow* mainWindow = new MainWindow(username, userId);
    mainWindow->show();
    this->close();
}
