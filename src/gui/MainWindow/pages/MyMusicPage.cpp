#include "gui/MainWindow/pages/MyMusicPage.h"
#include "core/managers/TrackManager.h"
#include "audio/LocalFileHandler.h"
#include "audio/AudioPlayer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDebug>
#include <QMenu>
#include <QRandomGenerator>
#include <QMediaPlayer>
#include "database/DatabaseManager.h"

MyMusicPage::MyMusicPage(int userId, QWidget *parent)
    : QWidget(parent), muserId(userId),
      mtrackManager(new TrackManager(userId, this)),
      mfileHandler(new LocalFileHandler(this)),
      maudioPlayer(new AudioPlayer(this)),
      mradioPlayer(new QMediaPlayer(this))
{
    setupUI();
    loadRadioStations();
}

MyMusicPage::~MyMusicPage()
{
}

void MyMusicPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(25);
    
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    mtitleLabel = new QLabel("Мои радиостанции");
    mtitleLabel->setStyleSheet("color: white; font-size: 32px; font-weight: bold;");
    
    mstatsLabel = new QLabel();
    mstatsLabel->setStyleSheet("color: rgba(255, 255, 255, 0.6); font-size: 14px;");
    
    headerLayout->addWidget(mtitleLabel);
    headerLayout->addWidget(mstatsLabel);
    headerLayout->addStretch();
    
    mainLayout->addWidget(headerWidget);
    
    QWidget *controlsWidget = new QWidget();
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setSpacing(10);
    
    mrefreshButton = new QPushButton("Обновить радиостанции");
    mrefreshButton->setMinimumHeight(50);
    mrefreshButton->setMinimumWidth(200);
    mrefreshButton->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 600;
            padding: 10px 20px;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
    )");
    
    mplayRandomButton = new QPushButton("Включить случайную");
    mplayRandomButton->setMinimumHeight(50);
    mplayRandomButton->setMinimumWidth(200);
    mplayRandomButton->setStyleSheet(mrefreshButton->styleSheet());
    
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(mrefreshButton);
    buttonsLayout->addWidget(mplayRandomButton);
    buttonsLayout->addStretch();
    
    controlsLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(controlsWidget);
    
    msearchEdit = new QLineEdit();
    msearchEdit->setPlaceholderText("Поиск по названию...");
    msearchEdit->setFixedHeight(45);
    msearchEdit->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 0 15px;
            color: white;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 2px solid #8A2BE2;
        }
    )");
    
    mainLayout->addWidget(msearchEdit);
    
    mradioList = new QListWidget();
    mradioList->setStyleSheet(R"(
        QListWidget {
            background: rgba(255, 255, 255, 0.03);
            border: 2px solid rgba(255, 255, 255, 0.05);
            border-radius: 15px;
            color: white;
            font-size: 14px;
            outline: none;
        }
        QListWidget::item {
            background: transparent;
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
            padding: 0px;
            margin: 0px;
            border-radius: 0px;
        }
        QListWidget::item:selected {
            background: rgba(138, 43, 226, 0.15);
            border: none;
        }
        QListWidget::item:hover {
            background: rgba(255, 255, 255, 0.05);
        }
    )");
    mradioList->setContextMenuPolicy(Qt::CustomContextMenu);
    
    mainLayout->addWidget(mradioList, 1);
    
    connect(mrefreshButton, &QPushButton::clicked, this, &MyMusicPage::onRefreshClicked);
    connect(mplayRandomButton, &QPushButton::clicked, this, &MyMusicPage::onPlayRadioClicked);
    connect(msearchEdit, &QLineEdit::textChanged, this, &MyMusicPage::onSearchChanged);
    connect(mradioList, &QListWidget::customContextMenuRequested, this, &MyMusicPage::onRadioListContextMenu);
}

void MyMusicPage::loadRadioStations()
{
    mradioList->clear();
    QList<TrackData> list = DatabaseManager::instance().getUserRadioStations(muserId);
    mallRadioStations.clear();
    for (const TrackData &item : list) {
        mallRadioStations.append(item);
    }
    
    qDebug() << "MyMusicPage::loadRadioStations - Loaded stations:" << mallRadioStations.count();
    
    mstatsLabel->setText(QString("Всего: %1").arg(mallRadioStations.count()));
    
    displayRadioStations(mallRadioStations);
}

void MyMusicPage::displayRadioStations(const QVector<TrackData> &stations)
{
    mradioList->clear();
    
    if (stations.isEmpty()) {
        QLabel *emptyLabel = new QLabel("Нет радиостанций.\nНайдите и добавьте станции в разделе 'Поиск музыки'");
        emptyLabel->setStyleSheet("color: rgba(255, 255, 255, 0.5); font-size: 16px;");
        emptyLabel->setAlignment(Qt::AlignCenter | Qt::AlignTop);
        QListWidgetItem *item = new QListWidgetItem();
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(0, 100));
        mradioList->addItem(item);
        mradioList->setItemWidget(item, emptyLabel);
        return;
    }
    
    for (const TrackData &station : stations) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::UserRole, station.id);
        item->setSizeHint(QSize(0, 70));
        
        QWidget *stationWidget = new QWidget();
        stationWidget->setStyleSheet("background-color: transparent;");
        
        QHBoxLayout *layout = new QHBoxLayout(stationWidget);
        layout->setContentsMargins(15, 8, 15, 8);
        layout->setSpacing(12);
        
        QLabel *iconLabel = new QLabel("RADIO");
        iconLabel->setStyleSheet("font-size: 14px; min-width: 45px; color: #8A2BE2; font-weight: bold;");
        
        QWidget *textWidget = new QWidget();
        QVBoxLayout *textLayout = new QVBoxLayout(textWidget);
        textLayout->setContentsMargins(0, 0, 0, 0);
        textLayout->setSpacing(2);
        
        QLabel *titleLabel = new QLabel(station.title);
        titleLabel->setStyleSheet(R"(
            QLabel {
                color: white;
                font-size: 14px;
                font-weight: 500;
                background: transparent;
            }
        )");
        
        QLabel *artistLabel = new QLabel(station.artist);
        artistLabel->setStyleSheet(R"(
            QLabel {
                color: rgba(255, 255, 255, 0.6);
                font-size: 12px;
                background: transparent;
            }
        )");
        
        textLayout->addWidget(titleLabel);
        textLayout->addWidget(artistLabel);
        
        QPushButton *playBtn = new QPushButton();
        playBtn->setFixedSize(45, 40);
        playBtn->setText(QString::fromUtf8("\u25B6"));
        playBtn->setStyleSheet(R"(
            QPushButton {
                background-color: rgba(138, 43, 226, 0.2);
                color: white;
                border: none;
                border-radius: 5px;
                font-size: 18px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: rgba(138, 43, 226, 0.3);
            }
        )");
        
        int stationId = station.id;
        connect(playBtn, &QPushButton::clicked, [this, stationId]() {
            onPlayRadio(stationId);
        });
        
        layout->addWidget(iconLabel);
        layout->addWidget(textWidget, 1);
        layout->addWidget(playBtn);
        
        mradioList->addItem(item);
        mradioList->setItemWidget(item, stationWidget);
    }
}

void MyMusicPage::onRefreshClicked()
{
    loadRadioStations();
}

void MyMusicPage::onPlayRadioClicked()
{
    if (mallRadioStations.isEmpty()) {
        return;
    }
    
    int randomIndex = QRandomGenerator::global()->bounded(mallRadioStations.count());
    onPlayRadio(mallRadioStations[randomIndex].id);
}

void MyMusicPage::onSearchChanged(const QString &text)
{
    if (text.isEmpty()) {
        displayRadioStations(mallRadioStations);
        return;
    }
    
    QVector<TrackData> filtered;
    for (const TrackData &station : mallRadioStations) {
        if (station.title.contains(text, Qt::CaseInsensitive) ||
            station.artist.contains(text, Qt::CaseInsensitive)) {
            filtered.append(station);
        }
    }
    
    displayRadioStations(filtered);
}

void MyMusicPage::onRadioListContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = mradioList->itemAt(pos);
    if (!item) return;
    
    int stationId = item->data(Qt::UserRole).toInt();
    
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
    
    QAction *playAction = menu.addAction("Слушать");
    QAction *deleteAction = menu.addAction("Удалить");
    
    QAction *selected = menu.exec(mradioList->mapToGlobal(pos));
    
    if (selected == playAction) {
        onPlayRadio(stationId);
    }
    else if (selected == deleteAction) {
        onDeleteRadio(stationId);
    }
}

void MyMusicPage::onPlayRadio(int stationId)
{
    for (const TrackData &station : mallRadioStations) {
        if (station.id == stationId) {
            qDebug() << "Playing radio station:" << station.title << station.streamUrl;
            mradioPlayer->setMedia(QUrl(station.streamUrl));
            mradioPlayer->play();
            return;
        }
    }
}

void MyMusicPage::onDeleteRadio(int stationId)
{
    if (DatabaseManager::instance().deleteRadioStation(stationId)) {
        loadRadioStations();
    }
}

void MyMusicPage::loadTracks()
{
}

void MyMusicPage::displayTracks(const QVector<TrackData> &tracks)
{
}

void MyMusicPage::onTrackDoubleClicked(QListWidgetItem *item)
{
}

void MyMusicPage::onTrackContextMenu(const QPoint &pos)
{
}

void MyMusicPage::onDeleteTrack(int trackId)
{
}