#include "SearchMusicPage.h"
#include <database/api/MusicAPIManager.h>
#include <database/DatabaseManager.h>
#include <gui/dialogs/NotificationDialog.h>
#include <audio/AudioPlayer.h>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QWidget>
#include <QMenu>
#include <QDebug>

SearchMusicPage::SearchMusicPage(int userId, AudioPlayer *audioPlayer, QWidget *parent)
    : QWidget(parent)
    , muserId(userId)
    , mapiManager(new MusicAPIManager(this))
    , m_audioPlayer(audioPlayer)
    , msearchInput(nullptr)
    , msearchButton(nullptr)
    , mtopStationsButton(nullptr)
    , mgenreCombo(nullptr)
    , mstationsList(nullptr)
    , mstationInfo(nullptr)
    , mloadingLabel(nullptr)
    , mcurrentSelectedIndex(-1)
{
    qDebug() << "SearchMusicPage::SearchMusicPage - audioPlayer:" << audioPlayer;
    
    DatabaseManager::instance().initialize();
    setupUI();

    connect(mapiManager, &MusicAPIManager::stationsFound, this, &SearchMusicPage::onStationsFound);
    connect(mapiManager, &MusicAPIManager::errorOccurred, this, &SearchMusicPage::onNetworkError);
}


SearchMusicPage::~SearchMusicPage() {}

void SearchMusicPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(25);

    QLabel *titleLabel = new QLabel("Поиск музыки");
    titleLabel->setStyleSheet("color: white; font-size: 32px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    QWidget *searchWidget = new QWidget(this);
    QHBoxLayout *searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(15);

    msearchInput = new QLineEdit(searchWidget);
    msearchInput->setPlaceholderText("Поиск станций...");
    msearchInput->setMinimumHeight(50);
    msearchInput->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 12px;
            padding: 0 20px;
            color: white;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 2px solid #8A2BE2;
            background: rgba(255, 255, 255, 0.07);
        }
    )");

    msearchButton = new QPushButton("Поиск", searchWidget);
    msearchButton->setFixedWidth(120);
    msearchButton->setMinimumHeight(50);
    msearchButton->setStyleSheet(R"(
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
        }
    )");

    mtopStationsButton = new QPushButton("Популярные", searchWidget);
    mtopStationsButton->setFixedWidth(150);
    mtopStationsButton->setMinimumHeight(50);
    mtopStationsButton->setStyleSheet(msearchButton->styleSheet());

    searchLayout->addWidget(msearchInput, 1);
    searchLayout->addWidget(msearchButton);
    searchLayout->addWidget(mtopStationsButton);
    mainLayout->addWidget(searchWidget);

    QWidget *genreWidget = new QWidget(this);
    QHBoxLayout *genreLayout = new QHBoxLayout(genreWidget);
    genreLayout->setContentsMargins(0, 0, 0, 0);
    genreLayout->setSpacing(15);

    QLabel *genreLabel = new QLabel("Жанр:");
    genreLabel->setStyleSheet("color: white; font-size: 14px;");

    mgenreCombo = new QComboBox(genreWidget);
    mgenreCombo->setMinimumHeight(40);
    mgenreCombo->setMaximumWidth(250);
    mgenreCombo->addItem("Все");
    mgenreCombo->addItem("Rock");
    mgenreCombo->addItem("Pop");
    mgenreCombo->addItem("Jazz");
    mgenreCombo->addItem("Classical");
    mgenreCombo->addItem("Electronic");
    mgenreCombo->addItem("Hip Hop");
    mgenreCombo->addItem("Country");
    mgenreCombo->setStyleSheet(R"(
        QComboBox {
            background: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 8px;
            padding: 0 10px;
            color: white;
            font-size: 14px;
        }
        QComboBox::drop-down {
            border: none;
        }
        QComboBox QAbstractItemView {
            background: #1A1A21;
            color: white;
            selection-background-color: rgba(138, 43, 226, 0.3);
        }
    )");

    genreLayout->addWidget(genreLabel);
    genreLayout->addWidget(mgenreCombo);
    genreLayout->addStretch();
    mainLayout->addWidget(genreWidget);

    QWidget *resultsWidget = new QWidget(this);
    QHBoxLayout *resultsLayout = new QHBoxLayout(resultsWidget);
    resultsLayout->setContentsMargins(0, 0, 0, 0);
    resultsLayout->setSpacing(20);

    mstationsList = new QListWidget(resultsWidget);
    mstationsList->setStyleSheet(R"(
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
        }
        QListWidget::item:selected {
            background: rgba(138, 43, 226, 0.15);
            border: 1px solid rgba(138, 43, 226, 0.3);
        }
        QListWidget::item:hover {
            background: rgba(255, 255, 255, 0.05);
        }
    )");
    mstationsList->setContextMenuPolicy(Qt::CustomContextMenu);

    QWidget *infoContainer = new QWidget();
    infoContainer->setMaximumWidth(380);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoContainer);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(12);

    mstationInfo = new QTextEdit();
    mstationInfo->setReadOnly(true);
    mstationInfo->setStyleSheet(R"(
        QTextEdit {
            background: rgba(255, 255, 255, 0.03);
            border: 2px solid rgba(255, 255, 255, 0.05);
            border-radius: 15px;
            color: white;
            font-size: 13px;
            padding: 15px;
        }
    )");
    mstationInfo->setPlaceholderText("Информация о станции");

    infoLayout->addWidget(mstationInfo);

    QWidget *buttonsWidget = new QWidget();
    QVBoxLayout *buttonsLayout = new QVBoxLayout(buttonsWidget);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(10);

    QPushButton *playBtn = new QPushButton("Играть");
    playBtn->setMinimumHeight(40);
    playBtn->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
    )");

    QPushButton *addBtn = new QPushButton("Добавить в библиотеку");
    addBtn->setMinimumHeight(40);
    addBtn->setStyleSheet(R"(
        QPushButton {
            background: rgba(255, 255, 255, 0.1);
            color: white;
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.15);
        }
    )");

    connect(playBtn, &QPushButton::clicked, this, &SearchMusicPage::playSelectedStation);
    connect(addBtn, &QPushButton::clicked, this, &SearchMusicPage::addSelectedStation);

    buttonsLayout->addWidget(playBtn);
    buttonsLayout->addWidget(addBtn);
    buttonsLayout->addStretch();

    infoLayout->addWidget(buttonsWidget);

    resultsLayout->addWidget(mstationsList, 1);
    resultsLayout->addWidget(infoContainer);
    mainLayout->addWidget(resultsWidget, 1);

    mloadingLabel = new QLabel("Загрузка...");
    mloadingLabel->setStyleSheet("color: #8A2BE2; font-size: 16px;");
    mloadingLabel->setAlignment(Qt::AlignCenter);
    mloadingLabel->hide();
    mainLayout->addWidget(mloadingLabel);

    connect(msearchButton, &QPushButton::clicked, this, &SearchMusicPage::onSearchClicked);
    connect(mtopStationsButton, &QPushButton::clicked, this, &SearchMusicPage::onTopStationsClicked);
    connect(msearchInput, &QLineEdit::returnPressed, this, &SearchMusicPage::onSearchClicked);
    connect(mgenreCombo, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &SearchMusicPage::onGenreChanged);
    connect(mstationsList, &QListWidget::itemClicked, this, &SearchMusicPage::onStationSelected);
    connect(mstationsList, &QListWidget::customContextMenuRequested, this, &SearchMusicPage::onStationContextMenu);
}

void SearchMusicPage::onSearchClicked()
{
    QString query = msearchInput->text().trimmed();
    if (query.isEmpty())
        return;

    mloadingLabel->show();
    mstationsList->clear();
    mstationInfo->clear();
    mcurrentStations.clear();

    mapiManager->searchStations(query, muserId);
}

void SearchMusicPage::onTopStationsClicked()
{
    mloadingLabel->show();
    mstationsList->clear();
    mstationInfo->clear();
    mcurrentStations.clear();

    mapiManager->getTopStations(muserId);
}

void SearchMusicPage::onGenreChanged(const QString &genre)
{
    if (genre.isEmpty() || genre == "Все")
        return;

    mloadingLabel->show();
    mstationsList->clear();
    mstationInfo->clear();
    mcurrentStations.clear();

    qDebug() << "SearchMusicPage::onGenreChanged: Поиск по жанру:" << genre;
    mapiManager->getStationsByGenre(genre, muserId);
}

void SearchMusicPage::onStationsFound(const QVariantList &stations)
{
    mloadingLabel->hide();
    mcurrentStations = stations;
    displayStations(stations);
}

void SearchMusicPage::displayStations(const QVariantList &stations)
{
    mstationsList->clear();
    if (stations.isEmpty()) {
        QLabel *emptyLabel = new QLabel("Радиостанции не найдены");
        emptyLabel->setStyleSheet("color: rgba(255, 255, 255, 0.5); font-size: 16px;");
        emptyLabel->setAlignment(Qt::AlignCenter);
        QListWidgetItem *item = new QListWidgetItem();
        item->setFlags(Qt::NoItemFlags);
        mstationsList->addItem(item);
        mstationsList->setItemWidget(item, emptyLabel);
        return;
    }

    for (int i = 0; i < stations.size(); i++) {
        QVariantMap station = stations[i].toMap();
        QString text = station.value("title").toString();
        if (!station.value("artist").toString().isEmpty()) {
            text = QString("%1 - %2").arg(text, station.value("artist").toString());
        }

        QListWidgetItem *item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, i);
        mstationsList->addItem(item);
    }
}

void SearchMusicPage::onStationSelected(QListWidgetItem *item)
{
    int index = item->data(Qt::UserRole).toInt();
    if (index < 0 || index >= mcurrentStations.size())
        return;

    mcurrentSelectedIndex = index;
    mcurrentSelectedStation = mcurrentStations[index].toMap();

    QString info = QString("<h3 style=\"color:#8A2BE2;margin:0 0 10px 0;\">%1</h3>")
                   .arg(mcurrentSelectedStation.value("title").toString());

    if (!mcurrentSelectedStation.value("artist").toString().isEmpty()) {
        info += QString("<p style=\"color:white;font-size:14px;margin:0 0 8px 0;\"><b>%1</b></p>")
                .arg(mcurrentSelectedStation.value("artist").toString());
    }

    if (mcurrentSelectedStation.contains("genre") && !mcurrentSelectedStation.value("genre").toString().isEmpty()) {
        info += QString("<p style=\"color:rgba(255,255,255,0.7);font-size:12px;margin:0 0 4px 0;\">Жанр: %1</p>")
                .arg(mcurrentSelectedStation.value("genre").toString());
    }

    if (mcurrentSelectedStation.contains("bitrate") && mcurrentSelectedStation.value("bitrate").toInt() > 0) {
        info += QString("<p style=\"color:rgba(255,255,255,0.7);font-size:12px;margin:0;\">Битрейт: %1 kbps</p>")
                .arg(mcurrentSelectedStation.value("bitrate").toString());
    }

    mstationInfo->setHtml(info);
}

void SearchMusicPage::onStationContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = mstationsList->itemAt(pos);
    if (!item)
        return;

    int index = item->data(Qt::UserRole).toInt();
    if (index < 0 || index >= mcurrentStations.size())
        return;

    QVariantMap station = mcurrentStations[index].toMap();

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
    QAction *addAction = menu.addAction("Добавить в библиотеку");
    QAction *selected = menu.exec(mstationsList->mapToGlobal(pos));

    if (selected == playAction) {
        if (m_audioPlayer) {
            m_audioPlayer->playRadio(
                station.value("streamUrl").toString(),
                station.value("title").toString(),
                station.value("artist").toString()
            );
        }
    }
    else if (selected == addAction) {
        qDebug() << "onStationContextMenu: Попытка добавления радиостанции"
                 << "userId:" << muserId
                 << "title:" << station.value("title").toString()
                 << "streamUrl:" << station.value("streamUrl").toString();

        bool success = DatabaseManager::instance().addRadioStation(
            muserId,
            station.value("title").toString(),
            station.value("artist").toString(),
            station.value("streamUrl").toString(),
            station.value("genre").toString(),
            station.value("bitrate").toInt()
        );

        qDebug() << "onStationContextMenu: Результат добавления - success:" << success;

        if (success) {
            NotificationDialog dialog(
                QString("Радиостанция '%1' добавлена в библиотеку").arg(station.value("title").toString()),
                NotificationDialog::Type::Success,
                this
            );
            dialog.exec();
            emit stationAdded();
        } else {
            NotificationDialog dialog(
                "Не удалось добавить радиостанцию. Проверьте подключение и попробуйте снова.",
                NotificationDialog::Type::Error,
                this
            );
            dialog.exec();
        }
    }
}

void SearchMusicPage::playSelectedStation()
{
    if (mcurrentSelectedIndex >= 0 && mcurrentSelectedIndex < mcurrentStations.size()) {
        QVariantMap station = mcurrentStations[mcurrentSelectedIndex].toMap();
        if (m_audioPlayer) {
            m_audioPlayer->playRadio(
                station.value("streamUrl").toString(),
                station.value("title").toString(),
                station.value("artist").toString()
            );
        }
    }
}

void SearchMusicPage::addSelectedStation()
{
    if (mcurrentSelectedIndex >= 0 && mcurrentSelectedIndex < mcurrentStations.size()) {
        QVariantMap station = mcurrentStations[mcurrentSelectedIndex].toMap();

        bool success = DatabaseManager::instance().addRadioStation(
            muserId,
            station.value("title").toString(),
            station.value("artist").toString(),
            station.value("streamUrl").toString(),
            station.value("genre").toString(),
            station.value("bitrate").toInt()
        );

        if (success) {
            NotificationDialog dialog(
                QString("Радиостанция '%1' добавлена в библиотеку").arg(station.value("title").toString()),
                NotificationDialog::Type::Success,
                this
            );
            dialog.exec();
            emit stationAdded();
        } else {
            NotificationDialog dialog(
                "Не удалось добавить радиостанцию.",
                NotificationDialog::Type::Error,
                this
            );
            dialog.exec();
        }
    }
}

void SearchMusicPage::onNetworkError(const QString &error)
{
    mloadingLabel->hide();
    qDebug() << "SearchMusicPage::onNetworkError:" << error;
    NotificationDialog dialog(
        error,
        NotificationDialog::Type::Error,
        this
    );
    dialog.exec();
}
