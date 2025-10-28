#include "MainWindow.h"
#include <QVBoxLayout>
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
#include <QTime>
#include <QToolButton>
#include <QStyle>
#include <QTimer>
#include <QApplication>
#include <QMenu>
#include <QEvent>

#include "database/DatabaseManager.h"
#include "database/api/MusicAPIManager.h"
#include "audio/AudioPlayer.h"
#include "core/Track.h"

class TrackListWidgetItem : public QListWidgetItem {
public:
    TrackListWidgetItem(const QString& text, int trackId, QListWidget* parent = nullptr)
        : QListWidgetItem(text, parent), m_trackId(trackId) {}
    
    int trackId() const { return m_trackId; }
    
private:
    int m_trackId;
};

MainWindow::MainWindow(const QString& username, int userId, QWidget *parent)
    : QMainWindow(parent)
    , currentUsername(username)
    , currentUserId(userId)
    , sidebar(nullptr)
    , avatarButton(nullptr)
    , avatarOverlay(nullptr)
    , usernameLabel(nullptr)
    , profileBtn(nullptr)
    , messagesBtn(nullptr)
    , friendsBtn(nullptr)
    , notificationsBtn(nullptr)
    , playlistBtn(nullptr)
    , musicSearchBtn(nullptr)
    , myMusicBtn(nullptr)
    , mainStack(nullptr)
    , profilePage(nullptr)
    , messagesPage(nullptr)
    , friendsPage(nullptr)
    , notificationsPage(nullptr)
    , playlistPage(nullptr)
    , musicPage(nullptr)
    , myMusicPage(nullptr)
    , searchInput(nullptr)
    , searchButton(nullptr)
    , topTracksButton(nullptr)
    , tracksList(nullptr)
    , trackInfo(nullptr)
    , userTracksList(nullptr)
    , refreshTracksBtn(nullptr)
    , addLocalTrackBtn(nullptr)
    , apiManager(new MusicAPIManager(this))
    , audioPlayer(new AudioPlayer(this))
    , currentTrackIndex(-1)
{
    setupUI();
    setupConnections();
    loadUserAvatar();
    loadUserTracks();
    

    setWindowTitle("SoundSpace - " + username);
    setMinimumSize(1400, 800);
    setStyleSheet("QMainWindow { background-color: #121212; }");
}

MainWindow::~MainWindow()
{
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == avatarButton) {
        if (event->type() == QEvent::Enter) {
            onAvatarButtonEnter();
            return true;
        } else if (event->type() == QEvent::Leave) {
            onAvatarButtonLeave();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::onAvatarButtonEnter()
{
    if (avatarOverlay) {
        avatarOverlay->show();
    }
}

void MainWindow::onAvatarButtonLeave()
{
    if (avatarOverlay) {
        avatarOverlay->hide();
    }
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    
    createSidebar();
    contentLayout->addWidget(sidebar);
    
    createPages();
    contentLayout->addWidget(mainStack, 1);
    
    mainLayout->addLayout(contentLayout, 1);
    
    audioPlayer->setupPlayerControls(centralWidget);
    mainLayout->addWidget(audioPlayer->getPlayerControls());
    
    setCentralWidget(centralWidget);
}

void MainWindow::createSidebar()
{
    sidebar = new QWidget();
    sidebar->setFixedWidth(280);
    sidebar->setStyleSheet(
        "QWidget {"
        "   background-color: #000000;"
        "   border: none;"
        "}"
    );
    
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setSpacing(0);
    sidebarLayout->setContentsMargins(0, 20, 0, 20);
    
    QLabel *logoLabel = new QLabel("SoundSpace");
    logoLabel->setStyleSheet(
        "QLabel {"
        "   color: #8A2BE2;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   padding: 20px;"
        "   background-color: #000000;"
        "}"
    );
    logoLabel->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(logoLabel);
    
    QWidget *avatarContainer = new QWidget();
    avatarContainer->setStyleSheet("QWidget { background-color: transparent; }");
    QHBoxLayout *avatarLayout = new QHBoxLayout(avatarContainer);
    avatarLayout->setAlignment(Qt::AlignCenter);
    avatarLayout->setContentsMargins(0, 20, 0, 20);
    
    avatarButton = new QPushButton();
    avatarButton->setFixedSize(80, 80);
    avatarButton->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   border: 3px solid #8A2BE2;"
        "   border-radius: 40px;"
        "}"
        "QPushButton:hover {"
        "   border: 3px solid #9B4BFF;"
        "   background-color: rgba(138, 43, 226, 0.3);"
        "}"
    );
    avatarButton->setCursor(Qt::PointingHandCursor);
    
    avatarOverlay = new QLabel(avatarButton);
    avatarOverlay->setText("изменить?");
    avatarOverlay->setStyleSheet(
        "QLabel {"
        "   background-color: rgba(0, 0, 0, 0.7);"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "   border-radius: 40px;"
        "   border: 1px solid white;"
        "}"
    );
    avatarOverlay->setAlignment(Qt::AlignCenter);
    avatarOverlay->setFixedSize(80, 80);
    avatarOverlay->hide();
    
    avatarButton->installEventFilter(this);
    
    connect(avatarButton, &QPushButton::clicked, this, &MainWindow::changeAvatar);
    
    avatarLayout->addWidget(avatarButton);
    sidebarLayout->addWidget(avatarContainer);
    
    usernameLabel = new QLabel(currentUsername);
    usernameLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   padding: 10px;"
        "   background-color: transparent;"
        "}"
    );
    usernameLabel->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(usernameLabel);
    
    sidebarLayout->addSpacing(20);
    
    QString navButtonStyle = 
        "QPushButton {"
        "   background-color: transparent;"
        "   color: #CCCCCC;"
        "   border: none;"
        "   padding: 15px 25px;"
        "   text-align: left;"
        "   font-size: 14px;"
        "   border-radius: 0px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1A1A1A;"
        "   color: white;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #8A2BE2;"
        "}";
    
    myMusicBtn = new QPushButton("Моя музыка");
    profileBtn = new QPushButton("Профиль");
    musicSearchBtn = new QPushButton("Поиск музыки");
    playlistBtn = new QPushButton("Мои плейлисты");
    friendsBtn = new QPushButton("Друзья");
    messagesBtn = new QPushButton("Сообщения");
    notificationsBtn = new QPushButton("Уведомления");
    
    myMusicBtn->setStyleSheet(navButtonStyle);
    profileBtn->setStyleSheet(navButtonStyle);
    musicSearchBtn->setStyleSheet(navButtonStyle);
    playlistBtn->setStyleSheet(navButtonStyle);
    friendsBtn->setStyleSheet(navButtonStyle);
    messagesBtn->setStyleSheet(navButtonStyle);
    notificationsBtn->setStyleSheet(navButtonStyle);
    
    sidebarLayout->addWidget(myMusicBtn);
    sidebarLayout->addWidget(profileBtn);
    sidebarLayout->addWidget(musicSearchBtn);
    sidebarLayout->addWidget(playlistBtn);
    sidebarLayout->addWidget(friendsBtn);
    sidebarLayout->addWidget(messagesBtn);
    sidebarLayout->addWidget(notificationsBtn);
    
    sidebarLayout->addStretch();
    
    QPushButton *logoutBtn = new QPushButton("Выйти");
    logoutBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px;"
        "   margin: 20px;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );
    connect(logoutBtn, &QPushButton::clicked, this, &QMainWindow::close);
    sidebarLayout->addWidget(logoutBtn);
}

void MainWindow::createPages()
{
    mainStack = new QStackedWidget();
    mainStack->setStyleSheet("QStackedWidget { background-color: #121212; }");
    
    createMyMusicPage();
    
    auto createSimplePage = [](const QString& title) -> QWidget* {
        QWidget *page = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(30, 30, 30, 30);
        
        QLabel *label = new QLabel(title);
        label->setStyleSheet("QLabel { color: white; font-size: 24px; font-weight: bold; }");
        label->setAlignment(Qt::AlignCenter);
        
        layout->addWidget(label);
        layout->addStretch();
        return page;
    };
    
    profilePage = createSimplePage("Профиль\n\nПользователь: " + currentUsername);
    messagesPage = createSimplePage("Сообщения");
    friendsPage = createSimplePage("Друзья");
    notificationsPage = createSimplePage("Уведомления");
    playlistPage = createSimplePage("Мои плейлисты");
    
    createMusicPage();
    
    mainStack->addWidget(myMusicPage);
    mainStack->addWidget(profilePage);
    mainStack->addWidget(messagesPage);
    mainStack->addWidget(friendsPage);
    mainStack->addWidget(notificationsPage);
    mainStack->addWidget(playlistPage);
    mainStack->addWidget(musicPage);
}

void MainWindow::createMyMusicPage()
{
    myMusicPage = new QWidget();
    QVBoxLayout *myMusicLayout = new QVBoxLayout(myMusicPage);
    myMusicLayout->setContentsMargins(30, 30, 30, 30);
    myMusicLayout->setSpacing(20);
    
    QLabel *myMusicTitle = new QLabel("Моя музыка");
    myMusicTitle->setStyleSheet("QLabel { color: white; font-size: 28px; font-weight: bold; }");
    
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    refreshTracksBtn = new QPushButton("Обновить");
    addLocalTrackBtn = new QPushButton("Добавить трек");
    
    QString controlButtonStyle = 
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   padding: 10px 20px;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}";
    
    refreshTracksBtn->setStyleSheet(controlButtonStyle);
    addLocalTrackBtn->setStyleSheet(controlButtonStyle);
    
    controlsLayout->addWidget(refreshTracksBtn);
    controlsLayout->addWidget(addLocalTrackBtn);
    controlsLayout->addStretch();
    
    userTracksList = new QListWidget();
    userTracksList->setStyleSheet(
        "QListWidget {"
        "   background-color: #1A1A1A;"
        "   border: 2px solid #2A2A2A;"
        "   border-radius: 10px;"
        "   color: white;"
        "   font-size: 14px;"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   background-color: #1A1A1A;"
        "   border-bottom: 1px solid #2A2A2A;"
        "   height: 50px;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #2A2A2A;"
        "}"
    );
    
    myMusicLayout->addWidget(myMusicTitle);
    myMusicLayout->addLayout(controlsLayout);
    myMusicLayout->addWidget(userTracksList, 1);
}

void MainWindow::createMusicPage()
{
    musicPage = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(musicPage);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);
    
    QLabel *titleLabel = new QLabel("Поиск музыки через Last.fm");
    titleLabel->setStyleSheet("QLabel { color: white; font-size: 28px; font-weight: bold; }");
    mainLayout->addWidget(titleLabel);
    
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Введите название трека или исполнителя...");
    searchInput->setStyleSheet(
        "QLineEdit {"
        "   background-color: #1A1A1A;"
        "   border: 2px solid #2A2A2A;"
        "   border-radius: 25px;"
        "   padding: 12px 20px;"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #8A2BE2;"
        "}"
    );
    
    searchButton = new QPushButton("Поиск");
    searchButton->setFixedWidth(120);
    searchButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px;"
        "   border-radius: 25px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );
    
    topTracksButton = new QPushButton("Популярные треки");
    topTracksButton->setFixedWidth(150);
    topTracksButton->setStyleSheet(searchButton->styleSheet());
    
    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(topTracksButton);
    mainLayout->addLayout(searchLayout);
    
    QHBoxLayout *contentLayout = new QHBoxLayout();
    
    tracksList = new QListWidget();
    tracksList->setStyleSheet(
        "QListWidget {"
        "   background-color: #1A1A1A;"
        "   border: 2px solid #2A2A2A;"
        "   border-radius: 10px;"
        "   color: white;"
        "   font-size: 14px;"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   background-color: #1A1A1A;"
        "   border-bottom: 1px solid #2A2A2A;"
        "   padding: 15px;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #2A2A2A;"
        "}"
    );
    
    QWidget *trackInfoWidget = new QWidget();
    QVBoxLayout *trackInfoLayout = new QVBoxLayout(trackInfoWidget);
    
    trackInfo = new QTextEdit();
    trackInfo->setReadOnly(true);
    trackInfo->setStyleSheet(
        "QTextEdit {"
        "   background-color: #1A1A1A;"
        "   border: 2px solid #2A2A2A;"
        "   border-radius: 10px;"
        "   color: white;"
        "   font-size: 14px;"
        "   padding: 20px;"
        "}"
    );
    trackInfo->setPlaceholderText("Выберите трек для просмотра информации...");
    
    QPushButton *addToLibraryBtn = new QPushButton("Добавить в библиотеку");
    addToLibraryBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   margin-top: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );
    addToLibraryBtn->setVisible(false);
    
    trackInfoLayout->addWidget(trackInfo);
    trackInfoLayout->addWidget(addToLibraryBtn);
    
    contentLayout->addWidget(tracksList, 2);
    contentLayout->addWidget(trackInfoWidget, 1);
    mainLayout->addLayout(contentLayout, 1);
    
    tracksList->setContextMenuPolicy(Qt::CustomContextMenu);
    
    connect(tracksList, &QListWidget::itemClicked, this, [this, addToLibraryBtn](QListWidgetItem *item) {
        trackInfo->setPlainText(item->data(Qt::UserRole).toString());
        addToLibraryBtn->setVisible(true);
        addToLibraryBtn->setProperty("trackData", item->data(Qt::UserRole + 1));
    });
    
    connect(addToLibraryBtn, &QPushButton::clicked, this, [this, addToLibraryBtn]() {
        QVariant trackData = addToLibraryBtn->property("trackData");
        if (trackData.isValid()) {
            QVariantMap track = trackData.toMap();
            
            bool success = DatabaseManager::instance().addTrackFromAPI(
                currentUserId,
                track["title"].toString(),
                track["artist"].toString(),
                "",
                0,
                "",
                track["coverUrl"].toString()
            );
            
            if (success) {
                QMessageBox::information(this, "Успех", "Трек добавлен в библиотеку!");
                addToLibraryBtn->setVisible(false);
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось добавить трек в библиотеку");
            }
        }
    });
    
    connect(tracksList, &QListWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        QListWidgetItem *item = tracksList->itemAt(pos);
        if (item) {
            QMenu contextMenu(this);
            QAction *addToLibraryAction = contextMenu.addAction("Добавить в библиотеку");
            QAction *viewInfoAction = contextMenu.addAction("Просмотреть информацию");
            
            QAction *selectedAction = contextMenu.exec(tracksList->mapToGlobal(pos));
            
            if (selectedAction == addToLibraryAction) {
                QVariant trackData = item->data(Qt::UserRole + 1);
                if (trackData.isValid()) {
                    QVariantMap track = trackData.toMap();
                    
                    bool success = DatabaseManager::instance().addTrackFromAPI(
                        currentUserId,
                        track["title"].toString(),
                        track["artist"].toString(),
                        "", 0, "", track["coverUrl"].toString()
                    );
                    
                    if (success) {
                        QMessageBox::information(this, "Успех", "Трек добавлен в библиотеку!");
                    }
                }
            } else if (selectedAction == viewInfoAction) {
                trackInfo->setPlainText(item->data(Qt::UserRole).toString());
            }
        }
    });
}

void MainWindow::setupConnections()
{
    
    connect(myMusicBtn, &QPushButton::clicked, this, [this]() { 
        mainStack->setCurrentIndex(MY_MUSIC_PAGE); 
        loadUserTracks();
    });
    connect(profileBtn, &QPushButton::clicked, this, [this]() { 
        mainStack->setCurrentIndex(PROFILE_PAGE); 
    });
    connect(messagesBtn, &QPushButton::clicked, this, [this]() { 
        mainStack->setCurrentIndex(MESSAGES_PAGE); 
    });
    connect(friendsBtn, &QPushButton::clicked, this, [this]() { 
        mainStack->setCurrentIndex(FRIENDS_PAGE); 
    });
    connect(notificationsBtn, &QPushButton::clicked, this, [this]() { 
        mainStack->setCurrentIndex(NOTIFICATIONS_PAGE); 
    });
    connect(playlistBtn, &QPushButton::clicked, this, [this]() { 
        mainStack->setCurrentIndex(PLAYLIST_PAGE); 
    });
    connect(musicSearchBtn, &QPushButton::clicked, this, [this]() { 
        mainStack->setCurrentIndex(MUSIC_PAGE); 
    });
    
    connect(avatarButton, &QPushButton::clicked, this, &MainWindow::changeAvatar);
    
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchMusic);
    connect(topTracksButton, &QPushButton::clicked, this, &MainWindow::showTopTracks);
    connect(searchInput, &QLineEdit::returnPressed, this, &MainWindow::searchMusic);
    connect(apiManager, &MusicAPIManager::tracksFound, this, &MainWindow::onTracksFound);
    connect(apiManager, &MusicAPIManager::errorOccurred, this, &MainWindow::onNetworkError);
    
    connect(refreshTracksBtn, &QPushButton::clicked, this, &MainWindow::loadUserTracks);
    connect(addLocalTrackBtn, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this, "Выберите аудиофайл", QDir::homePath(), 
                                                       "Audio Files (*.mp3 *.wav *.flac *.ogg *.m4a)");
        if (!filePath.isEmpty()) {
            QMessageBox::information(this, "Успех", "Трек добавлен в библиотеку!");
            loadUserTracks();
        }
    });
    connect(userTracksList, &QListWidget::itemClicked, this, &MainWindow::onPlaylistItemClicked);
}

void MainWindow::playTrack(int trackId)
{
    for (int i = 0; i < currentTracks.size(); ++i) {
        if (currentTracks[i].id == trackId) {
            currentTrackIndex = i;
            Track track = currentTracks[i];
            
            audioPlayer->updateTrackInfo(track.title, track.artist);
            
            if (track.filePath.startsWith("lastfm://")) {
                QMessageBox::information(this, "Информация", 
                    QString("Трек '%1 - %2' из онлайн-библиотеки Last.fm.\n\n"
                           "Для воспроизведения онлайн-треков необходимо:\n"
                           "1. Получить URL потока из Last.fm API\n"
                           "2. Реализовать онлайн-воспроизведение").arg(track.artist).arg(track.title));
                return;
            }
            
            if (QFile::exists(track.filePath)) {
                audioPlayer->playTrack(track.filePath);
                DatabaseManager::instance().incrementPlayCount(trackId);
            } else {
                QMessageBox::warning(this, "Ошибка", "Файл не найден: " + track.filePath);
            }
            break;
        }
    }
}

void MainWindow::onPlaylistItemClicked(QListWidgetItem *item)
{
    if (item) {
        int trackId = item->data(Qt::UserRole).toInt();
        playTrack(trackId);
    }
}

void MainWindow::onTrackDeleteRequested(int trackId)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Удаление трека",
        "Вы уверены, что хотите удалить этот трек из своей библиотеки?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        
        bool success = DatabaseManager::instance().deleteTrack(trackId);
        
        if (success) {
            QMessageBox::information(this, "Успех", "Трек удален из библиотеки!");
            loadUserTracks();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить трек");
        }
    }
}
void MainWindow::loadUserTracks()
{
    userTracksList->clear();
    currentTracks = DatabaseManager::instance().getUserTracks(currentUserId);
    
    for (int i = 0; i < currentTracks.size(); ++i) {
        const Track &track = currentTracks[i];
        QString duration = track.duration > 0 ? audioPlayer->formatTime(track.duration) : "0:00";
        QString itemText = QString("%1 - %2 (%3)").arg(track.artist).arg(track.title).arg(duration);
        
        TrackListWidgetItem *item = new TrackListWidgetItem("", track.id, userTracksList);
        item->setData(Qt::UserRole, track.id);
        item->setSizeHint(QSize(item->sizeHint().width(), 50));
        
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(15, 8, 15, 8);
        itemLayout->setSpacing(10);
        
        QLabel *trackLabel = new QLabel(itemText);
        trackLabel->setStyleSheet("QLabel { color: white; font-size: 14px; background-color: transparent; }");
        trackLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        
        QPushButton *deleteBtn = new QPushButton("×");
        deleteBtn->setFixedSize(30, 25);
        deleteBtn->setStyleSheet(
            "QPushButton {"
            "   background-color: #FF4444;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 3px;"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background-color: #CC3333;"
            "}"
        );
        
        // ПРАВИЛЬНОЕ подключение сигнала
        connect(deleteBtn, &QPushButton::clicked, this, [this, trackId = track.id]() {
            onTrackDeleteRequested(trackId);
        });
        
        itemLayout->addWidget(trackLabel);
        itemLayout->addWidget(deleteBtn);
        
        itemWidget->setLayout(itemLayout);
        itemWidget->setStyleSheet("QWidget { background-color: transparent; }");
        
        userTracksList->setItemWidget(item, itemWidget);
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

void MainWindow::setupAvatar()
{
    setDefaultAvatar();
}

void MainWindow::setAvatarPixmap(const QPixmap& pixmap)
{
    QPixmap circularPixmap(80, 80);
    circularPixmap.fill(Qt::transparent);
    
    QPainter painter(&circularPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    
    QPainterPath path;
    path.addEllipse(0, 0, 80, 80);
    painter.setClipPath(path);
    
    QPixmap scaledPixmap = pixmap.scaled(80, 80, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    
    int x = (80 - scaledPixmap.width()) / 2;
    int y = (80 - scaledPixmap.height()) / 2;
    painter.drawPixmap(x, y, scaledPixmap);
    
    avatarButton->setIcon(QIcon(circularPixmap));
    avatarButton->setIconSize(QSize(80, 80));
    avatarButton->setText("");
}

void MainWindow::setDefaultAvatar()
{
    QPixmap defaultPixmap(80, 80);
    defaultPixmap.fill(Qt::transparent);
    
    QPainter painter(&defaultPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    QRadialGradient gradient(40, 40, 40);
    gradient.setColorAt(0, QColor("#9B4BFF"));
    gradient.setColorAt(1, QColor("#7B1FA2"));
    
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 80, 80);
    
    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Arial", 20, QFont::Bold));
    
    QString initials = currentUsername.left(2).toUpper();
    QRect textRect(0, 0, 80, 80);
    painter.drawText(textRect, Qt::AlignCenter, initials);
    
    avatarButton->setIcon(QIcon(defaultPixmap));
    avatarButton->setIconSize(QSize(80, 80));
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
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить изображение");
            return;
        }
        
        saveAvatar(newAvatar);
        setAvatarPixmap(newAvatar);
        
        QMessageBox::information(this, "Успех", "Аватарка успешно изменена!");
    }
}

void MainWindow::saveAvatar(const QPixmap& avatar)
{
    QDir().mkpath("avatars");
    
    QString filePath = QString("avatars/user_%1.png").arg(currentUserId);
    avatar.save(filePath, "PNG");
}

void MainWindow::showProfilePage() { mainStack->setCurrentIndex(PROFILE_PAGE); }
void MainWindow::showMessagesPage() { mainStack->setCurrentIndex(MESSAGES_PAGE); }
void MainWindow::showFriendsPage() { mainStack->setCurrentIndex(FRIENDS_PAGE); }
void MainWindow::showNotificationsPage() { mainStack->setCurrentIndex(NOTIFICATIONS_PAGE); }
void MainWindow::showPlaylistPage() { mainStack->setCurrentIndex(PLAYLIST_PAGE); }

void MainWindow::searchMusic()
{
    qDebug() << "Запуск поиска музыки...";
    
    QString query = searchInput->text().trimmed();
    if (query.isEmpty()) {
        trackInfo->setPlainText("Введите поисковый запрос!");
        return;
    }
    
    tracksList->clear();
    trackInfo->setPlainText("Поиск треков...");
    
    enableSearchControls(false);
    
    QApplication::processEvents();
    
    qDebug() << "Поиск по запросу:" << query;
    apiManager->searchTracks(query, currentUserId);
}

void MainWindow::showTopTracks()
{
    tracksList->clear();
    trackInfo->setPlainText("Загрузка популярных треков...");
    
    enableSearchControls(false);
    QApplication::processEvents();
    
    qDebug() << "Loading top tracks";
    apiManager->getTopTracks(currentUserId);
}

void MainWindow::onTracksFound(const QVariantList& tracks)
{
  
    enableSearchControls(true);
    
    tracksList->clear();
    
    if (tracks.isEmpty()) {
        trackInfo->setPlainText("Треки не найдены!");
        return;
    }
    
    for (const QVariant &trackVar : tracks) {
        QVariantMap track = trackVar.toMap();
        QString title = track["title"].toString();
        QString artist = track["artist"].toString();
        QString listeners = track.value("listeners", "N/A").toString();
        QString coverUrl = track.value("coverUrl", "").toString();
        
        QString itemText = QString("%1 - %2").arg(title).arg(artist);
        if (listeners != "N/A") {
            itemText += QString(" [%1]").arg(listeners);
        }
        
        QListWidgetItem *item = new QListWidgetItem(itemText, tracksList);
        
        QString trackInfoText = QString(
            "Трек: %1\n"
            "Исполнитель: %2\n"
            "Прослушиваний: %3\n"
            "Обложка: %4"
        ).arg(title).arg(artist).arg(listeners).arg(coverUrl.isEmpty() ? "Нет" : "Есть");
        
        item->setData(Qt::UserRole, trackInfoText);
        item->setData(Qt::UserRole + 1, track);
    }
    
    trackInfo->setPlainText(QString("Найдено треков: %1\n\nВыберите трек для подробной информации.").arg(tracks.size()));
}

void MainWindow::onNetworkError(const QString& error)
{
    enableSearchControls(true);
    
    QMessageBox::warning(this, "Ошибка поиска", 
        QString("Произошла ошибка при поиске музыки:\n%1").arg(error));
}


void MainWindow::enableSearchControls(bool enable)
{
    searchButton->setEnabled(enable);
    topTracksButton->setEnabled(enable);
    searchInput->setEnabled(enable);
}