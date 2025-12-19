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
#include <QIcon>
#include <QSize>
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
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include "database/DatabaseManager.h"
#include "database/api/MusicAPIManager.h"
#include "audio/AudioPlayer.h"
#include "core/models/Track.h"

static QString greetingForNow()
{
    int h = QTime::currentTime().hour();
    if (h >= 5 && h < 12) return "Доброе утро";
    if (h >= 12 && h < 18) return "Добрый день";
    if (h >= 18 && h < 23) return "Добрый вечер";
    return "Доброй ночи";
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget();
    centralWidget->setObjectName("windowContent");
    centralWidget->setStyleSheet(R"(
        #windowContent { background-color: #0F0F14; border-radius: 0; }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *titleBar = new QWidget();
    titleBar->setFixedHeight(40);
    titleBar->setStyleSheet(R"(
        QWidget { background: rgba(15, 15, 20, 0.8); border-bottom: 1px solid rgba(255, 255, 255, 0.05); }
    )");

    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(20, 0, 20, 0);

    QLabel *titleLabel = new QLabel("Chorus");
    titleLabel->setStyleSheet(R"(
        QLabel { color: #8A2BE2; font-size: 16px; font-weight: 900; opacity: 0.9; }
    )");

    QWidget *windowButtons = new QWidget();
    QHBoxLayout *buttonsLayout = new QHBoxLayout(windowButtons);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(5);

    QPushButton *minimizeBtn = new QPushButton("—");
    QPushButton *maximizeBtn = new QPushButton("□");
    QPushButton *closeBtn = new QPushButton("×");

    QString windowBtnStyle = R"(
        QPushButton { background: transparent; color: rgba(255, 255, 255, 0.6); border: none; font-size: 16px; padding: 0; min-width: 28px; min-height: 28px; border-radius: 4px; font-weight: 300; }
        QPushButton:hover { background: rgba(255, 255, 255, 0.1); color: white; }
        QPushButton#closeBtn:hover { background: #FF4444; color: white; }
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
        if (isMaximized()) showNormal();
        else showMaximized();
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
        QWidget { background: #0F0F14; border-right: 1px solid rgba(255, 255, 255, 0.05); }
    )");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    QWidget *logoSection = new QWidget();
    logoSection->setFixedHeight(70);
    logoSection->setStyleSheet(R"(
        QWidget { background: rgba(138, 43, 226, 0.1); border-bottom: 1px solid rgba(255, 255, 255, 0.05); }
    )");

    QHBoxLayout *logoLayout = new QHBoxLayout(logoSection);
    logoLayout->setContentsMargins(20, 0, 20, 0);

    QLabel *logoLabel = new QLabel(greetingForNow() + ", " + currentUsername + "!");
    logoLabel->setStyleSheet(R"(
        QLabel { color: #8A2BE2; font-size: 16px; font-weight: 800; }
    )");
    logoLabel->setWordWrap(true);

    logoLayout->addWidget(logoLabel);
    sidebarLayout->addWidget(logoSection);

    QWidget *profileSection = new QWidget();
    profileSection->setStyleSheet(R"(
        QWidget { background: transparent; padding: 20px; border-bottom: 1px solid rgba(255, 255, 255, 0.05); }
    )");

    QVBoxLayout *profileLayout = new QVBoxLayout(profileSection);
    profileLayout->setContentsMargins(0, 0, 0, 0);
    profileLayout->setSpacing(15);
    profileLayout->setAlignment(Qt::AlignTop);

    avatarButton = new QPushButton();
    avatarButton->setFixedSize(100, 100);
    avatarButton->setToolTip("Нажми, чтобы изменить аватар");
    avatarButton->setStyleSheet(R"(
        QPushButton { border: 3px solid #8A2BE2; border-radius: 50px; padding: 0; background: transparent; }
        QPushButton:hover { border-color: #9B4BFF; background: rgba(138, 43, 226, 0.1); }
    )");

    QGraphicsDropShadowEffect *avatarEffect = new QGraphicsDropShadowEffect();
    avatarEffect->setBlurRadius(20);
    avatarEffect->setColor(QColor(138, 43, 226, 100));
    avatarEffect->setOffset(0, 0);
    avatarButton->setGraphicsEffect(avatarEffect);


usernameLabel = new QLabel(currentUsername);
    usernameLabel->setStyleSheet(R"(
        QLabel { color: #FFFFFF; font-size: 18px; font-weight: 700; text-align: center; padding: 10px 0 5px 0; }
    )");
    usernameLabel->setWordWrap(true);
    usernameLabel->setMaximumWidth(260);

    QToolButton *statusButton = new QToolButton();
    statusButton->setText("Онлайн");
    statusButton->setObjectName("statusButton");
    statusButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    statusButton->setArrowType(Qt::NoArrow);
    statusButton->setCursor(Qt::PointingHandCursor);

    statusButton->setStyleSheet("QToolButton#statusButton{background:rgba(138,43,226,0.18);border:1px solid rgba(138,43,226,0.35);color:rgba(255,255,255,0.92);padding:8px 14px;border-radius:12px;font-size:13px;font-weight:800;}QToolButton#statusButton:hover{background:rgba(155,75,255,0.28);border:1px solid rgba(155,75,255,0.55);color:#fff;}QToolButton#statusButton:pressed{background:rgba(155,75,255,0.38);}QToolButton::menu-indicator{width:0px;}");
profileLayout->addWidget(avatarButton, 0, Qt::AlignHCenter);
    profileLayout->addWidget(usernameLabel, 0, Qt::AlignHCenter);
    profileLayout->addWidget(statusButton, 0, Qt::AlignHCenter);
    sidebarLayout->addWidget(profileSection);

    QWidget *navSection = new QWidget();
    navSection->setStyleSheet("background: transparent;");

    QVBoxLayout *navLayout = new QVBoxLayout(navSection);
    navLayout->setContentsMargins(15, 20, 15, 20);
    navLayout->setSpacing(3);

    QString navButtonStyle = R"(
        QPushButton { background: transparent; color: rgba(255, 255, 255, 0.8); border: none; text-align: left; padding: 12px 20px; font-size: 14px; font-weight: 600; border-radius: 8px; min-height: 45px; }
        QPushButton:hover { color: #FFFFFF; background: rgba(138, 43, 226, 0.15); padding-left: 25px; }
        QPushButton:pressed { background: rgba(138, 43, 226, 0.25); }
    )";

    myMusicBtn = new QPushButton("Мои радиостанции");
    musicSearchBtn = new QPushButton("Поиск радио");
    friendsBtn = new QPushButton("Друзья");
    messagesBtn = new QPushButton("Сообщения");

    QList<QPushButton*> navButtons = {
        myMusicBtn, musicSearchBtn, friendsBtn, messagesBtn
    };

    for (auto btn : navButtons) {
        btn->setStyleSheet(navButtonStyle);
        btn->setCursor(Qt::PointingHandCursor);
        navLayout->addWidget(btn);
    }

    navLayout->addStretch();
    sidebarLayout->addWidget(navSection, 1);

    QWidget *bottomSection = new QWidget();
    bottomSection->setStyleSheet(R"(
        QWidget { background: rgba(0, 0, 0, 0.2); padding: 20px; border-top: 1px solid rgba(255, 255, 255, 0.05); }
    )");

    QPushButton *logoutBtn = new QPushButton("Выход");
    logoutBtn->setObjectName("logoutBtn");
    logoutBtn->setStyleSheet(R"(
        QPushButton { background: rgba(255, 68, 68, 0.1); border: 1px solid rgba(255, 68, 68, 0.3); color: rgba(255, 255, 255, 0.9); border-radius: 8px; padding: 12px; font-size: 14px; font-weight: 600; width: 100%; min-height: 45px; }
        QPushButton:hover { background: rgba(255, 68, 68, 0.2); color: #FFFFFF; }
    )");
    connect(logoutBtn, &QPushButton::clicked, this, &QMainWindow::close);

    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomSection);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->addWidget(logoutBtn);

    sidebarLayout->addWidget(bottomSection);

    QMenu *statusMenu = new QMenu(this);
    statusMenu->setStyleSheet(R"(
        QMenu { background-color: #1A1A21; border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; padding: 5px; }
        QMenu::item { background-color: transparent; color: rgba(255, 255, 255, 0.8); padding: 8px 25px; border-radius: 4px; margin: 2px; font-size: 14px; min-height: 30px; }
        QMenu::item:selected { background-color: rgba(138, 43, 226, 0.3); color: #8A2BE2; }
    )");

    QAction *onlineAction = new QAction("Онлайн", this);
    QAction *awayAction = new QAction("Отсутствую", this);
    QAction *dndAction = new QAction("Не беспокоить", this);
    QAction *invisibleAction = new QAction("Невидимый", this);

    statusMenu->addAction(onlineAction);
    statusMenu->addAction(awayAction);
    statusMenu->addAction(dndAction);
    statusMenu->addAction(invisibleAction);

    statusButton->setMenu(statusMenu);
    statusButton->setPopupMode(QToolButton::InstantPopup);

    connect(onlineAction, &QAction::triggered, statusButton, [statusButton]() {
        statusButton->setText("Онлайн");
    });

    connect(awayAction, &QAction::triggered, statusButton, [statusButton]() {
        statusButton->setText("Отсутствую");
    });

    connect(dndAction, &QAction::triggered, statusButton, [statusButton]() {
        statusButton->setText("Не беспокоить");
    });

    connect(invisibleAction, &QAction::triggered, statusButton, [statusButton]() {
        statusButton->setText("Невидимый");
    });
}

void MainWindow::setupAvatar()
{    loadUserAvatar();
}

void MainWindow::setAvatarPixmap(const QPixmap& pixmap)
{
    if (!avatarButton) return;

    const int size = 100;
    QPixmap src = pixmap.isNull() ? QPixmap() : pixmap.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    QPixmap out(size, size);
    out.fill(Qt::transparent);

    {
        QPainter p(&out);
        p.setRenderHint(QPainter::Antialiasing, true);
        QPainterPath path;
        path.addEllipse(0, 0, size, size);
        p.setClipPath(path);
        if (!src.isNull()) {
            const int x = (size - src.width()) / 2;
            const int y = (size - src.height()) / 2;
            p.drawPixmap(x, y, src);
        }
    }

    avatarButton->setIcon(QIcon(out));
    avatarButton->setIconSize(QSize(size, size));
}

void MainWindow::setDefaultAvatar()
{
    if (!avatarButton) return;

    const int size = 100;
    QString letter = currentUsername.trimmed().left(1).toUpper();
    if (letter.isEmpty()) letter = "?";

    QPixmap out(size, size);
    out.fill(Qt::transparent);

    QPainter p(&out);
    p.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    path.addEllipse(0, 0, size, size);
    p.setClipPath(path);

    QLinearGradient g(0, 0, size, size);
    g.setColorAt(0, QColor(138, 43, 226));
    g.setColorAt(1, QColor(155, 75, 255));
    p.fillPath(path, g);

    p.setClipping(false);
    p.setPen(Qt::white);
    QFont f = p.font();
    f.setBold(true);
    f.setPointSize(size / 2.5);
    p.setFont(f);
    p.drawText(out.rect(), Qt::AlignCenter, letter);

    setAvatarPixmap(out);
}

void MainWindow::showAvatarOverlay() {}

void MainWindow::hideAvatarOverlay() {}

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

    saveAvatarToDbAndCache(px);
    setAvatarPixmap(px);
}
