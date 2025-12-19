#ifndef SEARCHMUSICPAGE_H
#define SEARCHMUSICPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QVariant>

class MusicAPIManager;
class AudioPlayer;

class SearchMusicPage : public QWidget
{
    Q_OBJECT

public:
    explicit SearchMusicPage(int userId, AudioPlayer *audioPlayer, QWidget *parent = nullptr);
    ~SearchMusicPage();

signals:
    void stationAdded();

private slots:
    void onSearchClicked();
    void onTopStationsClicked();
    void onGenreChanged(const QString &genre);
    void onStationsFound(const QVariantList &stations);
    void onStationSelected(QListWidgetItem *item);
    void onStationContextMenu(const QPoint &pos);
    void onNetworkError(const QString &error);
    void playSelectedStation();
    void addSelectedStation();

private:
    void setupUI();
    void displayStations(const QVariantList &stations);

    int muserId;
    MusicAPIManager *mapiManager;
    AudioPlayer *m_audioPlayer;

    QLineEdit *msearchInput;
    QPushButton *msearchButton;
    QPushButton *mtopStationsButton;
    QComboBox *mgenreCombo;
    QListWidget *mstationsList;
    QTextEdit *mstationInfo;
    QLabel *mloadingLabel;

    QVariantList mcurrentStations;
    QVariantMap mcurrentSelectedStation;
    int mcurrentSelectedIndex;
};

#endif
