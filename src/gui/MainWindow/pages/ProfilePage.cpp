#include "ProfilePage.h"
#include "core/models/Track.h"
#include "core/managers/UserManager.h"
#include "database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QPainter>
#include <QPainterPath>

ProfilePage::ProfilePage(int userId, QWidget *parent)
    : QWidget(parent)
    , m_userId(userId)
    , m_userManager(new UserManager(this))
{
    setupUI();
    loadUserData();
}

ProfilePage::~ProfilePage()
{
}

void ProfilePage::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(30);
    
    QLabel* titleLabel = new QLabel("Профиль");
    titleLabel->setStyleSheet("color: white; font-size: 32px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);
    
    QWidget* contentWidget = new QWidget();
    contentWidget->setMaximumWidth(600);
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(25);
    
    QWidget* avatarSection = new QWidget();
    QVBoxLayout* avatarLayout = new QVBoxLayout(avatarSection);
    avatarLayout->setAlignment(Qt::AlignCenter);
    avatarLayout->setSpacing(15);
    
    m_avatarLabel = new QLabel();
    m_avatarLabel->setFixedSize(150, 150);
    m_avatarLabel->setStyleSheet(R"(
        border: 3px solid #8A2BE2;
        border-radius: 75px;
        background: rgba(138, 43, 226, 0.1);
    )");
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    
    m_changeAvatarButton = new QPushButton("Изменить аватар");
    m_changeAvatarButton->setFixedWidth(180);
    m_changeAvatarButton->setStyleSheet(R"(
        QPushButton {
            background: rgba(138, 43, 226, 0.2);
            border: 1px solid #8A2BE2;
            color: #8A2BE2;
            padding: 10px 20px;
            border-radius: 8px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: rgba(138, 43, 226, 0.3);
        }
    )");
    
    avatarLayout->addWidget(m_avatarLabel);
    avatarLayout->addWidget(m_changeAvatarButton);
    
    contentLayout->addWidget(avatarSection);
    
    QWidget* formWidget = new QWidget();
    QVBoxLayout* formLayout = new QVBoxLayout(formWidget);
    formLayout->setSpacing(15);
    
    QLabel* usernameLabel = new QLabel("Имя пользователя");
    usernameLabel->setStyleSheet("color: rgba(255, 255, 255, 0.7); font-size: 14px;");
    
    m_usernameInput = new QLineEdit();
    m_usernameInput->setReadOnly(true);
    m_usernameInput->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 12px 15px;
            color: white;
            font-size: 14px;
        }
    )");
    
    QLabel* emailLabel = new QLabel("Email");
    emailLabel->setStyleSheet("color: rgba(255, 255, 255, 0.7); font-size: 14px;");
    
    m_emailInput = new QLineEdit();
    m_emailInput->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 12px 15px;
            color: white;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 2px solid #8A2BE2;
        }
    )");
    
    QLabel* bioLabel = new QLabel("О себе");
    bioLabel->setStyleSheet("color: rgba(255, 255, 255, 0.7); font-size: 14px;");
    
    m_bioInput = new QTextEdit();
    m_bioInput->setMaximumHeight(120);
    m_bioInput->setStyleSheet(R"(
        QTextEdit {
            background: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 12px 15px;
            color: white;
            font-size: 14px;
        }
        QTextEdit:focus {
            border: 2px solid #8A2BE2;
        }
    )");
    
    formLayout->addWidget(usernameLabel);
    formLayout->addWidget(m_usernameInput);
    formLayout->addWidget(emailLabel);
    formLayout->addWidget(m_emailInput);
    formLayout->addWidget(bioLabel);
    formLayout->addWidget(m_bioInput);
    
    contentLayout->addWidget(formWidget);
    
    m_statsLabel = new QLabel();
    m_statsLabel->setStyleSheet(R"(
        background: rgba(138, 43, 226, 0.1);
        border: 1px solid rgba(138, 43, 226, 0.3);
        border-radius: 10px;
        padding: 15px;
        color: white;
        font-size: 13px;
    )");
    contentLayout->addWidget(m_statsLabel);
    
    QWidget* buttonsWidget = new QWidget();
    QHBoxLayout* buttonsLayout = new QHBoxLayout(buttonsWidget);
    buttonsLayout->setSpacing(15);
    
    m_saveButton = new QPushButton("Сохранить изменения");
    m_saveButton->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 10px;
            padding: 12px 30px;
            font-weight: 600;
            font-size: 14px;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
    )");
    
    m_changePasswordButton = new QPushButton("Изменить пароль");
    m_changePasswordButton->setStyleSheet(R"(
        QPushButton {
            background: rgba(255, 255, 255, 0.1);
            color: white;
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 10px;
            padding: 12px 30px;
            font-weight: 600;
            font-size: 14px;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.15);
        }
    )");
    
    buttonsLayout->addWidget(m_saveButton);
    buttonsLayout->addWidget(m_changePasswordButton);
    buttonsLayout->addStretch();
    
    contentLayout->addWidget(buttonsWidget);
    
    mainLayout->addWidget(contentWidget);
    mainLayout->addStretch();
    
    connect(m_changeAvatarButton, &QPushButton::clicked, this, &ProfilePage::onChangeAvatar);
    connect(m_saveButton, &QPushButton::clicked, this, &ProfilePage::onSaveProfile);
    connect(m_changePasswordButton, &QPushButton::clicked, this, &ProfilePage::onChangePassword);
}

void ProfilePage::loadUserData()
{
    User user = m_userManager->getUser(m_userId);
    
    if (user.id == -1) return;
    
    m_usernameInput->setText(user.username);
    m_emailInput->setText(user.email);
    m_bioInput->setText(user.bio);
    
    if (!user.avatarPath.isEmpty()) {
        QPixmap avatar(user.avatarPath);
        if (!avatar.isNull()) {
            updateAvatar(avatar);
        } else {
            m_avatarLabel->setText(user.username.left(1).toUpper());
            m_avatarLabel->setStyleSheet(m_avatarLabel->styleSheet() + 
                "font-size: 60px; font-weight: bold; color: white;");
        }
    } else {
        m_avatarLabel->setText(user.username.left(1).toUpper());
        m_avatarLabel->setStyleSheet(m_avatarLabel->styleSheet() + 
            "font-size: 60px; font-weight: bold; color: white;");
    }
    
    QList<TrackData> tracks = DatabaseManager::instance().getUserTracks(m_userId);
    int totalTracks = tracks.size();
    int totalPlayCount = 0;
    
    for (const TrackData& track : tracks) {
        totalPlayCount += track.playCount;
    }
    
    QString stats = QString(
        "<b>Статистика:</b><br>"
        "Треков в библиотеке: %1<br>"
        "Всего прослушиваний: %2<br>"
        "Аккаунт создан: %3"
    ).arg(totalTracks).arg(totalPlayCount).arg(user.createdAt.toString("dd.MM.yyyy"));
    
    m_statsLabel->setText(stats);
}

void ProfilePage::updateAvatar(const QPixmap& avatar)
{
    QPixmap scaled = avatar.scaled(150, 150, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    
    QPixmap rounded(150, 150);
    rounded.fill(Qt::transparent);
    
    QPainter painter(&rounded);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QPainterPath path;
    path.addEllipse(0, 0, 150, 150);
    painter.setClipPath(path);
    
    painter.drawPixmap(0, 0, scaled);
    
    m_avatarLabel->setPixmap(rounded);
}

void ProfilePage::onChangeAvatar()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Выберите изображение",
        QDir::homePath(),
        "Images (*.png *.jpg *.jpeg *.bmp)"
    );
    
    if (fileName.isEmpty()) return;
    
    QPixmap avatar(fileName);
    if (avatar.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить изображение");
        return;
    }
    
    QString avatarDir = "avatars";
    QDir().mkpath(avatarDir);
    
    QString avatarPath = QString("%1/user_%2.png").arg(avatarDir).arg(m_userId);
    
    if (avatar.save(avatarPath)) {
        if (m_userManager->updateAvatar(m_userId, avatarPath)) {
            updateAvatar(avatar);
            QMessageBox::information(this, "Успех", "Аватар обновлен!");
        }
    }
}

void ProfilePage::onSaveProfile()
{
    QString email = m_emailInput->text().trimmed();
    QString bio = m_bioInput->toPlainText().trimmed();
    
    bool success = true;
    
    if (!email.isEmpty()) {
        success = success && m_userManager->updateEmail(m_userId, email);
    }
    
    success = success && m_userManager->updateBio(m_userId, bio);
    
    if (success) {
        QMessageBox::information(this, "Успех", "Профиль обновлен!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось обновить профиль");
    }
}

void ProfilePage::onChangePassword()
{
    bool ok;
    QString oldPassword = QInputDialog::getText(
        this,
        "Смена пароля",
        "Введите текущий пароль:",
        QLineEdit::Password,
        "",
        &ok
    );
    
    if (!ok || oldPassword.isEmpty()) return;
    
    QString newPassword = QInputDialog::getText(
        this,
        "Смена пароля",
        "Введите новый пароль (минимум 6 символов):",
        QLineEdit::Password,
        "",
        &ok
    );
    
    if (!ok || newPassword.length() < 6) {
        QMessageBox::warning(this, "Ошибка", "Пароль должен содержать минимум 6 символов");
        return;
    }
    
    QString confirmPassword = QInputDialog::getText(
        this,
        "Смена пароля",
        "Подтвердите новый пароль:",
        QLineEdit::Password,
        "",
        &ok
    );
    
    if (!ok || newPassword != confirmPassword) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
        return;
    }
    
    QMessageBox::information(this, "Успех", "Пароль изменен!");
}
