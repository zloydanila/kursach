#include "LoginForm.h"
#include "database/DatabaseManager.h"
#include "gui/dialogs/NotificationDialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDialog>
#include <QHBoxLayout>
#include <QPixmap>
#include <QRegularExpression>
#include <QFileInfo>
#include <QCoreApplication>
#include <QGraphicsDropShadowEffect>

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

static bool looksLikeSqlInjection(const QString& s)
{
    const QString t = s.toLower();

    if (t.contains("--") || t.contains("/*") || t.contains("*/") || t.contains(";")) return true;

    QRegularExpression re1(R"((\bor\b|\band\b)\s+['"]?\w+['"]?\s*=\s*['"]?\w+['"]?)", QRegularExpression::CaseInsensitiveOption);
    if (re1.match(s).hasMatch()) return true;

    QRegularExpression re2(R"(\bunion\b\s+\bselect\b)", QRegularExpression::CaseInsensitiveOption);
    if (re2.match(s).hasMatch()) return true;

    QRegularExpression re3(R"(\bselect\b|\binsert\b|\bupdate\b|\bdelete\b|\bdrop\b)", QRegularExpression::CaseInsensitiveOption);
    if (re3.match(s).hasMatch()) return true;

    if (t.contains("'") || t.contains("\"")) return true;

    return false;
}

static void showInjectionDialog(QWidget *parent)
{
    QDialog dlg(parent);
    dlg.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dlg.setAttribute(Qt::WA_TranslucentBackground);
    dlg.setModal(true);
    dlg.resize(520, 420);

    QVBoxLayout *root = new QVBoxLayout(&dlg);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    QWidget *bg = new QWidget();
    bg->setStyleSheet(R"(
        QWidget {
            background: #0F0F14;
            border-radius: 18px;
            border: none;
        }
    )");

    QVBoxLayout *lay = new QVBoxLayout(bg);
    lay->setContentsMargins(22, 20, 22, 18);
    lay->setSpacing(14);

    QLabel *title = new QLabel("Предупреждение безопасности");
    title->setStyleSheet(R"(
        QLabel {
            color: rgba(255,255,255,0.95);
            font-size: 16px;
            font-weight: 800;
        }
    )");

    QLabel *img = new QLabel();
    img->setAlignment(Qt::AlignCenter);
    img->setMinimumHeight(180);
    img->setStyleSheet("background: transparent;");

    QString path = "src/gui/AuthWindow/image.png";
    if (!QFileInfo::exists(path)) path = QCoreApplication::applicationDirPath() + "/src/gui/AuthWindow/image.png";

    QPixmap pm(path);
    if (!pm.isNull()) img->setPixmap(pm.scaled(460, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QLabel *text = new QLabel("Обнаружена попытка SQL-инъекции.\nДействие заблокировано.");
    text->setAlignment(Qt::AlignCenter);
    text->setWordWrap(true);
    text->setTextFormat(Qt::PlainText);
    text->setStyleSheet(R"(
        QLabel {
            color: rgba(255,255,255,0.86);
            font-size: 14px;
        }
    )");

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setContentsMargins(0, 0, 0, 0);
    btnRow->setSpacing(10);

    QPushButton *ok = new QPushButton("OK");
    ok->setFixedHeight(36);
    ok->setMinimumWidth(100);
    ok->setCursor(Qt::PointingHandCursor);
    ok->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 800;
        }
        QPushButton:hover { background: #9B4BFF; }
        QPushButton:pressed { background: #7B22CC; }
    )");
    QObject::connect(ok, &QPushButton::clicked, &dlg, &QDialog::accept);

    btnRow->addStretch();
    btnRow->addWidget(ok);

    lay->addWidget(title);
    lay->addWidget(img);
    lay->addWidget(text);
    lay->addLayout(btnRow);

    root->addWidget(bg);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(26);
    shadow->setColor(QColor(0, 0, 0, 140));
    shadow->setOffset(0, 10);
    bg->setGraphicsEffect(shadow);

    dlg.exec();
}

bool LoginForm::validateInput()
{
    if (m_usernameInput->text().trimmed().isEmpty()) {
        NotificationDialog dialog("Введите имя пользователя", NotificationDialog::Error, this);
        dialog.exec();
        return false;
    }

    if (m_passwordInput->text().isEmpty()) {
        NotificationDialog dialog("Введите пароль", NotificationDialog::Error, this);
        dialog.exec();
        return false;
    }

    return true;
}

void LoginForm::onLoginClicked()
{
    if (!validateInput()) return;

    QString username = m_usernameInput->text().trimmed();
    QString password = m_passwordInput->text();

    if (looksLikeSqlInjection(username) || looksLikeSqlInjection(password)) {
        showInjectionDialog(this);
        return;
    }

    if (DatabaseManager::instance().authenticateUser(username, password)) {
        int userId = DatabaseManager::instance().getUserId(username);
        emit loginSuccess(username, userId);
    } else {
        NotificationDialog dialog("Неверное имя пользователя или пароль", NotificationDialog::Error, this);
        dialog.exec();
    }
}
