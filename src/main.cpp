#include <QApplication>
#include <QDebug>

#include "gui/AuthWindow/AuthWindow.h"
#include "database/DatabaseManager.h"
#include "api/MusicAPIManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "🎵 SoundSpace - Launching Application";
    qDebug() << "======================================";
    
    // Инициализация БД
    qDebug() << "🔧 Initializing database...";
    if (!DatabaseManager::instance().initializeDatabase()) {
        qDebug() << "❌ Failed to initialize database!";
        return -1;
    }
    qDebug() << "✅ Database initialized successfully!";
    
    // Создание менеджера API
    qDebug() << "🔧 Initializing Last.fm API...";
    MusicAPIManager *apiManager = new MusicAPIManager();
    qDebug() << "✅ Last.fm API initialized!";
    
    // Создание и отображение окна авторизации
    AuthWindow authWindow;
    authWindow.show();
    
    qDebug() << "🚀 Application started successfully!";

    return app.exec();
}