#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QDir>

#include "gui/AuthWindow/AuthWindow.h"
#include "database/DatabaseManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "==========================================";
    qDebug() << "Запуск SoundSpace...";
    qDebug() << "==========================================";
    
    // Устанавливаем название приложения
    QApplication::setApplicationName("SoundSpace");
    QApplication::setOrganizationName("SoundSpace");
    
    // Загружаем глобальные стили
    QFile styleFile(":/styles/styles.css");
    if (styleFile.open(QIODevice::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        qDebug() << "✅ Стили загружены";
    } else {
        qDebug() << "❌ Не удалось загрузить стили";
        qDebug() << "Путь:" << styleFile.fileName();
        qDebug() << "Рабочая директория:" << QDir::currentPath();
    }
    
    qDebug() << "Инициализация базы данных...";
    
    // Инициализируем базу данных
    if (!DatabaseManager::instance().initialize()) {
        qDebug() << "❌ Критическая ошибка: не удалось подключиться к базе данных";
        
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка подключения");
        msgBox.setText("Не удалось подключиться к базе данных.\n"
                      "Проверьте интернет-соединение.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return -1;
    }
    
    // Проверяем подключение
    if (!DatabaseManager::instance().isConnected()) {
        qDebug() << "❌ Нет подключения к базе данных";
        QMessageBox::critical(nullptr, "Ошибка подключения", 
            "Подключение к базе данных отсутствует.");
        return -1;
    }
    
    qDebug() << "✅ Подключение к базе данных установлено";
    qDebug() << "==========================================";
    
    // Создаем и показываем окно авторизации
    AuthWindow authWindow;
    authWindow.show();
    
    qDebug() << "Окно авторизации запущено";
    qDebug() << "==========================================";
    
    return app.exec();
}