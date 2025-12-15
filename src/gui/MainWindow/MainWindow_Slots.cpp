#include "MainWindow.h"
#include "gui/MainWindow/pages/MyMusicPage.h"
#include "pages/FriendsPage.h"
#include "pages/MessagesPage.h"

#include <QPushButton>
#include <QStackedWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDebug>

#include "database/DatabaseManager.h"
#include "audio/AudioPlayer.h"
#include "core/models/Track.h"

void MainWindow::setupConnections()
{
    if (myMusicBtn)     connect(myMusicBtn,     &QPushButton::clicked, this, &MainWindow::showMyMusicPage);
    if (musicSearchBtn) connect(musicSearchBtn, &QPushButton::clicked, this, &MainWindow::showSearchRadioPage);
    if (friendsBtn)     connect(friendsBtn,     &QPushButton::clicked, this, &MainWindow::showFriendsPage);
    if (messagesBtn)    connect(messagesBtn,    &QPushButton::clicked, this, &MainWindow::showMessagesPage);

    if (avatarButton)   connect(avatarButton,   &QPushButton::clicked, this, &MainWindow::changeAvatar);
}

void MainWindow::showMyMusicPage()     { if (mainStack && myMusicPage)  mainStack->setCurrentWidget(static_cast<QWidget*>(myMusicPage)); }
void MainWindow::showMessagesPage()    { if (mainStack && messagesPage) mainStack->setCurrentWidget(static_cast<QWidget*>(messagesPage)); }
void MainWindow::showFriendsPage()     { if (mainStack && friendsPage)  mainStack->setCurrentWidget(static_cast<QWidget*>(friendsPage)); }
void MainWindow::showSearchRadioPage() { if (mainStack && musicPage)    mainStack->setCurrentWidget(musicPage); }

void MainWindow::playRadio(int radioId)
{
    QList<TrackData> radioList = DatabaseManager::instance().getUserTracks(currentUserId);
    for (const TrackData& radio : radioList) {
        if (radio.id == radioId) {
            audioPlayer->updateTrackInfo(radio.title, radio.artist);
            if (QFile::exists(radio.filePath)) {
                audioPlayer->playTrack(radio.filePath);
                DatabaseManager::instance().incrementPlayCount(radioId);
            } else {
                QMessageBox::warning(this, "Ошибка", "Файл не найден: " + radio.filePath);
            }
            break;
        }
    }
}

void MainWindow::onPlaylistItemClicked(QListWidgetItem *item)
{
    if (!item) return;
    playRadio(item->data(Qt::UserRole).toInt());
}

void MainWindow::onRadioDeleteRequested(int radioId)
{
    bool success = DatabaseManager::instance().deleteTrack(radioId);
    QMessageBox::information(this, success ? "Успех" : "Ошибка",
                             success ? "Радиостанция удалена!" : "Не удалось удалить радиостанцию");
}

void MainWindow::loadUserRadio()
{
    qDebug() << "loadUserRadio: Функция удалена, используется MyMusicPage вместо этого";
}

void MainWindow::onRadioStationAdded()
{
    qDebug() << "onRadioStationAdded: received";
}

void MainWindow::showProfilePage()        { /* removed */ }
void MainWindow::showNotificationsPage()  { /* removed */ }
void MainWindow::showRoomsPage()          { /* removed */ }
