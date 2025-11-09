#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QDir>
#include "gui/AuthWindow/AuthWindow.h"
#include "database/DatabaseManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Устанавливаем информацию о приложении
    app.setApplicationName("SoundSpace");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("SoundSpace");
    
    // Создаем папки для аватаров, если их нет
    QDir().mkpath("avatars");
    
    // Инициализируем базу данных
    if (!DatabaseManager::instance().initializeDatabase()) {
        QMessageBox::critical(nullptr, "Ошибка базы данных", 
                            "Не удалось подключиться к базе данных.\n"
                            "Проверьте подключение к интернету и настройки базы данных.");
        return -1;
    }
    
    // Показываем окно авторизации
    AuthWindow authWindow;
    authWindow.show();
    
    qDebug() << "🚀 Приложение SoundSpace запущено";
    
    return app.exec();
}