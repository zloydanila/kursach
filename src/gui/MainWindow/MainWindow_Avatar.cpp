#include "MainWindow.h"
#include "pages/FriendsPage.h"
#include "pages/MessagesPage.h"
#include <QVBoxLayout>
#include <QInputDialog>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QLineEdit>
#include <QListWidget>
#include <QTextEdit>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QToolButton>
#include <QTimer>
#include <QApplication>
#include <QMenu>
#include <QEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QDebug>
#include "database/DatabaseManager.h"
#include "database/api/MusicAPIManager.h"
#include "audio/AudioPlayer.h"
#include "core/models/Track.h"

void MainWindow::setupAvatar()
{
    setDefaultAvatar();
}

void MainWindow::setAvatarPixmap(const QPixmap& pixmap)
{
    QPixmap scaledPixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    QPixmap circularPixmap(100, 100);
    circularPixmap.fill(Qt::transparent);

    QPainter painter(&circularPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path;
    path.addEllipse(0, 0, 100, 100);
    painter.setClipPath(path);

    int x = (100 - scaledPixmap.width()) / 2;
    int y = (100 - scaledPixmap.height()) / 2;
    painter.drawPixmap(x, y, scaledPixmap);

    avatarButton->setIcon(QIcon(circularPixmap));
    avatarButton->setIconSize(QSize(100, 100));
    avatarButton->setText("");
}

void MainWindow::setDefaultAvatar()
{
    QPixmap defaultPixmap(100, 100);
    defaultPixmap.fill(Qt::transparent);

    QPainter painter(&defaultPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath circlePath;
    circlePath.addEllipse(0, 0, 100, 100);
    painter.setClipPath(circlePath);

    QRadialGradient gradient(50, 50, 50);
    gradient.setColorAt(0, QColor("#9B4BFF"));
    gradient.setColorAt(1, QColor("#7B1FA2"));

    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 100, 100);

    painter.setPen(QPen(Qt::white, 2));
    painter.setFont(QFont("Arial", 24, QFont::Bold));

    QString initials = currentUsername.left(2).toUpper();
    painter.drawText(QRect(0, 0, 100, 100), Qt::AlignCenter, initials);

    avatarButton->setIcon(QIcon(defaultPixmap));
    avatarButton->setIconSize(QSize(100, 100));
    avatarButton->setText("");
}

void MainWindow::changeAvatar()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Выберите изображение для аватарки",
        QDir::homePath(),
        "Images (*.png *.jpg *.jpeg *.bmp *.gif)"
    );

    if (!filePath.isEmpty()) {
        QPixmap newAvatar(filePath);
        if (newAvatar.isNull()) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Ошибка");
            msgBox.setText("Не удалось загрузить изображение");
            msgBox.setStyleSheet(R"(
                QMessageBox {
                    background-color: #1A1A21;
                    color: white;
                }
                QMessageBox QLabel {
                    color: white;
                }
                QMessageBox QPushButton {
                    background-color: #8A2BE2;
                    color: white;
                    border: none;
                    padding: 8px 16px;
                    border-radius: 6px;
                    min-width: 60px;
                }
                QMessageBox QPushButton:hover {
                    background-color: #9B4BFF;
                }
            )");
            msgBox.exec();
            return;
        }

        saveAvatar(newAvatar);
        setAvatarPixmap(newAvatar);

        QMessageBox msgBox;
        msgBox.setWindowTitle("Успех");
        msgBox.setText("Аватарка успешно изменена!");
        msgBox.setStyleSheet(R"(
            QMessageBox {
                background-color: #1A1A21;
                color: white;
            }
            QMessageBox QLabel {
                color: white;
            }
            QMessageBox QPushButton {
                background-color: #8A2BE2;
                color: white;
                border: none;
                padding: 8px 16px;
                border-radius: 6px;
                min-width: 60px;
            }
            QMessageBox QPushButton:hover {
                background-color: #9B4BFF;
            }
        )");
        msgBox.exec();
    }
}

void MainWindow::saveAvatar(const QPixmap& avatar)
{
    QDir dir("avatars");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString filePath = QString("avatars/user_%1.png").arg(currentUserId);

    if (!avatar.save(filePath, "PNG", 100)) {
        qDebug() << "Не удалось сохранить аватар:" << filePath;
    } else {
        qDebug() << "Аватар сохранен:" << filePath;
    }
}

void MainWindow::loadUserAvatar()
{
    QString userAvatarPath = QString("avatars/user_%1.png").arg(currentUserId);
    QFile avatarFile(userAvatarPath);

    if (avatarFile.exists()) {
        QPixmap avatarPixmap(userAvatarPath);
        if (!avatarPixmap.isNull()) {
            setAvatarPixmap(avatarPixmap);
            return;
        }
    }

    setDefaultAvatar();
}

void MainWindow::showAvatarOverlay()
{
    if (!avatarOverlay) {
        avatarOverlay = new QLabel(avatarButton);
        avatarOverlay->setText("Сменить\nаватар?");
        avatarOverlay->setStyleSheet(R"(
            QLabel {
                background-color: rgba(0, 0, 0, 0.85);
                color: white;
                font-weight: 600;
                font-size: 12px;
                border-radius: 50px;
                border: 2px solid rgba(138, 43, 226, 0.5);
                text-align: center;
                padding: 5px;
            }
        )");
        avatarOverlay->setAlignment(Qt::AlignCenter);

        avatarOverlay->setFixedSize(100, 100);

        avatarOverlay->move(0, 0);

        avatarOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    avatarOverlay->show();
    avatarOverlay->raise();
}

void MainWindow::hideAvatarOverlay()
{
    if (avatarOverlay) {
        avatarOverlay->hide();
    }
}

void MainWindow::onAvatarButtonEnter()
{
    showAvatarOverlay();
}

void MainWindow::onAvatarButtonLeave()
{
    hideAvatarOverlay();
}