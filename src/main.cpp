#include <QApplication>
#include <QDebug>
#include <QTimer>

#include "gui/AuthWindow/AuthWindow.h"
#include "database/DatabaseManager.h"
#include "api/MusicAPIManager.h"
#include "api/AudioPlayer/AudioPlayer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qDebug() << "создание базы данных" ;
    if (!DatabaseManager::instance().initializeDatabase()) {
        qDebug() << "ошибка создания БД";
        return -1;
    }
    qDebug() << "БД успешно создана";
    qDebug() << "подключение к API";
    MusicAPIManager *apiManager = new MusicAPIManager();
    qDebug() << "API подключено";
    
    AuthWindow authWindow;
    authWindow.show();
    
    return app.exec();
}