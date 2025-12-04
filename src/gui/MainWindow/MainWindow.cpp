#include <QVBoxLayout>
#include <QBitmap>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QLineEdit>
#include <QListWidget>
#include <QTextEdit>
#include <QListWidgetItem>
#include <QRadialGradient>
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
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QScrollBar>
#include <QDebug>

#include "MainWindow.h"
#include "../../database/DatabaseManager.h"
#include "../../database/api/MusicAPIManager.h"
#include "../../audio/AudioPlayer.h"
#include "../../core/Track.h"
#include "../Frameless/FramelessWindow.h"

// Вспомогательная функция для создания иконок статуса
QIcon createStatusIcon(const QString& color) {
    QPixmap pixmap(8, 8);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(color));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 8, 8);
    return QIcon(pixmap);
}

class TrackListWidgetItem : public QListWidgetItem {
public:
    TrackListWidgetItem(const QString& text, int trackId, QListWidget* parent = nullptr)
        : QListWidgetItem(text, parent), m_trackId(trackId) {}
    
    int trackId() const { return m_trackId; }
    
private:
    int m_trackId;
};

MainWindow::MainWindow(const QString& username, int userId, QWidget *parent)
    : FramelessWindow(parent)
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
    , roomsBtn(nullptr)
    , mainStack(nullptr)
    , profilePage(nullptr)
    , messagesPage(nullptr)
    , friendsPage(nullptr)
    , notificationsPage(nullptr)
    , playlistPage(nullptr)
    , musicPage(nullptr)
    , myMusicPage(nullptr)
    , roomsPage(nullptr)
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
    setupAvatar(); 
    loadUserAvatar();
    loadUserTracks();
    
    setWindowTitle("SoundSpace - " + username);
    
    // Устанавливаем глобальные стили
    QFile styleFile(":/styles/styles.css");
    if (styleFile.open(QIODevice::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        setStyleSheet(styleSheet);
    }
    
    // Устанавливаем eventFilter для аватарки
    avatarButton->installEventFilter(this);
    
    // Увеличиваем минимальные размеры
    setMinimumSize(1200, 750);
    
    // Устанавливаем оптимальный начальный размер
    resize(1400, 850);
    
    // Устанавливаем шрифт
    QFont font = QApplication::font();
    font.setPointSize(10);
    QApplication::setFont(font);
}

MainWindow::~MainWindow()
{
    // Деструктор MainWindow
    // Qt автоматически удалит все дочерние виджеты
    // так как они созданы с this в качестве родителя
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == avatarButton) {
        if (event->type() == QEvent::Enter) {
            showAvatarOverlay();
            return true;
        } else if (event->type() == QEvent::Leave) {
            hideAvatarOverlay();
            return true;
        }
    }
    
    // Обработка клика на оверлее
    if (obj == avatarOverlay) {
        if (event->type() == QEvent::MouseButtonPress) {
            changeAvatar();
            hideAvatarOverlay();
            return true;
        }
    }
    
    return FramelessWindow::eventFilter(obj, event);
}
void MainWindow::onAvatarButtonEnter()
{
    showAvatarOverlay();
}

void MainWindow::onAvatarButtonLeave()
{
    hideAvatarOverlay();
}

void MainWindow::showAvatarOverlay()
{
    if (!avatarOverlay) {
        // Создаем как дочерний виджет кнопки
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
        
        // Устанавливаем тот же размер, что и у аватарки
        avatarOverlay->setFixedSize(100, 100);
        
        // Позиционируем в (0,0) относительно аватарки
        avatarOverlay->move(0, 0);
        
        // Важно: делаем оверлей "прозрачным" для событий мыши
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

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget();
    centralWidget->setObjectName("windowContent");
    centralWidget->setStyleSheet(R"(
        #windowContent {
            background-color: #0F0F14;
            border-radius: 0;
        }
    )");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Панель заголовка
    QWidget *titleBar = new QWidget();
    titleBar->setFixedHeight(40);
    titleBar->setStyleSheet(R"(
        QWidget {
            background: rgba(15, 15, 20, 0.8);
            border-top-left-radius: 0;
            border-top-right-radius: 0;
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
        }
    )");
    
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(20, 0, 20, 0);
    
    QLabel *titleLabel = new QLabel("SoundSpace");
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #8A2BE2;
            font-size: 16px;
            font-weight: bold;
            opacity: 0.9;
        }
    )");
    
    QWidget *windowButtons = new QWidget();
    QHBoxLayout *buttonsLayout = new QHBoxLayout(windowButtons);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(5);
    
    QPushButton *minimizeBtn = new QPushButton("—");
    QPushButton *maximizeBtn = new QPushButton("□");
    QPushButton *closeBtn = new QPushButton("×");
    
    QString windowBtnStyle = R"(
        QPushButton {
            background: transparent;
            color: rgba(255, 255, 255, 0.6);
            border: none;
            font-size: 16px;
            padding: 0;
            min-width: 28px;
            min-height: 28px;
            border-radius: 4px;
            font-weight: 300;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.1);
            color: white;
        }
        QPushButton#closeBtn:hover {
            background: #FF4444;
            color: white;
        }
    )";
    
    minimizeBtn->setStyleSheet(windowBtnStyle);
    maximizeBtn->setStyleSheet(windowBtnStyle);
    closeBtn->setStyleSheet(windowBtnStyle);
    closeBtn->setObjectName("closeBtn");
    
    buttonsLayout->addWidget(minimizeBtn);
    buttonsLayout->addWidget(maximizeBtn);
    buttonsLayout->addWidget(closeBtn);
    
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(windowButtons);
    
    connect(minimizeBtn, &QPushButton::clicked, this, &QMainWindow::showMinimized);
    connect(closeBtn, &QPushButton::clicked, this, &QMainWindow::close);
    connect(maximizeBtn, &QPushButton::clicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });
    
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    
    createSidebar();
    contentLayout->addWidget(sidebar);
    
    createPages();
    contentLayout->addWidget(mainStack, 1);
    
    mainLayout->addWidget(titleBar);
    mainLayout->addLayout(contentLayout, 1);
    
    audioPlayer->setupPlayerControls(centralWidget);
    mainLayout->addWidget(audioPlayer->getPlayerControls());
    
    setCentralWidget(centralWidget);
}

void MainWindow::createSidebar()
{
    sidebar = new QWidget();
    sidebar->setFixedWidth(280);
    sidebar->setStyleSheet(R"(
        QWidget {
            background: #0F0F14;
            border-right: 1px solid rgba(255, 255, 255, 0.05);
        }
    )");
    
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    
    // Верхняя часть с логотипом
    QWidget *logoSection = new QWidget();
    logoSection->setFixedHeight(70);
    logoSection->setStyleSheet(R"(
        QWidget {
            background: rgba(138, 43, 226, 0.1);
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
        }
    )");
    
    QHBoxLayout *logoLayout = new QHBoxLayout(logoSection);
    logoLayout->setContentsMargins(20, 0, 20, 0);
    
    QLabel *logoLabel = new QLabel("SoundSpace");
    logoLabel->setStyleSheet(R"(
        QLabel {
            color: #8A2BE2;
            font-size: 22px;
            font-weight: 800;
        }
    )");
    
    logoLayout->addWidget(logoLabel);
    sidebarLayout->addWidget(logoSection);
    
    // Профиль пользователя
    QWidget *profileSection = new QWidget();
    profileSection->setStyleSheet(R"(
        QWidget {
            background: transparent;
            padding: 20px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
        }
    )");
    
    QVBoxLayout *profileLayout = new QVBoxLayout(profileSection);
    profileLayout->setContentsMargins(0, 0, 0, 0);
    profileLayout->setSpacing(15);
    profileLayout->setAlignment(Qt::AlignTop);
    
    // Аватар
    avatarButton = new QPushButton();
    avatarButton->setFixedSize(100, 100);
    avatarButton->setStyleSheet(R"(
        QPushButton {
            border: 3px solid #8A2BE2;
            border-radius: 50px;
            padding: 0;
            background: transparent;
        }
        QPushButton:hover {
            border-color: #9B4BFF;
            background: rgba(138, 43, 226, 0.1);
        }
    )");
    
    QGraphicsDropShadowEffect *avatarEffect = new QGraphicsDropShadowEffect();
    avatarEffect->setBlurRadius(20);
    avatarEffect->setColor(QColor(138, 43, 226, 100));
    avatarEffect->setOffset(0, 0);
    avatarButton->setGraphicsEffect(avatarEffect);
    
    // Ник пользователя
    usernameLabel = new QLabel(currentUsername);
    usernameLabel->setStyleSheet(R"(
        QLabel {
            color: #FFFFFF;
            font-size: 18px;
            font-weight: 600;
            text-align: center;
            padding: 10px 0 5px 0;
        }
    )");
    usernameLabel->setWordWrap(true);
    usernameLabel->setMaximumWidth(260);
    
    // Статус - используем QToolButton для меню
    QToolButton *statusButton = new QToolButton();
    statusButton->setText("Онлайн");
    statusButton->setObjectName("statusButton");
    statusButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    statusButton->setArrowType(Qt::NoArrow);
    statusButton->setStyleSheet(R"(
        QToolButton {
            color: #4CAF50;
            font-size: 14px;
            font-weight: 500;
            background: transparent;
            border: 1px solid rgba(76, 175, 80, 0.3);
            border-radius: 12px;
            padding: 6px 15px;
            margin: 0 70px;
        }
        QToolButton:hover {
            background: rgba(76, 175, 80, 0.1);
        }
        QToolButton::menu-indicator {
            width: 0px;
        }
    )");
    statusButton->setCursor(Qt::PointingHandCursor);
    
    profileLayout->addWidget(avatarButton, 0, Qt::AlignHCenter);
    profileLayout->addWidget(usernameLabel, 0, Qt::AlignHCenter);
    profileLayout->addWidget(statusButton, 0, Qt::AlignHCenter);
    sidebarLayout->addWidget(profileSection);
    
    // Навигация
    QWidget *navSection = new QWidget();
    navSection->setStyleSheet("background: transparent;");
    
    QVBoxLayout *navLayout = new QVBoxLayout(navSection);
    navLayout->setContentsMargins(15, 20, 15, 20);
    navLayout->setSpacing(3);
    
    QString navButtonStyle = R"(
        QPushButton {
            background: transparent;
            color: rgba(255, 255, 255, 0.8);
            border: none;
            text-align: left;
            padding: 12px 20px;
            font-size: 14px;
            font-weight: 500;
            border-radius: 8px;
            min-height: 45px;
        }
        QPushButton:hover {
            color: #FFFFFF;
            background: rgba(138, 43, 226, 0.15);
            padding-left: 25px;
        }
        QPushButton:pressed {
            background: rgba(138, 43, 226, 0.25);
        }
    )";
    
    // Создаем навигационные кнопки
    myMusicBtn = new QPushButton("Моя музыка");
    profileBtn = new QPushButton("Профиль");
    musicSearchBtn = new QPushButton("Поиск музыки");
    playlistBtn = new QPushButton("Мои плейлисты");
    friendsBtn = new QPushButton("Друзья");
    messagesBtn = new QPushButton("Сообщения");
    notificationsBtn = new QPushButton("Уведомления");
    roomsBtn = new QPushButton("Комнаты");
    
    QList<QPushButton*> navButtons = {
        myMusicBtn, profileBtn, musicSearchBtn, playlistBtn,
        friendsBtn, messagesBtn, notificationsBtn, roomsBtn
    };
    
    for (auto btn : navButtons) {
        btn->setStyleSheet(navButtonStyle);
        btn->setCursor(Qt::PointingHandCursor);
        navLayout->addWidget(btn);
    }
    
    navLayout->addStretch();
    sidebarLayout->addWidget(navSection, 1);
    
    // Нижняя часть
    QWidget *bottomSection = new QWidget();
    bottomSection->setStyleSheet(R"(
        QWidget {
            background: rgba(0, 0, 0, 0.2);
            padding: 20px;
            border-top: 1px solid rgba(255, 255, 255, 0.05);
        }
    )");
    
    // Кнопка выхода
    QPushButton *logoutBtn = new QPushButton("Выйти");
    logoutBtn->setObjectName("logoutBtn");
    logoutBtn->setStyleSheet(R"(
        QPushButton {
            background: rgba(255, 68, 68, 0.1);
            border: 1px solid rgba(255, 68, 68, 0.3);
            color: rgba(255, 255, 255, 0.9);
            border-radius: 8px;
            padding: 12px;
            font-size: 14px;
            font-weight: 500;
            width: 100%;
            min-height: 45px;
        }
        QPushButton:hover {
            background: rgba(255, 68, 68, 0.2);
            color: #FFFFFF;
        }
    )");
    connect(logoutBtn, &QPushButton::clicked, this, &QMainWindow::close);
    
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomSection);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->addWidget(logoutBtn);
    
    sidebarLayout->addWidget(bottomSection);
    
    // Контекстное меню для статуса
    QMenu *statusMenu = new QMenu(this);
    statusMenu->setStyleSheet(R"(
        QMenu {
            background-color: #1A1A21;
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 8px;
            padding: 5px;
        }
        QMenu::item {
            background-color: transparent;
            color: rgba(255, 255, 255, 0.8);
            padding: 8px 25px;
            border-radius: 4px;
            margin: 2px;
            font-size: 14px;
            min-height: 30px;
        }
        QMenu::item:selected {
            background-color: rgba(138, 43, 226, 0.3);
            color: #8A2BE2;
        }
    )");
    
    QAction *onlineAction = new QAction("Онлайн", this);
    QAction *awayAction = new QAction("Нет на месте", this);
    QAction *dndAction = new QAction("Не беспокоить", this);
    QAction *invisibleAction = new QAction("Невидимка", this);
    
    statusMenu->addAction(onlineAction);
    statusMenu->addAction(awayAction);
    statusMenu->addAction(dndAction);
    statusMenu->addAction(invisibleAction);
    
    // Устанавливаем меню для кнопки
    statusButton->setMenu(statusMenu);
    statusButton->setPopupMode(QToolButton::InstantPopup);
    
    // Обработчики изменения статуса
    connect(onlineAction, &QAction::triggered, [statusButton]() {
        statusButton->setText("Онлайн");
        statusButton->setStyleSheet(R"(
            QToolButton {
                color: #4CAF50;
                font-size: 14px;
                font-weight: 500;
                background: transparent;
                border: 1px solid rgba(76, 175, 80, 0.3);
                border-radius: 12px;
                padding: 6px 15px;
                margin: 0 70px;
            }
            QToolButton:hover {
                background: rgba(76, 175, 80, 0.1);
            }
            QToolButton::menu-indicator {
                width: 0px;
            }
        )");
    });
    
    connect(awayAction, &QAction::triggered, [statusButton]() {
        statusButton->setText("Нет на месте");
        statusButton->setStyleSheet(R"(
            QToolButton {
                color: #FFC107;
                font-size: 14px;
                font-weight: 500;
                background: transparent;
                border: 1px solid rgba(255, 193, 7, 0.3);
                border-radius: 12px;
                padding: 6px 15px;
                margin: 0 70px;
            }
            QToolButton:hover {
                background: rgba(255, 193, 7, 0.1);
            }
            QToolButton::menu-indicator {
                width: 0px;
            }
        )");
    });
    
    connect(dndAction, &QAction::triggered, [statusButton]() {
        statusButton->setText("Не беспокоить");
        statusButton->setStyleSheet(R"(
            QToolButton {
                color: #F44336;
                font-size: 14px;
                font-weight: 500;
                background: transparent;
                border: 1px solid rgba(244, 67, 54, 0.3);
                border-radius: 12px;
                padding: 6px 15px;
                margin: 0 70px;
            }
            QToolButton:hover {
                background: rgba(244, 67, 54, 0.1);
            }
            QToolButton::menu-indicator {
                width: 0px;
            }
        )");
    });
    
    connect(invisibleAction, &QAction::triggered, [statusButton]() {
        statusButton->setText("Невидимка");
        statusButton->setStyleSheet(R"(
            QToolButton {
                color: #9E9E9E;
                font-size: 14px;
                font-weight: 500;
                background: transparent;
                border: 1px solid rgba(158, 158, 158, 0.3);
                border-radius: 12px;
                padding: 6px 15px;
                margin: 0 70px;
            }
            QToolButton:hover {
                background: rgba(158, 158, 158, 0.1);
            }
            QToolButton::menu-indicator {
                width: 0px;
            }
        )");
    });
}

void MainWindow::createRoomsPage()
{
    roomsPage = new QWidget();
    roomsPage->setObjectName("roomsPage");
    roomsPage->setStyleSheet(R"(
        #roomsPage {
            background: #0F0F14;
        }
    )");
    
    // Главный контейнер с правильной структурой
    QVBoxLayout *mainLayout = new QVBoxLayout(roomsPage);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(25);
    
    // =========== HEADER ===========
    QWidget *headerWidget = new QWidget();
    headerWidget->setStyleSheet("background: transparent;");
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(10);
    
    QLabel *titleLabel = new QLabel("Комнаты");
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #FFFFFF;
            font-size: 28px;
            font-weight: 700;
            font-family: 'Segoe UI', 'Arial', sans-serif;
        }
    )");
    
    QLabel *subtitleLabel = new QLabel("Совместное прослушивание музыки");
    subtitleLabel->setStyleSheet(R"(
        QLabel {
            color: rgba(255, 255, 255, 0.6);
            font-size: 15px;
            font-family: 'Segoe UI', 'Arial', sans-serif;
        }
    )");
    
    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(subtitleLabel);
    mainLayout->addWidget(headerWidget);
    
    // =========== CONTROLS ===========
    QWidget *controlsWidget = new QWidget();
    controlsWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setSpacing(15);
    
    QLineEdit *searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Поиск комнат...");
    searchInput->setMinimumHeight(45);
    searchInput->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.05);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 0 20px;
            color: white;
            font-size: 14px;
            font-family: 'Segoe UI', 'Arial', sans-serif;
        }
        QLineEdit:focus {
            border: 1px solid #8A2BE2;
        }
    )");
    
    QPushButton *createRoomBtn = new QPushButton("Создать комнату");
    createRoomBtn->setMinimumHeight(45);
    createRoomBtn->setMinimumWidth(160);
    createRoomBtn->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 600;
            font-family: 'Segoe UI', 'Arial', sans-serif;
            padding: 0 25px;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
    )");
    
    controlsLayout->addWidget(searchInput, 1);
    controlsLayout->addWidget(createRoomBtn);
    mainLayout->addWidget(controlsWidget);
    
    // =========== SEPARATOR ===========
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("border: 1px solid rgba(255, 255, 255, 0.05);");
    mainLayout->addWidget(separator);
    
    // =========== ROOMS GRID ===========
    // Контейнер с прокруткой
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: rgba(255, 255, 255, 0.05);
            width: 6px;
            border-radius: 3px;
        }
        QScrollBar::handle:vertical {
            background: rgba(138, 43, 226, 0.3);
            border-radius: 3px;
            min-height: 20px;
        }
    )");
    
    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    
    QGridLayout *roomsGrid = new QGridLayout(scrollContent);
    roomsGrid->setSpacing(20);
    roomsGrid->setContentsMargins(0, 10, 0, 10);
    
    // Данные комнат
    struct RoomData {
        QString name;
        QString icon;
        QString details;
        QString currentSong;
        int users;
        int maxUsers;
    };
    
    QVector<RoomData> rooms = {
        {"Вечерний лофти", "🎵", "Джаз • Соул", "Frank Sinatra - Fly Me To The Moon", 4, 10},
        {"Ретро волна", "🎵", "80s • Диско", "Michael Jackson - Billie Jean", 6, 12},
        {"Инди-собрание", "🎵", "Инди-рок", "Arctic Monkeys - Do I Wanna Know?", 3, 8},
        {"Хип-хоп лаунж", "🎵", "Рэп • Хип-хоп", "Kendrick Lamar - HUMBLE.", 8, 15},
        {"Электронная вечеринка", "🎵", "Техно • Хаус", "Daft Punk - Around the World", 12, 20},
        {"Акустическая гостиная", "🎵", "Фолк • Акустика", "Ed Sheeran - Perfect", 2, 6}
    };
    
    for (int i = 0; i < rooms.size(); ++i) {
        const RoomData &room = rooms[i];
        
        // Карточка комнаты
        QWidget *roomCard = new QWidget();
        roomCard->setFixedHeight(160);
        
        // Основной контейнер карточки
        QVBoxLayout *cardLayout = new QVBoxLayout(roomCard);
        cardLayout->setContentsMargins(20, 15, 20, 15);
        cardLayout->setSpacing(10);
        
        // Верхняя строка: название + пользователи
        QWidget *topRow = new QWidget();
        QHBoxLayout *topRowLayout = new QHBoxLayout(topRow);
        topRowLayout->setContentsMargins(0, 0, 0, 0);
        topRowLayout->setSpacing(10);
        
        QLabel *iconLabel = new QLabel(room.icon);
        iconLabel->setStyleSheet(R"(
            QLabel {
                color: #8A2BE2;
                font-size: 16px;
            }
        )");
        
        QLabel *nameLabel = new QLabel(room.name);
        nameLabel->setStyleSheet(R"(
            QLabel {
                color: #FFFFFF;
                font-size: 16px;
                font-weight: 600;
                font-family: 'Segoe UI', 'Arial', sans-serif;
            }
        )");
        nameLabel->setWordWrap(true);
        
        QLabel *usersLabel = new QLabel(QString("%1/%2").arg(room.users).arg(room.maxUsers));
        usersLabel->setStyleSheet(R"(
            QLabel {
                color: rgba(255, 255, 255, 0.7);
                font-size: 13px;
                background: rgba(255, 255, 255, 0.05);
                padding: 4px 10px;
                border-radius: 10px;
                font-family: 'Segoe UI', 'Arial', sans-serif;
            }
        )");
        usersLabel->setAlignment(Qt::AlignCenter);
        
        topRowLayout->addWidget(iconLabel);
        topRowLayout->addWidget(nameLabel, 1);
        topRowLayout->addWidget(usersLabel);
        
        // Детали комнаты
        QLabel *detailsLabel = new QLabel(room.details);
        detailsLabel->setStyleSheet(R"(
            QLabel {
                color: #8A2BE2;
                font-size: 14px;
                font-weight: 500;
                font-family: 'Segoe UI', 'Arial', sans-serif;
            }
        )");
        
        // Текущий трек
        QLabel *songLabel = new QLabel(room.currentSong);
        songLabel->setStyleSheet(R"(
            QLabel {
                color: rgba(255, 255, 255, 0.7);
                font-size: 13px;
                font-style: italic;
                font-family: 'Segoe UI', 'Arial', sans-serif;
            }
        )");
        songLabel->setWordWrap(true);
        songLabel->setMaximumHeight(34);
        
        // Кнопка присоединения с фиксированным текстом
        QPushButton *joinBtn = new QPushButton("Присоединиться");
        joinBtn->setFixedHeight(35);
        joinBtn->setStyleSheet(R"(
            QPushButton {
                background: rgba(138, 43, 226, 0.2);
                border: 1px solid #8A2BE2;
                color: #8A2BE2;
                border-radius: 8px;
                font-size: 14px;
                font-weight: 600;
                font-family: 'Segoe UI', 'Arial', sans-serif;
            }
            QPushButton:hover {
                background: rgba(138, 43, 226, 0.3);
            }
        )");
        joinBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        
        // Добавляем элементы в карточку
        cardLayout->addWidget(topRow);
        cardLayout->addWidget(detailsLabel);
        cardLayout->addWidget(songLabel, 1);
        cardLayout->addWidget(joinBtn);
        
        // Стиль карточки
        roomCard->setStyleSheet(R"(
            QWidget {
                background: rgba(255, 255, 255, 0.03);
                border: 1px solid rgba(255, 255, 255, 0.05);
                border-radius: 12px;
            }
            QWidget:hover {
                background: rgba(255, 255, 255, 0.05);
                border: 1px solid rgba(138, 43, 226, 0.3);
            }
        )");
        
        // Добавляем в сетку (2 колонки)
        int row = i / 2;
        int col = i % 2;
        roomsGrid->addWidget(roomCard, row, col, 1, 1);
        
        // Устанавливаем растяжение для равномерного распределения
        roomsGrid->setColumnStretch(col, 1);
        roomsGrid->setRowStretch(row, 0);
    }
    
    // Добавляем stretch для заполнения оставшегося пространства
    int totalRows = (rooms.size() + 1) / 2;
    for (int i = totalRows; i < 10; ++i) {
        roomsGrid->setRowStretch(i, 1);
    }
    
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea, 1);
    
    // =========== ACTIVE ROOM ===========
    QWidget *activeRoomWidget = new QWidget();
    activeRoomWidget->setStyleSheet(R"(
        QWidget {
            background: rgba(138, 43, 226, 0.1);
            border: 1px solid rgba(138, 43, 226, 0.3);
            border-radius: 12px;
            padding: 20px;
        }
    )");
    
    QHBoxLayout *activeLayout = new QHBoxLayout(activeRoomWidget);
    activeLayout->setContentsMargins(0, 0, 0, 0);
    activeLayout->setSpacing(20);
    
    QLabel *activeIcon = new QLabel("🎧");
    activeIcon->setStyleSheet("font-size: 24px;");
    
    QWidget *infoWidget = new QWidget();
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(5);
    
    QLabel *activeTitle = new QLabel("Джаз вечер");
    activeTitle->setStyleSheet(R"(
        QLabel {
            color: #FFFFFF;
            font-size: 16px;
            font-weight: 600;
            font-family: 'Segoe UI', 'Arial', sans-serif;
        }
    )");
    
    QLabel *activeDetails = new QLabel("3 участника • Frank Sinatra - Fly Me To The Moon");
    activeDetails->setStyleSheet(R"(
        QLabel {
            color: rgba(255, 255, 255, 0.7);
            font-size: 14px;
            font-family: 'Segoe UI', 'Arial', sans-serif;
        }
    )");
    
    infoLayout->addWidget(activeTitle);
    infoLayout->addWidget(activeDetails);
    
    QPushButton *enterBtn = new QPushButton("Войти");
    enterBtn->setFixedWidth(100);
    enterBtn->setFixedHeight(40);
    enterBtn->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
            font-family: 'Segoe UI', 'Arial', sans-serif;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
    )");
    
    activeLayout->addWidget(activeIcon);
    activeLayout->addWidget(infoWidget, 1);
    activeLayout->addWidget(enterBtn);
    
    mainLayout->addWidget(activeRoomWidget);
}

void MainWindow::createPages()
{
    mainStack = new QStackedWidget();
    mainStack->setStyleSheet(R"(
        QStackedWidget {
            background: #0F0F14;
        }
    )");
    
    // 1. Сначала создаем ВСЕ страницы
    createMyMusicPage();    // Страница 0
    createMusicPage();      // Страница 1
    createRoomsPage();      // Страница 2
    
    // 2. Простые страницы
    auto createSimplePage = [](const QString& title) -> QWidget* {
        QWidget *page = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(40, 40, 40, 40);
        
        QLabel *titleLabel = new QLabel(title);
        titleLabel->setStyleSheet(R"(
            QLabel {
                color: #FFFFFF;
                font-size: 32px;
                font-weight: bold;
                text-align: center;
                margin-bottom: 20px;
            }
        )");
        
        QLabel *descLabel = new QLabel("Эта страница в разработке");
        descLabel->setStyleSheet(R"(
            QLabel {
                color: rgba(255, 255, 255, 0.5);
                font-size: 16px;
                text-align: center;
            }
        )");
        
        layout->addStretch();
        layout->addWidget(titleLabel);
        layout->addWidget(descLabel);
        layout->addStretch();
        
        return page;
    };
    
    profilePage = createSimplePage("Профиль");
    messagesPage = createSimplePage("Сообщения");
    friendsPage = createSimplePage("Друзья");
    notificationsPage = createSimplePage("Уведомления");
    playlistPage = createSimplePage("Мои плейлисты");
    
    // 3. КРИТИЧЕСКИ ВАЖНО: добавляем в правильном порядке!
    // Порядок должен совпадать с enum PageIndex
    mainStack->addWidget(myMusicPage);      // 0: MY_MUSIC_PAGE
    mainStack->addWidget(profilePage);      // 1: PROFILE_PAGE
    mainStack->addWidget(messagesPage);     // 2: MESSAGES_PAGE
    mainStack->addWidget(friendsPage);      // 3: FRIENDS_PAGE
    mainStack->addWidget(notificationsPage); // 4: NOTIFICATIONS_PAGE
    mainStack->addWidget(playlistPage);     // 5: PLAYLIST_PAGE
    mainStack->addWidget(musicPage);        // 6: MUSIC_PAGE
    mainStack->addWidget(roomsPage);        // 7: ROOMS_PAGE
    
    // Для отладки
    qDebug() << "Создано страниц в mainStack:" << mainStack->count();
}

void MainWindow::createMyMusicPage()
{
    myMusicPage = new QWidget();
    myMusicPage->setObjectName("myMusicPage");
    myMusicPage->setStyleSheet(R"(
        #myMusicPage {
            background: #0F0F14;
        }
    )");
    
    QVBoxLayout *myMusicLayout = new QVBoxLayout(myMusicPage);
    myMusicLayout->setContentsMargins(40, 40, 40, 40);
    myMusicLayout->setSpacing(25);
    
    QLabel *myMusicTitle = new QLabel("Моя музыка");
    myMusicTitle->setStyleSheet(R"(
        QLabel {
            color: #FFFFFF;
            font-size: 32px;
            font-weight: bold;
        }
    )");
    
    // Панель управления
    QWidget *controlsWidget = new QWidget();
    controlsWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    
    refreshTracksBtn = new QPushButton("Обновить");
    addLocalTrackBtn = new QPushButton("Добавить трек");
    
    QString controlButtonStyle = R"(
        QPushButton {
            background: rgba(138, 43, 226, 0.2);
            border: 1px solid #8A2BE2;
            color: #8A2BE2;
            padding: 10px 20px;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background: rgba(138, 43, 226, 0.3);
            transform: translateY(-1px);
        }
        QPushButton:pressed {
            transform: translateY(0px);
            background: rgba(138, 43, 226, 0.4);
        }
    )";
    
    refreshTracksBtn->setStyleSheet(controlButtonStyle);
    addLocalTrackBtn->setStyleSheet(controlButtonStyle);
    
    controlsLayout->addWidget(refreshTracksBtn);
    controlsLayout->addWidget(addLocalTrackBtn);
    controlsLayout->addStretch();
    
    // Список треков
    userTracksList = new QListWidget();
    userTracksList->setStyleSheet(R"(
        QListWidget {
            background: rgba(255, 255, 255, 0.03);
            border: 2px solid rgba(255, 255, 255, 0.05);
            border-radius: 15px;
            color: white;
            font-size: 14px;
            outline: none;
            padding: 5px;
        }
        QListWidget::item {
            background: transparent;
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
            height: 60px;
            border-radius: 8px;
            margin: 2px;
        }
        QListWidget::item:selected {
            background: rgba(138, 43, 226, 0.15);
            color: white;
            border: 1px solid rgba(138, 43, 226, 0.3);
        }
        QListWidget::item:hover {
            background: rgba(255, 255, 255, 0.05);
        }
    )");
    
    myMusicLayout->addWidget(myMusicTitle);
    myMusicLayout->addWidget(controlsWidget);
    myMusicLayout->addWidget(userTracksList, 1);
}

void MainWindow::createMusicPage()
{
    musicPage = new QWidget();
    musicPage->setObjectName("musicPage");
    musicPage->setStyleSheet(R"(
        #musicPage {
            background: #0F0F14;
        }
    )");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(musicPage);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(25);
    
    QLabel *titleLabel = new QLabel("Поиск музыки");
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #FFFFFF;
            font-size: 32px;
            font-weight: bold;
        }
    )");
    mainLayout->addWidget(titleLabel);
    
    // Поисковая строка
    QWidget *searchWidget = new QWidget();
    searchWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(15);
    
    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Введите название трека, альбома или исполнителя...");
    searchInput->setMinimumHeight(50);
    searchInput->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 12px;
            padding: 0 20px;
            color: white;
            font-size: 14px;
            selection-background-color: #8A2BE2;
        }
        QLineEdit:focus {
            border: 2px solid #8A2BE2;
            background: rgba(255, 255, 255, 0.07);
        }
        QLineEdit::placeholder {
            color: rgba(255, 255, 255, 0.3);
        }
    )");
    
    searchButton = new QPushButton("Найти");
    searchButton->setFixedWidth(120);
    searchButton->setMinimumHeight(50);
    searchButton->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 12px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #9B4BFF;
            transform: translateY(-1px);
        }
        QPushButton:pressed {
            transform: translateY(0px);
            background: #7B1FA2;
        }
    )");
    
    topTracksButton = new QPushButton("🔥 Популярное");
    topTracksButton->setFixedWidth(150);
    topTracksButton->setMinimumHeight(50);
    topTracksButton->setStyleSheet(searchButton->styleSheet());
    
    searchLayout->addWidget(searchInput, 1);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(topTracksButton);
    mainLayout->addWidget(searchWidget);
    
    // Результаты поиска
    QWidget *resultsWidget = new QWidget();
    resultsWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *resultsLayout = new QHBoxLayout(resultsWidget);
    resultsLayout->setContentsMargins(0, 0, 0, 0);
    resultsLayout->setSpacing(20);
    
    // Список треков
    tracksList = new QListWidget();
    tracksList->setStyleSheet(R"(
        QListWidget {
            background: rgba(255, 255, 255, 0.03);
            border: 2px solid rgba(255, 255, 255, 0.05);
            border-radius: 15px;
            color: white;
            font-size: 14px;
            outline: none;
            padding: 10px;
        }
        QListWidget::item {
            background: transparent;
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
            padding: 15px;
            border-radius: 8px;
            margin: 2px;
        }
        QListWidget::item:selected {
            background: rgba(138, 43, 226, 0.15);
            color: white;
            border: 1px solid rgba(138, 43, 226, 0.3);
        }
        QListWidget::item:hover {
            background: rgba(255, 255, 255, 0.05);
        }
    )");
    
    // Информация о треке
    QWidget *infoWidget = new QWidget();
    infoWidget->setStyleSheet(R"(
        QWidget {
            background: rgba(255, 255, 255, 0.03);
            border: 2px solid rgba(255, 255, 255, 0.05);
            border-radius: 15px;
        }
    )");
    
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(25, 25, 25, 25);
    infoLayout->setSpacing(20);
    
    QLabel *infoTitle = new QLabel("Информация о треке");
    infoTitle->setStyleSheet(R"(
        QLabel {
            color: #FFFFFF;
            font-size: 18px;
            font-weight: 600;
        }
    )");
    
    trackInfo = new QTextEdit();
    trackInfo->setReadOnly(true);
    trackInfo->setStyleSheet(R"(
        QTextEdit {
            background: transparent;
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            color: rgba(255, 255, 255, 0.8);
            font-size: 14px;
            padding: 15px;
            selection-background-color: #8A2BE2;
        }
    )");
    trackInfo->setPlaceholderText("Выберите трек для просмотра информации...");
    
    QPushButton *addToLibraryBtn = new QPushButton("➕ Добавить в библиотеку");
    addToLibraryBtn->setStyleSheet(R"(
        QPushButton {
            background: rgba(138, 43, 226, 0.2);
            border: 1px solid #8A2BE2;
            color: #8A2BE2;
            border-radius: 8px;
            padding: 12px;
            font-weight: 600;
            font-size: 14px;
        }
        QPushButton:hover {
            background: rgba(138, 43, 226, 0.3);
        }
        QPushButton:pressed {
            background: rgba(138, 43, 226, 0.4);
        }
    )");
    addToLibraryBtn->setVisible(false);
    
    infoLayout->addWidget(infoTitle);
    infoLayout->addWidget(trackInfo, 1);
    infoLayout->addWidget(addToLibraryBtn);
    
    resultsLayout->addWidget(tracksList, 2);
    resultsLayout->addWidget(infoWidget, 1);
    mainLayout->addWidget(resultsWidget, 1);
    
    // Контекстное меню
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
            contextMenu.setStyleSheet(R"(
                QMenu {
                    background-color: #1A1A21;
                    border: 1px solid rgba(255, 255, 255, 0.1);
                    border-radius: 8px;
                    padding: 5px;
                }
                QMenu::item {
                    background-color: transparent;
                    color: rgba(255, 255, 255, 0.8);
                    padding: 8px 20px;
                    border-radius: 4px;
                    margin: 2px;
                }
                QMenu::item:selected {
                    background-color: rgba(138, 43, 226, 0.3);
                    color: #8A2BE2;
                }
                QMenu::separator {
                    height: 1px;
                    background: rgba(255, 255, 255, 0.1);
                    margin: 5px 10px;
                }
            )");
            
            QAction *addToLibraryAction = contextMenu.addAction("➕ Добавить в библиотеку");
            QAction *viewInfoAction = contextMenu.addAction("ℹ️ Просмотреть информацию");
            contextMenu.addSeparator();
            QAction *shareAction = contextMenu.addAction("↗️ Поделиться");
            
            QAction *selectedAction = contextMenu.exec(tracksList->mapToGlobal(pos));
            
            if (selectedAction == addToLibraryAction) {
                QVariant trackData = item->data(Qt::UserRole + 1);
                if (trackData.isValid()) {
                    QVariantMap track = trackData.toMap();
                    
                    bool success = DatabaseManager::instance().addTrackFromAPI(
                        currentUserId,
                        track["title"].toString(),
                        track["artist"].toString(),
                        track["coverUrl"].toString()
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

void MainWindow::showRoomsPage()
{
    mainStack->setCurrentIndex(ROOMS_PAGE);
}

void MainWindow::setupConnections()
{
    // 1. Проверяем порядок с enum
    qDebug() << "MY_MUSIC_PAGE индекс:" << MY_MUSIC_PAGE;
    qDebug() << "ROOMS_PAGE индекс:" << ROOMS_PAGE;
    
    // 2. Исправляем подключения
    connect(myMusicBtn, &QPushButton::clicked, this, [this]() { 
        qDebug() << "Нажата Моя музыка, переключаем на индекс" << MY_MUSIC_PAGE;
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
        qDebug() << "Нажата Поиск музыки, переключаем на индекс" << MUSIC_PAGE;
        mainStack->setCurrentIndex(MUSIC_PAGE); 
    });
    
    connect(roomsBtn, &QPushButton::clicked, this, [this]() { 
        qDebug() << "Нажата Комнаты, переключаем на индекс" << ROOMS_PAGE;
        mainStack->setCurrentIndex(ROOMS_PAGE); 
    });
    
    // Остальные подключения остаются как есть
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
    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление трека");
    msgBox.setText("Вы уверены, что хотите удалить этот трек из своей библиотеки?");
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
        }
        QMessageBox QPushButton:hover {
            background-color: #9B4BFF;
        }
        QMessageBox QPushButton#NoButton {
            background-color: rgba(255, 255, 255, 0.1);
        }
    )");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    
    if (msgBox.exec() == QMessageBox::Yes) {
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
    
    for (const Track &track : currentTracks) {
        QString duration = track.duration > 0 ? audioPlayer->formatTime(track.duration) : "0:00";
        QString itemText = QString("%1 - %2 (%3)").arg(track.artist).arg(track.title).arg(duration);
        
        TrackListWidgetItem *item = new TrackListWidgetItem("", track.id, userTracksList);
        item->setData(Qt::UserRole, track.id);
        item->setSizeHint(QSize(item->sizeHint().width(), 60));
        
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(15, 10, 15, 10);
        itemLayout->setSpacing(15);
        
        QLabel *trackIcon = new QLabel("🎵");
        trackIcon->setStyleSheet("font-size: 16px;");
        
        QWidget *textWidget = new QWidget();
        QVBoxLayout *textLayout = new QVBoxLayout(textWidget);
        textLayout->setContentsMargins(0, 0, 0, 0);
        textLayout->setSpacing(3);
        
        QLabel *titleLabel = new QLabel(track.title);
        titleLabel->setStyleSheet(R"(
            QLabel {
                color: white;
                font-size: 14px;
                font-weight: 500;
                background-color: transparent;
            }
        )");
        
        QLabel *artistLabel = new QLabel(track.artist);
        artistLabel->setStyleSheet(R"(
            QLabel {
                color: rgba(255, 255, 255, 0.6);
                font-size: 12px;
                background-color: transparent;
            }
        )");
        
        textLayout->addWidget(titleLabel);
        textLayout->addWidget(artistLabel);
        
        QLabel *durationLabel = new QLabel(duration);
        durationLabel->setStyleSheet(R"(
            QLabel {
                color: rgba(255, 255, 255, 0.5);
                font-size: 12px;
                min-width: 40px;
            }
        )");
        
        QPushButton *playBtn = new QPushButton("▶");
        playBtn->setFixedSize(35, 35);
        playBtn->setStyleSheet(R"(
            QPushButton {
                background-color: rgba(138, 43, 226, 0.2);
                color: #8A2BE2;
                border: none;
                border-radius: 17px;
                font-size: 12px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: rgba(138, 43, 226, 0.3);
                color: white;
            }
        )");
        
        QPushButton *deleteBtn = new QPushButton("×");
        deleteBtn->setFixedSize(25, 25);
        deleteBtn->setStyleSheet(R"(
            QPushButton {
                background-color: rgba(255, 68, 68, 0.1);
                color: rgba(255, 68, 68, 0.7);
                border: 1px solid rgba(255, 68, 68, 0.3);
                border-radius: 4px;
                font-size: 12px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: rgba(255, 68, 68, 0.2);
                color: #FF4444;
            }
        )");
        
        // ЗАХВАТЫВАЕМ trackId КАК ЛОКАЛЬНУЮ КОПИЮ
        int trackId = track.id;  // Создаем локальную копию
        
        connect(playBtn, &QPushButton::clicked, [this, trackId]() {
            playTrack(trackId);
        });
        
        connect(deleteBtn, &QPushButton::clicked, this, [this, trackId]() {
            onTrackDeleteRequested(trackId);
        });
        
        itemLayout->addWidget(trackIcon);
        itemLayout->addWidget(textWidget, 1);
        itemLayout->addWidget(durationLabel);
        itemLayout->addWidget(playBtn);
        itemLayout->addWidget(deleteBtn);
        
        itemWidget->setLayout(itemLayout);
        itemWidget->setStyleSheet("QWidget { background-color: transparent; }");
        
        userTracksList->setItemWidget(item, itemWidget);
    }
}


void MainWindow::setupAvatar()
{
  setDefaultAvatar();
}

void MainWindow::setAvatarPixmap(const QPixmap& pixmap)
{
    // Убедись, что аватарка круглая через стили, а не маску
    QPixmap scaledPixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    
    // Создаем круглую иконку
    QPixmap circularPixmap(100, 100);
    circularPixmap.fill(Qt::transparent);
    
    QPainter painter(&circularPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    
    QPainterPath path;
    path.addEllipse(0, 0, 100, 100);
    painter.setClipPath(path);
    
    // Центрируем
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
    
    // Круглый градиентный фон
    QPainterPath circlePath;
    circlePath.addEllipse(0, 0, 100, 100);
    painter.setClipPath(circlePath);
    
    QRadialGradient gradient(50, 50, 50);
    gradient.setColorAt(0, QColor("#9B4BFF"));
    gradient.setColorAt(1, QColor("#7B1FA2"));
    
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 100, 100);
    
    // Инициалы
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
    QDir dir("avatars");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QString filePath = QString("avatars/user_%1.png").arg(currentUserId);
    
    // Сохраняем с лучшим качеством
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
    
    // Если файла нет, устанавливаем дефолтный аватар
    setDefaultAvatar();
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
            "Обложка: %4\n\n"
            "Для добавления в библиотеку нажмите правой кнопкой мыши"
        ).arg(title).arg(artist).arg(listeners).arg(coverUrl.isEmpty() ? "Нет" : "Есть");
        
        item->setData(Qt::UserRole, trackInfoText);
        item->setData(Qt::UserRole + 1, track);
    }
    
    trackInfo->setPlainText(QString("Найдено треков: %1\n\nВыберите трек для подробной информации.").arg(tracks.size()));
}

void MainWindow::onNetworkError(const QString& error)
{
    enableSearchControls(true);
    
    QMessageBox::critical(this, "Ошибка поиска", 
        QString("Произошла ошибка при поиске музыки:\n%1").arg(error));
}

void MainWindow::enableSearchControls(bool enable)
{
    searchButton->setEnabled(enable);
    topTracksButton->setEnabled(enable);
    searchInput->setEnabled(enable);
    
    QString style = enable ? 
        "QPushButton { background: #8A2BE2; }" :
        "QPushButton { background: rgba(138, 43, 226, 0.3); color: rgba(255, 255, 255, 0.5); }";
    
    searchButton->setStyleSheet(style);
    topTracksButton->setStyleSheet(style);
}