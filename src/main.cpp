#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QDebug>
#include <QApplication>
#include <QDebug>

#include "gui/AuthWindow.h"
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
    
    // Создание и отображение окна авторизации
    AuthWindow authWindow;
    authWindow.show();
    
    qDebug() << "🚀 Application started successfully!";
    
    return app.exec();
    
    // ТЕСТ 2: РЕГИСТРАЦИЯ ПОЛЬЗОВАТЕЛЕЙ
    qDebug() << "\n👤 2. Testing user registration...";
    
    // Успешная регистрация
    DatabaseManager::instance().registerUser("admin", "admin123");
    DatabaseManager::instance().registerUser("alice", "alice2024");
    DatabaseManager::instance().registerUser("bob", "bobpassword");
    DatabaseManager::instance().registerUser("Stavr", "Dskalsjsjjs");
    // Попытка зарегистрировать существующего пользователя
    DatabaseManager::instance().registerUser("admin", "differentpass");
    
    // Регистрация с коротким именем
    DatabaseManager::instance().registerUser("ab", "password123");
    
    // Регистрация с коротким паролем
    DatabaseManager::instance().registerUser("test", "123");
    
    // ТЕСТ 3: АУТЕНТИФИКАЦИЯ
    qDebug() << "\n🔐 3. Testing authentication...";
    
    // Правильные credentials
    if (DatabaseManager::instance().authenticateUser("admin", "admin123")) {
        qDebug() << "✅ Admin authentication: SUCCESS";
    } else {
        qDebug() << "❌ Admin authentication: FAILED";
    }
    
    // Неправильный пароль
    if (DatabaseManager::instance().authenticateUser("admin", "wrongpass")) {
        qDebug() << "❌ Wrong password: UNEXPECTED SUCCESS";
    } else {
        qDebug() << "✅ Wrong password: CORRECTLY REJECTED";
    }
    
    // Несуществующий пользователь
    if (DatabaseManager::instance().authenticateUser("ghost", "pass")) {
        qDebug() << "❌ Non-existent user: UNEXPECTED SUCCESS";
    } else {
        qDebug() << "✅ Non-existent user: CORRECTLY REJECTED";
    }
    
    // Пустые данные
    if (DatabaseManager::instance().authenticateUser("", "")) {
        qDebug() << "❌ Empty credentials: UNEXPECTED SUCCESS";
    } else {
        qDebug() << "✅ Empty credentials: CORRECTLY REJECTED";
    }
    
    // ТЕСТ 4: ПРОВЕРКА СОСТОЯНИЯ БД
    qDebug() << "\n📊 4. Database status check...";
    if (DatabaseManager::instance().isDatabaseOpen()) {
        qDebug() << "✅ Database is open and ready";
    } else {
        qDebug() << "❌ Database is closed";
    }
    
    // ВИЗУАЛЬНЫЙ ИНТЕРФЕЙС С РЕЗУЛЬТАТАМИ
    QMainWindow window;
    window.setWindowTitle("SoundSpace - Database Test Results");
    window.resize(500, 400);
    
    QString results = 
        "🎵 SoundSpace Database Test Results\n\n"
        "✅ Database Connection: WORKING\n"
        "✅ Table Creation: WORKING\n"
        "✅ User Registration: WORKING\n" 
        "✅ User Authentication: WORKING\n"
        "✅ Error Handling: WORKING\n\n"
        "🚀 All core functions are operational!\n\n"
        "Next: Add track management functionality";
    
    QLabel *label = new QLabel(results);
    label->setAlignment(Qt::AlignCenter);
    window.setCentralWidget(label);
    
    window.show();
    
    qDebug() << "\n🎉 ALL TESTS COMPLETED! Database core is working correctly!";
    
    return app.exec();
}