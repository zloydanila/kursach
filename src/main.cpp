#include <QApplication>
#include <QDebug>

#include "gui/AuthWindow/AuthWindow.h"
#include "database/DatabaseManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Инициализация базы данных...";
    if (!DatabaseManager::instance().initializeDatabase()) {
        qDebug() << "Ошибка создания БД";
        return -1;
    }
    
    qDebug() << "БД успешно создана";
    
    if (!DatabaseManager::instance().isDatabaseOpen()) {
        qDebug() << "Нет подключения к БД";
        return -1;
    }
    
    qDebug() << "Подключение к БД активно";
    
    AuthWindow authWindow;
    authWindow.show();
    
    return app.exec();
}