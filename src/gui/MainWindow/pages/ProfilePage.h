#ifndef PROFILEPAGE_H
#define PROFILEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QPixmap>

class UserManager;

class ProfilePage : public QWidget
{
    Q_OBJECT
public:
    explicit ProfilePage(int userId, QWidget *parent = nullptr);
    ~ProfilePage();

signals:
    void avatarUpdated(int userId);

private slots:
    void onChangeAvatar();
    void onSaveProfile();
    void onChangePassword();

private:
    void setupUI();
    void loadUserData();
    void updateAvatar(const QPixmap& avatar);

    int m_userId;
    UserManager* m_userManager;

    QLabel* m_avatarLabel;
    QPushButton* m_changeAvatarButton;
    QLineEdit* m_usernameInput;
    QLineEdit* m_emailInput;
    QTextEdit* m_bioInput;
    QLabel* m_statsLabel;
    QPushButton* m_saveButton;
    QPushButton* m_changePasswordButton;
};

#endif
