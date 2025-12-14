#include "PlaylistsPage.h"
#include "core/managers/PlaylistManager.h"
#include "gui/widgets/TrackListItem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QStackedWidget>

PlaylistPage::PlaylistPage(int userId, QWidget *parent)
    : QWidget(parent)
    , m_userId(userId)
    , m_currentPlaylistId(-1)
    , m_playlistManager(new PlaylistManager(userId, this))
{
    setupUI();
    loadPlaylists();
    
    connect(m_playlistManager, &PlaylistManager::playlistCreated, [this](int) {
        loadPlaylists();
    });
}

PlaylistPage::~PlaylistPage()
{
}

void PlaylistPage::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    QStackedWidget* stackedWidget = new QStackedWidget();
    
    m_playlistsWidget = new QWidget();
    QVBoxLayout* playlistsLayout = new QVBoxLayout(m_playlistsWidget);
    playlistsLayout->setContentsMargins(40, 40, 40, 40);
    playlistsLayout->setSpacing(25);
    
    QWidget* headerWidget = new QWidget();
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* titleLabel = new QLabel("Плейлисты");
    titleLabel->setStyleSheet("color: white; font-size: 32px; font-weight: bold;");
    
    m_createButton = new QPushButton("+ Создать плейлист");
    m_createButton->setFixedHeight(45);
    m_createButton->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 10px;
            padding: 0 25px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
    )");
    
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_createButton);
    
    m_playlistsList = new QListWidget();
    m_playlistsList->setStyleSheet(R"(
        QListWidget {
            background: transparent;
            border: none;
            outline: none;
        }
        QListWidget::item {
            background: rgba(255, 255, 255, 0.03);
            border: 1px solid rgba(255, 255, 255, 0.05);
            border-radius: 12px;
            padding: 20px;
            margin-bottom: 10px;
        }
        QListWidget::item:hover {
            background: rgba(255, 255, 255, 0.05);
            border: 1px solid rgba(138, 43, 226, 0.3);
        }
        QListWidget::item:selected {
            background: rgba(138, 43, 226, 0.15);
            border: 1px solid #8A2BE2;
        }
    )");
    
    playlistsLayout->addWidget(headerWidget);
    playlistsLayout->addWidget(m_playlistsList);
    
    m_tracksWidget = new QWidget();
    QVBoxLayout* tracksLayout = new QVBoxLayout(m_tracksWidget);
    tracksLayout->setContentsMargins(40, 40, 40, 40);
    tracksLayout->setSpacing(25);
    
    QWidget* tracksHeaderWidget = new QWidget();
    QHBoxLayout* tracksHeaderLayout = new QHBoxLayout(tracksHeaderWidget);
    tracksHeaderLayout->setContentsMargins(0, 0, 0, 0);
    
    m_backButton = new QPushButton("← Назад");
    m_backButton->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            color: rgba(255, 255, 255, 0.7);
            border: none;
            font-size: 14px;
            padding: 10px;
        }
        QPushButton:hover {
            color: white;
        }
    )");
    
    m_playlistTitleLabel = new QLabel();
    m_playlistTitleLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");
    
    tracksHeaderLayout->addWidget(m_backButton);
    tracksHeaderLayout->addWidget(m_playlistTitleLabel);
    tracksHeaderLayout->addStretch();
    
    m_tracksList = new QListWidget();
    m_tracksList->setStyleSheet(R"(
        QListWidget {
            background: rgba(255, 255, 255, 0.02);
            border: 2px solid rgba(255, 255, 255, 0.05);
            border-radius: 15px;
            outline: none;
            padding: 10px;
        }
        QListWidget::item {
            background: transparent;
            border: none;
        }
        QListWidget::item:hover {
            background: rgba(255, 255, 255, 0.05);
            border-radius: 8px;
        }
    )");
    
    tracksLayout->addWidget(tracksHeaderWidget);
    tracksLayout->addWidget(m_tracksList);
    
    stackedWidget->addWidget(m_playlistsWidget);
    stackedWidget->addWidget(m_tracksWidget);
    
    mainLayout->addWidget(stackedWidget);
    
    connect(m_createButton, &QPushButton::clicked, this, &PlaylistPage::onCreatePlaylist);
    connect(m_playlistsList, &QListWidget::itemDoubleClicked, this, &PlaylistPage::onPlaylistSelected);
    connect(m_tracksList, &QListWidget::itemDoubleClicked, this, &PlaylistPage::onTrackDoubleClicked);
    connect(m_backButton, &QPushButton::clicked, this, &PlaylistPage::onBackToPlaylists);
}

void PlaylistPage::loadPlaylists()
{
    QVector<Playlist> playlists = m_playlistManager->getUserPlaylists();
    displayPlaylists(playlists);
}

void PlaylistPage::displayPlaylists(const QVector<Playlist>& playlists)
{
    m_playlistsList->clear();
    
    for (const Playlist& playlist : playlists) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setData(Qt::UserRole, playlist.id);
        
        QString displayText = QString("📁  %1\n%2")
            .arg(playlist.name)
            .arg(playlist.createdAt);
        
        item->setText(displayText);
        item->setSizeHint(QSize(0, 80));
        
        m_playlistsList->addItem(item);
    }
}

void PlaylistPage::onCreatePlaylist()
{
    bool ok;
    QString name = QInputDialog::getText(
        this,
        "Создать плейлист",
        "Название плейлиста:",
        QLineEdit::Normal,
        "",
        &ok
    );
    
    if (!ok || name.isEmpty()) return;
    
    QString description = QInputDialog::getText(
        this,
        "Создать плейлист",
        "Описание (необязательно):",
        QLineEdit::Normal,
        "",
        &ok
    );
    
    int playlistId = m_playlistManager->createPlaylist(name, description);
    
    if (playlistId != -1) {
        QMessageBox::information(this, "Успех", "Плейлист создан!");
        loadPlaylists();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось создать плейлист");
    }
}

void PlaylistPage::onPlaylistSelected(QListWidgetItem* item)
{
    int playlistId = item->data(Qt::UserRole).toInt();
    m_currentPlaylistId = playlistId;
    
    Playlist playlist = m_playlistManager->getPlaylist(playlistId);
    m_playlistTitleLabel->setText(playlist.name);
    
    displayPlaylistTracks(playlistId);
    showTracksView();
}

void PlaylistPage::displayPlaylistTracks(int playlistId)
{
    m_tracksList->clear();
    
    QVector<TrackData> tracks = m_playlistManager->getPlaylistTracks(playlistId);
    
    if (tracks.isEmpty()) {
        QLabel* emptyLabel = new QLabel("В этом плейлисте пока нет треков");
        emptyLabel->setStyleSheet("color: rgba(255, 255, 255, 0.5); font-size: 16px;");
        emptyLabel->setAlignment(Qt::AlignCenter);
        
        QListWidgetItem* item = new QListWidgetItem(m_tracksList);
        item->setSizeHint(QSize(0, 100));
        m_tracksList->setItemWidget(item, emptyLabel);
        return;
    }
    
    for (const TrackData& track : tracks) {
        QListWidgetItem* item = new QListWidgetItem(m_tracksList);
        item->setData(Qt::UserRole, track.id);
        
        TrackListItem* trackWidget = new TrackListItem(track, true);
        
        connect(trackWidget, &TrackListItem::playClicked, [this, track]() {
            emit playTrackRequested(track);
        });
        
        connect(trackWidget, &TrackListItem::moreClicked, [this, track, playlistId](int trackId, const QPoint& pos) {
            QMenu menu(this);
            menu.setStyleSheet(R"(
                QMenu {
                    background: #1A1A21;
                    border: 1px solid rgba(255, 255, 255, 0.1);
                    border-radius: 8px;
                    padding: 5px;
                }
                QMenu::item {
                    background: transparent;
                    color: white;
                    padding: 8px 20px;
                    border-radius: 4px;
                }
                QMenu::item:selected {
                    background: rgba(138, 43, 226, 0.3);
                }
            )");
            
            QAction* playAction = menu.addAction("Воспроизвести");
            QAction* removeAction = menu.addAction("Удалить из плейлиста");
            
            QAction* selected = menu.exec(pos);
            
            if (selected == playAction) {
                emit playTrackRequested(track);
            } else if (selected == removeAction) {
                if (m_playlistManager->removeTrackFromPlaylist(playlistId, trackId)) {
                    displayPlaylistTracks(playlistId);
                }
            }
        });
        
        item->setSizeHint(trackWidget->sizeHint());
        m_tracksList->setItemWidget(item, trackWidget);
    }
}

void PlaylistPage::onTrackDoubleClicked(QListWidgetItem* item)
{
    int trackId = item->data(Qt::UserRole).toInt();
    
    QVector<TrackData> tracks = m_playlistManager->getPlaylistTracks(m_currentPlaylistId);
    
    for (const TrackData& track : tracks) {
        if (track.id == trackId) {
            emit playTrackRequested(track);
            break;
        }
    }
}

void PlaylistPage::onBackToPlaylists()
{
    showPlaylistsView();
}

void PlaylistPage::showPlaylistsView()
{
    QStackedWidget* stack = qobject_cast<QStackedWidget*>(layout()->itemAt(0)->widget());
    if (stack) {
        stack->setCurrentIndex(0);
    }
}

void PlaylistPage::showTracksView()
{
    QStackedWidget* stack = qobject_cast<QStackedWidget*>(layout()->itemAt(0)->widget());
    if (stack) {
        stack->setCurrentIndex(1);
    }
}
