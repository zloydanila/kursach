#ifndef PROFILEPAGE_H
#define PROFILEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include "core/models/User.h"

class UserManager;

class ProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit ProfilePage(int userId, QWidget *parent = nullptr);
    ~ProfilePage();

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
    QPushButton* m_saveButton;
    QPushButton* m_changePasswordButton;
    QLabel* m_statsLabel;
};

#endif
