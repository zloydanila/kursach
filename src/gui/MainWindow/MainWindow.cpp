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
#include <QDebug>

MainWindow::MainWindow(const QString& username, int userId, QWidget *parent)
    : QMainWindow(parent)
    , currentUsername(username)
    , currentUserId(userId)
    , sidebar(nullptr)
    , avatarButton(nullptr)
    , usernameLabel(nullptr)
    , profileBtn(nullptr)
    , messagesBtn(nullptr)
    , friendsBtn(nullptr)
    , notificationsBtn(nullptr)
    , playlistBtn(nullptr)
    , musicSearchBtn(nullptr)
    , mainStack(nullptr)
    , profilePage(nullptr)
    , messagesPage(nullptr)
    , friendsPage(nullptr)
    , notificationsPage(nullptr)
    , playlistPage(nullptr)
    , musicPage(nullptr)
    , searchInput(nullptr)
    , searchButton(nullptr)
    , topTracksButton(nullptr)
    , tracksList(nullptr)
    , trackInfo(nullptr)
    , apiManager(new MusicAPIManager(this))
{
    setupUI();
    setupConnections();
    loadUserAvatar();
    
    setWindowTitle("SoundSpace - Главное окно");
    setMinimumSize(1200, 700);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    createSidebar();
    mainLayout->addWidget(sidebar);
    
    createPages();
    mainLayout->addWidget(mainStack);
    
    setCentralWidget(centralWidget);
}

void MainWindow::createSidebar()
{
    sidebar = new QWidget();
    sidebar->setFixedWidth(250);
    sidebar->setStyleSheet("QWidget { background-color: #8A2BE2; }");
    
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    
    // Контейнер для аватарки с центрированием
    QWidget *avatarContainer = new QWidget();
    QHBoxLayout *avatarLayout = new QHBoxLayout(avatarContainer);
    avatarLayout->setAlignment(Qt::AlignCenter);
    avatarLayout->setContentsMargins(0, 0, 0, 0);
    
    // Аватарка как кнопка (для смены)
    avatarButton = new QPushButton();
    avatarButton->setFixedSize(100, 100);
    avatarButton->setStyleSheet(
        "QPushButton {"
        "   background-color: white;"
        "   border-radius: 50px;"
        "   border: 3px solid white;"
        "}"
        "QPushButton:hover {"
        "   border: 3px solid #FFD700;"
        "}"
        "QPushButton:pressed {"
        "   border: 3px solid #FFA500;"
        "}"
    );
    avatarButton->setCursor(Qt::PointingHandCursor);
    
    // Устанавливаем стандартную аватарку
    setupAvatar();
    
    avatarLayout->addWidget(avatarButton);
    
    usernameLabel = new QLabel(currentUsername);
    usernameLabel->setStyleSheet("QLabel { color: white; font-size: 16px; font-weight: bold; margin-top: 10px; }");
    usernameLabel->setAlignment(Qt::AlignCenter);
    
    // Кнопки навигации
    profileBtn = new QPushButton("Профиль");
    messagesBtn = new QPushButton("Сообщения");
    friendsBtn = new QPushButton("Друзья");
    notificationsBtn = new QPushButton("Уведомления");
    playlistBtn = new QPushButton("Мой плейлист");
    musicSearchBtn = new QPushButton("🎵 Поиск музыки");
    
    QString buttonStyle = 
        "QPushButton {"
        "   background-color: transparent;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px;"
        "   text-align: left;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #6A1B9A;"
        "}";
    
    profileBtn->setStyleSheet(buttonStyle);
    messagesBtn->setStyleSheet(buttonStyle);
    friendsBtn->setStyleSheet(buttonStyle);
    notificationsBtn->setStyleSheet(buttonStyle);
    playlistBtn->setStyleSheet(buttonStyle);
    musicSearchBtn->setStyleSheet(buttonStyle);
    
    // Компоновка
    sidebarLayout->addWidget(avatarContainer);
    sidebarLayout->addWidget(usernameLabel);
    sidebarLayout->addSpacing(20);
    sidebarLayout->addWidget(profileBtn);
    sidebarLayout->addWidget(messagesBtn);
    sidebarLayout->addWidget(friendsBtn);
    sidebarLayout->addWidget(notificationsBtn);
    sidebarLayout->addWidget(playlistBtn);
    sidebarLayout->addWidget(musicSearchBtn);
    sidebarLayout->addStretch();
}

void MainWindow::createPages()
{
    mainStack = new QStackedWidget();
    
    // Существующие страницы
    profilePage = new QWidget();
    QVBoxLayout *profileLayout = new QVBoxLayout(profilePage);
    QLabel *profileLabel = new QLabel("Страница профиля\n\nПользователь: " + currentUsername);
    profileLabel->setAlignment(Qt::AlignCenter);
    profileLabel->setStyleSheet("QLabel { font-size: 18px; color: #333; }");
    profileLayout->addWidget(profileLabel);

    messagesPage = new QWidget();
    QVBoxLayout *messagesLayout = new QVBoxLayout(messagesPage);
    QLabel *messagesLabel = new QLabel("Страница сообщений\n\nЗдесь будут ваши сообщения");
    messagesLabel->setAlignment(Qt::AlignCenter);
    messagesLabel->setStyleSheet("QLabel { font-size: 18px; color: #333; }");
    messagesLayout->addWidget(messagesLabel);
    
    friendsPage = new QWidget();
    QVBoxLayout *friendsLayout = new QVBoxLayout(friendsPage);
    QLabel *friendsLabel = new QLabel("Страница друзей\n\nЗдесь будет список ваших друзей");
    friendsLabel->setAlignment(Qt::AlignCenter);
    friendsLabel->setStyleSheet("QLabel { font-size: 18px; color: #333; }");
    friendsLayout->addWidget(friendsLabel);
    
    notificationsPage = new QWidget();
    QVBoxLayout *notificationsLayout = new QVBoxLayout(notificationsPage);
    QLabel *notificationsLabel = new QLabel("Страница уведомлений\n\nЗдесь будут ваши уведомления");
    notificationsLabel->setAlignment(Qt::AlignCenter);
    notificationsLabel->setStyleSheet("QLabel { font-size: 18px; color: #333; }");
    notificationsLayout->addWidget(notificationsLabel);
    
    playlistPage = new QWidget();
    QVBoxLayout *playlistLayout = new QVBoxLayout(playlistPage);
    QLabel *playlistLabel = new QLabel("Мой плейлист\n\nЗдесь будет ваш плейлист");
    playlistLabel->setAlignment(Qt::AlignCenter);
    playlistLabel->setStyleSheet("QLabel { font-size: 18px; color: #333; }");
    playlistLayout->addWidget(playlistLabel);
    
    // Новая страница для поиска музыки
    createMusicPage();
    
    // Добавляем все страницы
    mainStack->addWidget(profilePage);
    mainStack->addWidget(messagesPage);
    mainStack->addWidget(friendsPage);
    mainStack->addWidget(notificationsPage);
    mainStack->addWidget(playlistPage);
    mainStack->addWidget(musicPage);
}

void MainWindow::createMusicPage()
{
    musicPage = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(musicPage);
    
    // Заголовок
    QLabel *titleLabel = new QLabel("🎵 Поиск музыки через Last.fm");
    titleLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #8A2BE2; margin: 10px; }");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Панель поиска
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Введите название трека или исполнителя...");
    searchInput->setStyleSheet("QLineEdit { padding: 8px; font-size: 14px; border: 2px solid #8A2BE2; border-radius: 5px; }");
    
    searchButton = new QPushButton("🔍 Поиск");
    searchButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 16px;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #7B1FA2; }"
        "QPushButton:pressed { background-color: #6A1B9A; }"
    );
    
    topTracksButton = new QPushButton("🔥 Популярные треки");
    topTracksButton->setStyleSheet(searchButton->styleSheet());
    
    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(topTracksButton);
    mainLayout->addLayout(searchLayout);
    
    // Список треков и информация
    QHBoxLayout *contentLayout = new QHBoxLayout();
    
    tracksList = new QListWidget();
    tracksList->setStyleSheet(
        "QListWidget {"
        "   border: 2px solid #8A2BE2;"
        "   border-radius: 5px;"
        "   background-color: white;"
        "   font-size: 14px;"
        "}"
        "QListWidget::item {"
        "   padding: 8px;"
        "   border-bottom: 1px solid #E0E0E0;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #8A2BE2;"
        "   color: white;"
        "}"
    );
    
    trackInfo = new QTextEdit();
    trackInfo->setReadOnly(true);
    trackInfo->setStyleSheet(
        "QTextEdit {"
        "   border: 2px solid #8A2BE2;"
        "   border-radius: 5px;"
        "   background-color: white;"
        "   font-size: 14px;"
        "   padding: 10px;"
        "}"
    );
    trackInfo->setPlaceholderText("Выберите трек для просмотра информации...");
    
    contentLayout->addWidget(tracksList, 2);
    contentLayout->addWidget(trackInfo, 1);
    mainLayout->addLayout(contentLayout, 1);
}

void MainWindow::setupConnections()
{
    connect(profileBtn, &QPushButton::clicked, this, &MainWindow::showProfilePage);
    connect(messagesBtn, &QPushButton::clicked, this, &MainWindow::showMessagesPage);
    connect(friendsBtn, &QPushButton::clicked, this, &MainWindow::showFriendsPage);
    connect(notificationsBtn, &QPushButton::clicked, this, &MainWindow::showNotificationsPage);
    connect(playlistBtn, &QPushButton::clicked, this, &MainWindow::showPlaylistPage);
    connect(musicSearchBtn, &QPushButton::clicked, this, [this]() { mainStack->setCurrentIndex(MUSIC_PAGE); });
    
    // Соединение для смены аватарки
    connect(avatarButton, &QPushButton::clicked, this, &MainWindow::changeAvatar);
    
    // Соединения для музыки
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchMusic);
    connect(topTracksButton, &QPushButton::clicked, this, &MainWindow::showTopTracks);
    connect(searchInput, &QLineEdit::returnPressed, this, &MainWindow::searchMusic);
    connect(apiManager, &MusicAPIManager::tracksFound, this, &MainWindow::onTracksFound);
    connect(tracksList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        trackInfo->setPlainText(item->data(Qt::UserRole).toString());
    });
}

void MainWindow::showProfilePage() { mainStack->setCurrentIndex(PROFILE_PAGE); }
void MainWindow::showMessagesPage() { mainStack->setCurrentIndex(MESSAGES_PAGE); }
void MainWindow::showFriendsPage() { mainStack->setCurrentIndex(FRIENDS_PAGE); }
void MainWindow::showNotificationsPage() { mainStack->setCurrentIndex(NOTIFICATIONS_PAGE); }
void MainWindow::showPlaylistPage() { mainStack->setCurrentIndex(PLAYLIST_PAGE); }

void MainWindow::searchMusic()
{
    QString query = searchInput->text().trimmed();
    if (query.isEmpty()) {
        trackInfo->setPlainText("❌ Введите поисковый запрос!");
        return;
    }
    
    tracksList->clear();
    trackInfo->setPlainText("🔍 Поиск треков...");
    
    qDebug() << "Searching for:" << query;
    apiManager->searchTracks(query, currentUserId);
}

void MainWindow::showTopTracks()
{
    tracksList->clear();
    trackInfo->setPlainText("📊 Загрузка популярных треков...");
    
    qDebug() << "Loading top tracks";
    apiManager->getTopTracks(currentUserId);
}

void MainWindow::onTracksFound(const QVariantList& tracks)
{
    tracksList->clear();
    
    if (tracks.isEmpty()) {
        trackInfo->setPlainText("❌ Треки не найдены!");
        return;
    }
    
    for (const QVariant &trackVar : tracks) {
        QVariantMap track = trackVar.toMap();
        QString title = track["title"].toString();
        QString artist = track["artist"].toString();
        QString listeners = track.value("listeners", "N/A").toString();
        QString coverUrl = track.value("coverUrl", "").toString();
        
        QString itemText = QString("🎵 %1 - %2").arg(title).arg(artist);
        if (listeners != "N/A") {
            itemText += QString(" [👥 %1]").arg(listeners);
        }
        
        QListWidgetItem *item = new QListWidgetItem(itemText, tracksList);
        
        // Сохраняем полную информацию для отображения
        QString trackInfoText = QString(
            "🎵 Трек: %1\n"
            "👨‍🎤 Исполнитель: %2\n"
            "👥 Прослушиваний: %3\n"
            "🖼️ Обложка: %4\n"
            "\n✅ Трек добавлен в вашу библиотеку!"
        ).arg(title).arg(artist).arg(listeners).arg(coverUrl.isEmpty() ? "Нет" : "Есть");
        
        item->setData(Qt::UserRole, trackInfoText);
    }
    
    trackInfo->setPlainText(QString("✅ Найдено треков: %1\n\nВыберите трек для подробной информации.").arg(tracks.size()));
    qDebug() << "Tracks displayed:" << tracks.size();
}

void MainWindow::loadUserAvatar()
{
    // Загружаем аватарку из файловой системы
    QString userAvatarPath = QString("avatars/user_%1.png").arg(currentUserId);
    QFile avatarFile(userAvatarPath);
    
    if (avatarFile.exists()) {
        QPixmap avatarPixmap(userAvatarPath);
        if (!avatarPixmap.isNull()) {
            setAvatarPixmap(avatarPixmap);
            return;
        }
    }
    
    // Если аватарки нет, используем стандартную
    setDefaultAvatar();
}

void MainWindow::setupAvatar()
{
    // Устанавливаем эмодзи камеры
    avatarButton->setText("📷");
    avatarButton->setStyleSheet(avatarButton->styleSheet() + 
        "QPushButton { font-size: 30px; color: #8A2BE2; }");
}

void MainWindow::setAvatarPixmap(const QPixmap& pixmap)
{
    // Масштабируем и обрезаем изображение для круглой аватарки
    QPixmap scaledPixmap = pixmap.scaled(90, 90, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    
    // Создаем круглую маску
    QPixmap circularPixmap(90, 90);
    circularPixmap.fill(Qt::transparent);
    
    QPainter painter(&circularPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Рисуем круглую маску
    QPainterPath path;
    path.addEllipse(0, 0, 90, 90);
    painter.setClipPath(path);
    
    // Центрируем изображение
    int x = (90 - scaledPixmap.width()) / 2;
    int y = (90 - scaledPixmap.height()) / 2;
    painter.drawPixmap(x, y, scaledPixmap);
    
    avatarButton->setIcon(QIcon(circularPixmap));
    avatarButton->setIconSize(QSize(90, 90));
    avatarButton->setText(""); // Убираем текст если был
}

void MainWindow::setDefaultAvatar()
{
    // Создаем красивую стандартную аватарку с инициалами
    QPixmap defaultPixmap(90, 90);
    defaultPixmap.fill(Qt::white);
    
    QPainter painter(&defaultPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Рисуем круглый фон
    painter.setBrush(QBrush(QColor("#8A2BE2")));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 90, 90);
    
    // Рисуем инициалы пользователя
    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    
    QString initials = currentUsername.left(2).toUpper();
    QRect textRect(0, 0, 90, 90);
    painter.drawText(textRect, Qt::AlignCenter, initials);
    
    avatarButton->setIcon(QIcon(defaultPixmap));
    avatarButton->setIconSize(QSize(90, 90));
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
        
        // Сохраняем аватарку
        saveAvatar(newAvatar);
        setAvatarPixmap(newAvatar);
        
        QMessageBox::information(this, "Успех", "Аватарка успешно изменена!");
    }
}

void MainWindow::saveAvatar(const QPixmap& avatar)
{
    // Создаем папку для аватарок если её нет
    QDir().mkpath("avatars");
    
    // Сохраняем аватарку в файл
    QString filePath = QString("avatars/user_%1.png").arg(currentUserId);
    avatar.save(filePath, "PNG");
}