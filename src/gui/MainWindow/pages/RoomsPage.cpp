#include "gui/MainWindow/pages/RoomsPage.h"
#include "network/RoomManager.h"
#include "network/WebSocketClient.h"
#include "gui/widgets/RoomCard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QDialog>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

// Темный стилизованный диалог
class DarkInputDialog : public QDialog
{
public:
    enum Type { Text, Integer };

    DarkInputDialog(const QString &title, const QString &label, Type type = Text, QWidget *parent = nullptr)
        : QDialog(parent), m_type(type), m_result(false)
    {
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setStyleSheet("background: transparent;");
        setFixedSize(450, 220);
        setWindowTitle(title);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        QWidget* container = new QWidget();
        container->setStyleSheet(R"(
            QWidget {
                background: #0F0F14;
                border: 1px solid rgba(138, 43, 226, 0.3);
                border-radius: 12px;
            }
        )");

        QVBoxLayout* containerLayout = new QVBoxLayout(container);
        containerLayout->setContentsMargins(0, 0, 0, 0);
        containerLayout->setSpacing(0);

        // Header
        QWidget* header = new QWidget();
        header->setFixedHeight(50);
        header->setStyleSheet(R"(
            QWidget {
                background: rgba(138, 43, 226, 0.1);
                border-bottom: 1px solid rgba(138, 43, 226, 0.2);
                border-radius: 12px 12px 0px 0px;
            }
        )");

        QHBoxLayout* headerLayout = new QHBoxLayout(header);
        headerLayout->setContentsMargins(15, 0, 15, 0);

        QLabel* titleLabel = new QLabel(title);
        titleLabel->setStyleSheet(R"(
            QLabel {
                color: #8A2BE2;
                font-size: 14px;
                font-weight: 600;
                background: transparent;
                border: none;
            }
        )");

        headerLayout->addWidget(titleLabel);
        headerLayout->addStretch();

        // Body
        QWidget* body = new QWidget();
        body->setStyleSheet("background: transparent;");
        QVBoxLayout* bodyLayout = new QVBoxLayout(body);
        bodyLayout->setContentsMargins(20, 15, 20, 15);
        bodyLayout->setSpacing(12);

        QLabel* labelText = new QLabel(label);
        labelText->setStyleSheet(R"(
            QLabel {
                color: rgba(255, 255, 255, 0.85);
                font-size: 13px;
                font-weight: 400;
                background: transparent;
                border: none;
            }
        )");

        m_input = new QLineEdit();
        m_input->setMinimumHeight(40);
        m_input->setStyleSheet(R"(
            QLineEdit {
                background: rgba(255, 255, 255, 0.05);
                border: 2px solid rgba(138, 43, 226, 0.2);
                border-radius: 8px;
                padding: 0 12px;
                color: white;
                font-size: 13px;
            }
            QLineEdit:focus {
                border: 2px solid #8A2BE2;
                background: rgba(138, 43, 226, 0.08);
            }
        )");

        bodyLayout->addWidget(labelText);
        bodyLayout->addWidget(m_input);

        // Footer
        QWidget* footer = new QWidget();
        footer->setFixedHeight(50);
        footer->setStyleSheet("background: transparent;");
        QHBoxLayout* footerLayout = new QHBoxLayout(footer);
        footerLayout->setContentsMargins(15, 0, 15, 0);
        footerLayout->setSpacing(10);

        QPushButton* okBtn = new QPushButton("OK");
        okBtn->setFixedWidth(100);
        okBtn->setFixedHeight(35);
        okBtn->setStyleSheet(R"(
            QPushButton {
                background: #8A2BE2;
                color: white;
                border: none;
                border-radius: 6px;
                font-size: 13px;
                font-weight: 600;
            }
            QPushButton:hover {
                background: #9B4BFF;
            }
            QPushButton:pressed {
                background: #7B1FA2;
            }
        )");

        QPushButton* cancelBtn = new QPushButton("Отмена");
        cancelBtn->setFixedWidth(100);
        cancelBtn->setFixedHeight(35);
        cancelBtn->setStyleSheet(R"(
            QPushButton {
                background: rgba(138, 43, 226, 0.2);
                color: #8A2BE2;
                border: 1px solid #8A2BE2;
                border-radius: 6px;
                font-size: 13px;
                font-weight: 600;
            }
            QPushButton:hover {
                background: rgba(138, 43, 226, 0.3);
            }
        )");

        footerLayout->addStretch();
        footerLayout->addWidget(okBtn);
        footerLayout->addWidget(cancelBtn);

        containerLayout->addWidget(header);
        containerLayout->addWidget(body);
        containerLayout->addWidget(footer);

        mainLayout->addWidget(container);

        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(15);
        shadow->setColor(QColor(0, 0, 0, 60));
        shadow->setOffset(0, 4);
        container->setGraphicsEffect(shadow);

        connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
        connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
        connect(m_input, &QLineEdit::returnPressed, this, &QDialog::accept);
    }

    QString getText() const { return m_input->text(); }
    int getInt() const { return m_input->text().toInt(); }

private:
    Type m_type;
    bool m_result;
    QLineEdit* m_input;
};

RoomsPage::RoomsPage(int userId, QWidget *parent)
    : QWidget(parent)
    , m_userId(userId)
    , m_currentRoomId(-1)
    , m_roomManager(new RoomManager(userId, this))
    , m_wsClient(new WebSocketClient(userId, this))
{
    setupUI();
    
    connect(m_roomManager, &RoomManager::roomJoined, [this](int roomId) {
        m_currentRoomId = roomId;
        m_wsClient->joinRoom(roomId);
    });
    
    connect(m_roomManager, &RoomManager::roomLeft, [this](int roomId) {
        m_wsClient->leaveRoom(roomId);
        m_currentRoomId = -1;
    });
    
// m_wsClient->connectToServer("ws://localhost:8080");    
    loadRooms();
}

RoomsPage::~RoomsPage()
{
    if (m_currentRoomId != -1) {
        m_roomManager->leaveRoom(m_currentRoomId);
    }
}

void RoomsPage::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(25);
    
    QLabel* titleLabel = new QLabel("Комнаты");
    titleLabel->setStyleSheet("color: white; font-size: 32px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);
    
    QWidget* controlsWidget = new QWidget();
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setSpacing(15);
    
    m_searchInput = new QLineEdit();
    m_searchInput->setPlaceholderText("Поиск комнат...");
    m_searchInput->setMinimumHeight(45);
    m_searchInput->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 0 20px;
            color: white;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 2px solid #8A2BE2;
        }
    )");
    
    m_createButton = new QPushButton("Создать комнату");
    m_createButton->setMinimumHeight(45);
    m_createButton->setFixedWidth(180);
    m_createButton->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
    )");
    
    controlsLayout->addWidget(m_searchInput);
    controlsLayout->addWidget(m_createButton);
    
    mainLayout->addWidget(controlsWidget);
    
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setStyleSheet(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: rgba(255, 255, 255, 0.05);
            width: 8px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background: rgba(138, 43, 226, 0.5);
            border-radius: 4px;
        }
    )");
    
    m_roomsContainer = new QWidget();
    m_roomsContainer->setStyleSheet("background: transparent;");
    m_scrollArea->setWidget(m_roomsContainer);
    
    mainLayout->addWidget(m_scrollArea, 1);
    
    connect(m_createButton, &QPushButton::clicked, this, &RoomsPage::onCreateRoom);
    connect(m_searchInput, &QLineEdit::textChanged, this, &RoomsPage::onSearchRooms);
}

void RoomsPage::loadRooms()
{
    QVector<Room> rooms = m_roomManager->getPublicRooms();
    displayRooms(rooms);
}

void RoomsPage::displayRooms(const QVector<Room>& rooms)
{
    if (m_roomsContainer->layout()) {
        QLayoutItem* item;
        while ((item = m_roomsContainer->layout()->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete m_roomsContainer->layout();
    }
    
    QVBoxLayout* mainLayout = new QVBoxLayout(m_roomsContainer);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    if (rooms.isEmpty()) {
        mainLayout->addStretch();
        
        QWidget* centerWidget = new QWidget();
        QVBoxLayout* centerLayout = new QVBoxLayout(centerWidget);
        centerLayout->setContentsMargins(0, 0, 0, 0);
        centerLayout->setSpacing(15);
        
        QLabel *emptyLabel = new QLabel("Нет доступных комнат");
        emptyLabel->setStyleSheet("color: rgba(255, 255, 255, 0.6); font-size: 18px; font-weight: 500;");
        emptyLabel->setAlignment(Qt::AlignCenter);
        
        QLabel *hintLabel = new QLabel("Создайте новую комнату или попросите приглашение");
        hintLabel->setStyleSheet("color: rgba(255, 255, 255, 0.4); font-size: 14px;");
        hintLabel->setAlignment(Qt::AlignCenter);
        
        centerLayout->addWidget(emptyLabel);
        centerLayout->addWidget(hintLabel);
        centerWidget->setLayout(centerLayout);
        
        mainLayout->addWidget(centerWidget, 0, Qt::AlignCenter);
        mainLayout->addStretch();
    } else {
        QGridLayout* gridLayout = new QGridLayout();
        gridLayout->setSpacing(20);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        
        for (int i = 0; i < rooms.size(); ++i) {
            RoomCard* card = new RoomCard(rooms[i]);
            connect(card, &RoomCard::joinClicked, this, &RoomsPage::onJoinRoom);
            
            int row = i / 2;
            int col = i % 2;
            gridLayout->addWidget(card, row, col);
        }
        
        gridLayout->setRowStretch(gridLayout->rowCount(), 1);
        gridLayout->setColumnStretch(0, 1);
        gridLayout->setColumnStretch(1, 1);
        
        mainLayout->addLayout(gridLayout, 1);
    }
}

void RoomsPage::onCreateRoom()
{
    openRoomDialog();
}

void RoomsPage::openRoomDialog()
{
    // Название комнаты
    DarkInputDialog nameDialog("Создание комнаты", "Название комнаты:", DarkInputDialog::Text, this);
    if (nameDialog.exec() != QDialog::Accepted) return;
    QString name = nameDialog.getText();
    if (name.isEmpty()) return;

    // Жанр
    DarkInputDialog genreDialog("Создание комнаты", "Жанр:", DarkInputDialog::Text, this);
    if (genreDialog.exec() != QDialog::Accepted) return;
    QString genre = genreDialog.getText();

    // Максимум участников
    DarkInputDialog membersDialog("Создание комнаты", "Максимум участников (2-100):", DarkInputDialog::Integer, this);
    if (membersDialog.exec() != QDialog::Accepted) return;
    int maxMembers = membersDialog.getInt();
    if (maxMembers < 2 || maxMembers > 100) {
        QMessageBox::warning(this, "Ошибка", "Количество участников должно быть от 2 до 100");
        return;
    }

    int roomId = m_roomManager->createRoom(name, "", genre, maxMembers, false);

    if (roomId != -1) {
        QMessageBox::information(this, "Успех", "Комната создана!");
        loadRooms();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось создать комнату");
    }
}

void RoomsPage::onJoinRoom(int roomId)
{
    if (m_roomManager->joinRoom(roomId)) {
        QMessageBox::information(this, "Успех", "Вы присоединились к комнате!");
        loadRooms();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось присоединиться");
    }
}

void RoomsPage::onLeaveRoom()
{
    if (m_currentRoomId != -1) {
        m_roomManager->leaveRoom(m_currentRoomId);
    }
}

void RoomsPage::onSearchRooms(const QString& query)
{
    QVector<Room> allRooms = m_roomManager->getPublicRooms();
    
    if (query.isEmpty()) {
        displayRooms(allRooms);
        return;
    }
    
    QVector<Room> filtered;
    for (const Room& room : allRooms) {
        if (room.name.contains(query, Qt::CaseInsensitive) ||
            room.genre.contains(query, Qt::CaseInsensitive)) {
            filtered.append(room);
        }
    }
    
    displayRooms(filtered);
}