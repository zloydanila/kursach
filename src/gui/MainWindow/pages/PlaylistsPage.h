#ifndef PLAYLISTPAGE_H
#define PLAYLISTPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include "core/models/Playlist.h"
#include "core/models/Track.h"

class PlaylistManager;

class PlaylistPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistPage(int userId, QWidget *parent = nullptr);
    ~PlaylistPage();
    
    void loadPlaylists();

signals:
    void playTrackRequested(const TrackData& track);

private slots:
    void onCreatePlaylist();
    void onPlaylistSelected(QListWidgetItem* item);
    void onTrackDoubleClicked(QListWidgetItem* item);
    void onBackToPlaylists();

private:
    void setupUI();
    void displayPlaylists(const QVector<Playlist>& playlists);
    void displayPlaylistTracks(int playlistId);
    void showPlaylistsView();
    void showTracksView();
    
    int m_userId;
    int m_currentPlaylistId;
    PlaylistManager* m_playlistManager;
    
    QWidget* m_playlistsWidget;
    QWidget* m_tracksWidget;
    QListWidget* m_playlistsList;
    QListWidget* m_tracksList;
    QPushButton* m_createButton;
    QPushButton* m_backButton;
    QLabel* m_playlistTitleLabel;
};

#endif
