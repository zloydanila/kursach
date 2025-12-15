#include "MainWindow.h"

#include <QFileDialog>
#include <QEvent>
#include <QPixmap>
#include <QIcon>

// Минимально рабочая реализация аватар-UI, чтобы линкер собрался.

void MainWindow::setupAvatar()
{
    // На этом этапе avatarButton уже должен быть создан в UI-коде.
    // Просто загрузим актуальный аватар (из БД -> кэш -> дефолт).
    loadUserAvatar();
}

void MainWindow::setAvatarPixmap(const QPixmap& pixmap)
{
    if (!avatarButton) return;

    QPixmap scaled = pixmap.isNull()
        ? QPixmap()
        : pixmap.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    avatarButton->setIcon(QIcon(scaled));
    avatarButton->setIconSize(QSize(100, 100));
}

void MainWindow::setDefaultAvatar()
{
    // Самый простой дефолт: пустая иконка (если хочешь — заменишь на ресурс).
    if (!avatarButton) return;
    avatarButton->setIcon(QIcon());
}

void MainWindow::showAvatarOverlay()
{
    if (avatarOverlay) avatarOverlay->show();
}

void MainWindow::hideAvatarOverlay()
{
    if (avatarOverlay) avatarOverlay->hide();
}

void MainWindow::changeAvatar()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        "Выбрать аватар",
        QString(),
        "Images (*.png *.jpg *.jpeg *.bmp)"
    );

    if (fileName.isEmpty()) return;

    QPixmap px(fileName);
    if (px.isNull()) return;

    // Истина — БД, кэш — вторично
    if (saveAvatarToDbAndCache(px)) {
        setAvatarPixmap(px);
    } else {
        // если не сохранилось — хотя бы показать в UI
        setAvatarPixmap(px);
    }
}
