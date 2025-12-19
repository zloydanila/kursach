#include "gui/MainWindow/pages/MyMusicPage.h"
#include "audio/AudioPlayer.h"
#include "database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QRandomGenerator>

MyMusicPage::MyMusicPage(int userId, QWidget *parent)
    : QWidget(parent)
    , muserId(userId)
    , mtrackManager(nullptr)
    , mfileHandler(nullptr)    , mradioPlayer(nullptr)
{
    setupUI();
    loadRadioStations();
}

MyMusicPage::~MyMusicPage() {}

void MyMusicPage::reloadStations()
{
    loadRadioStations();
}

void MyMusicPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);

    mtitleLabel = new QLabel("Мои радиостанции");
    mtitleLabel->setStyleSheet("color: white; font-size: 32px; font-weight: 800;");

    mstatsLabel = new QLabel();
    mstatsLabel->setStyleSheet("color: rgba(255, 255, 255, 0.6); font-size: 14px;");

    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->addWidget(mtitleLabel);
    headerLayout->addWidget(mstatsLabel);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    mrefreshButton = new QPushButton("Обновить");
    mrefreshButton->setMinimumHeight(45);
    mrefreshButton->setStyleSheet(R"(
        QPushButton { background: #8A2BE2; color: white; border: none; border-radius: 12px; font-size: 14px; font-weight: 700; padding: 10px 18px; }
        QPushButton:hover { background: #9B4BFF; }
    )");

    mplayRandomButton = new QPushButton("Случайная");
    mplayRandomButton->setMinimumHeight(45);
    mplayRandomButton->setStyleSheet(mrefreshButton->styleSheet());

    buttonsLayout->addWidget(mrefreshButton);
    buttonsLayout->addWidget(mplayRandomButton);
    buttonsLayout->addStretch();
    mainLayout->addLayout(buttonsLayout);

    msearchEdit = new QLineEdit();
    msearchEdit->setPlaceholderText("Поиск по названию/стране/жанру...");
    msearchEdit->setFixedHeight(45);
    msearchEdit->setStyleSheet(R"(
        QLineEdit { background: rgba(255,255,255,0.05); border: 2px solid rgba(255,255,255,0.1); border-radius: 12px; padding: 0 16px; color: white; font-size: 14px; }
        QLineEdit:focus { border: 2px solid #8A2BE2; }
    )");
    mainLayout->addWidget(msearchEdit);

    mradioList = new QListWidget();
    mradioList->setStyleSheet(R"(
        QListWidget { background: rgba(255,255,255,0.03); border: 2px solid rgba(255,255,255,0.05); border-radius: 16px; color: white; font-size: 14px; outline: none; }
        QListWidget::item { background: transparent; border-bottom: 1px solid rgba(255,255,255,0.05); padding: 0px; margin: 0px; }
        QListWidget::item:selected { background: rgba(138,43,226,0.18); border: none; }
        QListWidget::item:hover { background: rgba(255,255,255,0.05); }
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
    QList<TrackData> list = DatabaseManager::instance().getUserRadioStations(muserId);

    mallRadioStations.clear();
    for (const TrackData &s : list) mallRadioStations.append(s);

    mstatsLabel->setText(QString("Всего: %1").arg(mallRadioStations.size()));
    displayRadioStations(mallRadioStations);
}

void MyMusicPage::displayRadioStations(const QVector<TrackData> &stations)
{
    mradioList->clear();

    if (stations.isEmpty()) {
        QLabel *emptyLabel = new QLabel("Нет радиостанций. Добавь в разделе «Поиск радио».");
        emptyLabel->setStyleSheet("color: rgba(255, 255, 255, 0.55); font-size: 16px;");
        emptyLabel->setAlignment(Qt::AlignCenter);
        QListWidgetItem *item = new QListWidgetItem();
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(0, 120));
        mradioList->addItem(item);
        mradioList->setItemWidget(item, emptyLabel);
        return;
    }

    for (const TrackData &station : stations) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::UserRole, station.id);
        item->setSizeHint(QSize(0, 72));

        QWidget *row = new QWidget();
        row->setStyleSheet("background: transparent;");

        QHBoxLayout *layout = new QHBoxLayout(row);
        layout->setContentsMargins(16, 10, 16, 10);
        layout->setSpacing(12);

        QLabel *icon = new QLabel("RADIO");
        icon->setStyleSheet("min-width: 52px; color: #8A2BE2; font-weight: 900; font-size: 13px; background: transparent;");

        QWidget *text = new QWidget();
        text->setStyleSheet("background: transparent;");
        QVBoxLayout *tl = new QVBoxLayout(text);
        tl->setContentsMargins(0, 0, 0, 0);
        tl->setSpacing(2);

        QString titleText = station.title;
        if (station.bitrate > 0) titleText += QString(" - %1kb/s").arg(station.bitrate);

        QLabel *title = new QLabel(titleText);
        title->setStyleSheet("color: white; font-size: 14px; font-weight: 700; background: transparent;");

        QString meta = station.artist;
        if (!station.album.isEmpty()) meta += " • " + station.album;
        QLabel *sub = new QLabel(meta);
        sub->setStyleSheet("color: rgba(255,255,255,0.6); font-size: 12px; background: transparent;");

        tl->addWidget(title);
        tl->addWidget(sub);

        QPushButton *play = new QPushButton(QString::fromUtf8("\u25B6"));
        play->setFixedSize(46, 40);
        play->setStyleSheet(R"(
            QPushButton { background: rgba(138,43,226,0.22); color: white; border: 1px solid rgba(138,43,226,0.35); border-radius: 12px; font-size: 18px; font-weight: 900; }
            QPushButton:hover { background: rgba(138,43,226,0.35); }
        )");

        int stationId = station.id;
        connect(play, &QPushButton::clicked, [this, stationId]() { onPlayRadio(stationId); });

        layout->addWidget(icon);
        layout->addWidget(text, 1);
        layout->addWidget(play);

        mradioList->addItem(item);
        mradioList->setItemWidget(item, row);
    }
}

void MyMusicPage::onRefreshClicked()
{
    loadRadioStations();
}

void MyMusicPage::onPlayRadioClicked()
{
    if (mallRadioStations.isEmpty()) return;
    int idx = QRandomGenerator::global()->bounded(mallRadioStations.size());
    onPlayRadio(mallRadioStations[idx].id);
}

void MyMusicPage::onSearchChanged(const QString &text)
{
    QString q = text.trimmed();
    if (q.isEmpty()) {
        displayRadioStations(mallRadioStations);
        return;
    }

    QVector<TrackData> filtered;
    for (const TrackData &s : mallRadioStations) {
        if (s.title.contains(q, Qt::CaseInsensitive) ||
            s.artist.contains(q, Qt::CaseInsensitive) ||
            s.album.contains(q, Qt::CaseInsensitive)) {
            filtered.append(s);
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
        QMenu { background: #1A1A21; border: 1px solid rgba(255,255,255,0.12); border-radius: 12px; padding: 6px; }
        QMenu::item { background: transparent; color: white; padding: 10px 18px; border-radius: 10px; }
        QMenu::item:selected { background: rgba(138,43,226,0.22); }
    )");

    QAction *actPlay = menu.addAction("Воспроизвести");
    QAction *actDelete = menu.addAction("Удалить");

    QAction *sel = menu.exec(mradioList->viewport()->mapToGlobal(pos));
    if (!sel) return;

    if (sel == actPlay) onPlayRadio(stationId);
    if (sel == actDelete) {
        if (DatabaseManager::instance().deleteRadioStation(stationId)) loadRadioStations();
    }
}

void MyMusicPage::onPlayRadio(int stationId)
{
    for (const TrackData &s : mallRadioStations) {
        if (s.id == stationId) {
            emit playRadioRequested(s.streamUrl, s.title, s.artist);
            DatabaseManager::instance().incrementRadioPlayCount(stationId);
            return;
        }
    }
}

void MyMusicPage::addRadioStation(const TrackData &station)
{
    QString name = station.title.trimmed();
    QString country = station.artist.trimmed();
    QString streamUrl = station.streamUrl.trimmed();
    QString genre = station.album.trimmed();
    int bitrate = station.bitrate > 0 ? station.bitrate : 128;

    if (name.isEmpty() || streamUrl.isEmpty()) return;

    if (DatabaseManager::instance().addRadioStation(muserId, name, country, streamUrl, genre, bitrate)) {
        loadRadioStations();
    }
}
