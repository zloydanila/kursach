#include <QApplication>
#include <QDebug>

#include "gui/AuthWindow/AuthWindow.h"
#include "database/DatabaseManager.h"

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
    
    // 🔽 ТЕСТИРОВАНИЕ БД (РАСКОММЕНТИРУЙ ЕСЛИ НУЖНО)
    /*
    qDebug() << "\n👤 Testing user registration...";
    DatabaseManager::instance().registerUser("admin", "admin123");
    DatabaseManager::instance().registerUser("alice", "alice2024");
    DatabaseManager::instance().registerUser("bob", "bobpassword");
    DatabaseManager::instance().registerUser("Stavr", "Dskalsjsjjs");
    
    qDebug() << "\n🔐 Testing authentication...";
    if (DatabaseManager::instance().authenticateUser("admin", "admin123")) {
        qDebug() << "✅ Admin authentication: SUCCESS";
    } else {
        qDebug() << "❌ Admin authentication: FAILED";
    }
    */
    
    // Создание и отображение окна авторизации
    AuthWindow authWindow;
    authWindow.show();
    
    qDebug() << "🚀 Application started successfully!";
    
    return app.exec();
}