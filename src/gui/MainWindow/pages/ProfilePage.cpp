#include "ProfilePage.h"
#include "core/managers/UserManager.h"
#include "database/DatabaseManager.h"
#include "gui/dialogs/NotificationDialog.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QPainter>
#include <QPainterPath>
#include <QCoreApplication>
#include <QDir>
#include <QPixmapCache>

ProfilePage::ProfilePage(int userId, QWidget *parent)
    : QWidget(parent)
    , m_userId(userId)
    , m_userManager(new UserManager(this))
{
    setupUI();
    loadUserData();
}

ProfilePage::~ProfilePage() {}

void ProfilePage::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);

    m_avatarLabel = new QLabel(this);
    m_avatarLabel->setFixedSize(150, 150);
    m_avatarLabel->setAlignment(Qt::AlignCenter);

    m_changeAvatarButton = new QPushButton("Изменить аватар", this);

    m_usernameInput = new QLineEdit(this);
    m_usernameInput->setReadOnly(true);
    m_emailInput = new QLineEdit(this);
    m_bioInput = new QTextEdit(this);
    m_statsLabel = new QLabel(this);

    m_saveButton = new QPushButton("Сохранить изменения", this);
    m_changePasswordButton = new QPushButton("Изменить пароль", this);

    mainLayout->addWidget(m_avatarLabel);
    mainLayout->addWidget(m_changeAvatarButton);
    mainLayout->addWidget(m_usernameInput);
    mainLayout->addWidget(m_emailInput);
    mainLayout->addWidget(m_bioInput);
    mainLayout->addWidget(m_statsLabel);
    mainLayout->addWidget(m_saveButton);
    mainLayout->addWidget(m_changePasswordButton);

    connect(m_changeAvatarButton, &QPushButton::clicked, this, &ProfilePage::onChangeAvatar);
    connect(m_saveButton, &QPushButton::clicked, this, &ProfilePage::onSaveProfile);
    connect(m_changePasswordButton, &QPushButton::clicked, this, &ProfilePage::onChangePassword);
}

static void setAvatarFallback(QLabel* label, const QString& username)
{
    label->setText(username.left(1).toUpper());
}

void ProfilePage::loadUserData()
{
    User user = m_userManager->getUser(m_userId);
    if (user.id == -1) return;

    m_usernameInput->setText(user.username);
    m_emailInput->setText(user.email);
    m_bioInput->setText(user.bio);

    if (!user.avatarData.isEmpty()) {
        QPixmap pm;
        if (pm.loadFromData(user.avatarData)) {
            updateAvatar(pm);
            return;
        }
    }
    setAvatarFallback(m_avatarLabel, user.username);
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
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите изображение", QDir::homePath(),
                                                    "Images (*.png *.jpg *.jpeg *.bmp)");
    if (fileName.isEmpty()) return;

    QPixmap avatar(fileName);
    if (avatar.isNull()) {
        NotificationDialog dialog("Не удалось загрузить изображение", NotificationDialog::Error, this);
        dialog.exec();
        return;
    }

    QString avatarDir = QCoreApplication::applicationDirPath() + "/avatars";
    QDir().mkpath(avatarDir);
    QString avatarPath = avatarDir + QString("/user_%1.png").arg(m_userId);

    if (!avatar.save(avatarPath)) {
        NotificationDialog dialog("Не удалось сохранить файл аватара", NotificationDialog::Error, this);
        dialog.exec();
        return;
    }

    if (!m_userManager->updateAvatar(m_userId, avatarPath)) {
        NotificationDialog dialog("Не удалось сохранить аватар в базе", NotificationDialog::Error, this);
        dialog.exec();
        return;
    }

    QPixmapCache::clear();
    emit avatarUpdated(m_userId);
    loadUserData();
}

void ProfilePage::onSaveProfile() {}
void ProfilePage::onChangePassword() {}
