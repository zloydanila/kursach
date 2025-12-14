#include "MainWindow.h"
#include "pages/FriendsPage.h"
#include "pages/MessagesPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QMenu>
#include <QApplication>
#include <QRandomGenerator>
#include <QDialog>
#include <QGraphicsDropShadowEffect>
#include "database/DatabaseManager.h"
#include "database/api/MusicAPIManager.h"
#include "audio/AudioPlayer.h"
#include "core/models/Track.h"


class ModernNotificationDialog : public QDialog
{
public:
    enum Type { Success, Error, Info };

    ModernNotificationDialog(const QString& message, Type type, QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setStyleSheet("background: transparent;");
        setFixedSize(420, 140);

        QString accentColor;
        QString icon;

        switch(type) {
            case Success:
                accentColor = "#2ECC71";
                icon = "✓";
                break;
            case Error:
                accentColor = "#E74C3C";
                icon = "✕";
                break;
            case Info:
            default:
                accentColor = "#3498DB";
                icon = "ⓘ";
                break;
        }

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        QWidget* container = new QWidget();
        container->setStyleSheet(QString(R"(
            QWidget {
                background: #0F0F14;
                border: 1px solid rgba(%1, 0.3);
                border-radius: 12px;
            }
        )").arg(accentColor.mid(1)));

        QVBoxLayout* containerLayout = new QVBoxLayout(container);
        containerLayout->setContentsMargins(0, 0, 0, 0);
        containerLayout->setSpacing(0);

        QWidget* header = new QWidget();
        header->setFixedHeight(50);
        header->setStyleSheet(QString(R"(
            QWidget {
                background: rgba(%1, 0.1);
                border-bottom: 1px solid rgba(%1, 0.2);
                border-radius: 12px 12px 0px 0px;
            }
        )").arg(accentColor.mid(1)));

        QHBoxLayout* headerLayout = new QHBoxLayout(header);
        headerLayout->setContentsMargins(15, 0, 15, 0);

        QLabel* iconLabel = new QLabel(icon);
        iconLabel->setStyleSheet(QString(R"(
            QLabel {
                color: %1;
                font-size: 20px;
                font-weight: bold;
                min-width: 30px;
                background: transparent;
                border: none;
            }
        )").arg(accentColor));

        QLabel* titleLabel = new QLabel("Уведомление");
        titleLabel->setStyleSheet(QString(R"(
            QLabel {
                color: %1;
                font-size: 14px;
                font-weight: 600;
                background: transparent;
                border: none;
            }
        )").arg(accentColor));

        headerLayout->addWidget(iconLabel);
        headerLayout->addWidget(titleLabel);
        headerLayout->addStretch();

        QWidget* body = new QWidget();
        body->setStyleSheet("background: transparent;");
        QVBoxLayout* bodyLayout = new QVBoxLayout(body);
        bodyLayout->setContentsMargins(20, 15, 20, 15);
        bodyLayout->setSpacing(12);

        QLabel* messageLabel = new QLabel(message);
        messageLabel->setStyleSheet(R"(
            QLabel {
                color: rgba(255, 255, 255, 0.85);
                font-size: 13px;
                font-weight: 400;
                background: transparent;
                border: none;
            }
        )");
        messageLabel->setWordWrap(true);

        bodyLayout->addWidget(messageLabel);

        QWidget* footer = new QWidget();
        footer->setFixedHeight(45);
        footer->setStyleSheet("background: transparent;");
        QHBoxLayout* footerLayout = new QHBoxLayout(footer);
        footerLayout->setContentsMargins(15, 0, 15, 0);
        footerLayout->setSpacing(10);

        QPushButton* okBtn = new QPushButton("OK");
        okBtn->setFixedWidth(100);
        okBtn->setFixedHeight(35);
        okBtn->setStyleSheet(QString(R"(
            QPushButton {
                background: %1;
                color: white;
                border: none;
                border-radius: 6px;
                font-size: 13px;
                font-weight: 600;
            }
            QPushButton:hover {
                background: rgba(%2, 0.8);
            }
            QPushButton:pressed {
                background: rgba(%2, 0.7);
            }
        )").arg(accentColor).arg(accentColor.mid(1)));

        connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

        footerLayout->addStretch();
        footerLayout->addWidget(okBtn);

        containerLayout->addWidget(header);
        containerLayout->addWidget(body);
        containerLayout->addWidget(footer);

        mainLayout->addWidget(container);

        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(15);
        shadow->setColor(QColor(0, 0, 0, 60));
        shadow->setOffset(0, 4);
        container->setGraphicsEffect(shadow);
    }
};


void MainWindow::setupConnections()
{
    connect(myMusicBtn, &QPushButton::clicked, this, [this]() {
        mainStack->setCurrentIndex(MYMUSICPAGE);
    });

    connect(profileBtn, &QPushButton::clicked, this, [this]() {
        mainStack->setCurrentIndex(PROFILEPAGE);
    });

    connect(messagesBtn, &QPushButton::clicked, this, [this]() {
        mainStack->setCurrentIndex(MESSAGESPAGE);
    });

    connect(friendsBtn, &QPushButton::clicked, this, [this]() {
        mainStack->setCurrentIndex(FRIENDSPAGE);
    });

    connect(notificationsBtn, &QPushButton::clicked, this, [this]() {
        mainStack->setCurrentIndex(NOTIFICATIONSPAGE);
    });

    connect(playlistBtn, &QPushButton::clicked, this, [this]() {
        mainStack->setCurrentIndex(PLAYLISTPAGE);
    });

    connect(musicSearchBtn, &QPushButton::clicked, this, [this]() {
        mainStack->setCurrentIndex(MUSICPAGE);
    });

    connect(roomsBtn, &QPushButton::clicked, this, [this]() {
        mainStack->setCurrentIndex(ROOMSPAGE);
    });

    connect(avatarButton, &QPushButton::clicked, this, &MainWindow::changeAvatar);
}


void MainWindow::playRadio(int radioId)
{
    QList<TrackData> radioList = DatabaseManager::instance().getUserTracks(currentUserId);

    for (const TrackData& radio : radioList) {
        if (radio.id == radioId) {
            audioPlayer->updateTrackInfo(radio.title, radio.artist);

            if (radio.filePath.startsWith("lastfm://")) {
                ModernNotificationDialog dialog(QString("Радиостанция '%1 - %2' из онлайн-библиотеки Last.fm. Для воспроизведения необходимо получить URL потока из Last.fm API.").arg(radio.artist).arg(radio.title), ModernNotificationDialog::Info, this);
                dialog.exec();
                return;
            }

            if (QFile::exists(radio.filePath)) {
                audioPlayer->playTrack(radio.filePath);
                DatabaseManager::instance().incrementPlayCount(radioId);
            } else {
                ModernNotificationDialog dialog("Файл не найден: " + radio.filePath, ModernNotificationDialog::Error, this);
                dialog.exec();
            }
            break;
        }
    }
}


void MainWindow::onPlaylistItemClicked(QListWidgetItem *item)
{
    if (item) {
        int radioId = item->data(Qt::UserRole).toInt();
        playRadio(radioId);
    }
}


void MainWindow::onRadioDeleteRequested(int radioId)
{
    bool success = DatabaseManager::instance().deleteTrack(radioId);

    if (success) {
        ModernNotificationDialog dialog("Радиостанция удалена из библиотеки!", ModernNotificationDialog::Success, this);
        dialog.exec();
    } else {
        ModernNotificationDialog dialog("Не удалось удалить радиостанцию", ModernNotificationDialog::Error, this);
        dialog.exec();
    }
}


void MainWindow::loadUserRadio()
{
    qDebug() << "loadUserRadio: Функция удалена, используется MyMusicPage вместо этого";
}


void MainWindow::showProfilePage() { mainStack->setCurrentIndex(PROFILEPAGE); }
void MainWindow::showMessagesPage() { mainStack->setCurrentIndex(MESSAGESPAGE); }
void MainWindow::showFriendsPage() { mainStack->setCurrentIndex(FRIENDSPAGE); }
void MainWindow::showNotificationsPage() { mainStack->setCurrentIndex(NOTIFICATIONSPAGE); }
void MainWindow::showPlaylistPage() { mainStack->setCurrentIndex(PLAYLISTPAGE); }
void MainWindow::showRoomsPage() { mainStack->setCurrentIndex(ROOMSPAGE); }


void MainWindow::onRadioStationAdded()
{
    qDebug() << "onRadioStationAdded: Сигнал получен, перезагружаем радиостанции для userId:" << currentUserId;
}