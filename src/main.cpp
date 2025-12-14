#include <QApplication>
#include <QFile>
#include "gui/AuthWindow/AuthWindow.h"
#include "database/DatabaseManager.h"
#include "utils/Config.h"
#include <QDebug>

void loadStyleSheet(QApplication& app)
{
    QString styleSheet = R"(
        * {
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        
        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #0F0F14, stop:1 #1A1A21);
        }
        
        QScrollBar:vertical {
            background: rgba(255, 255, 255, 0.05);
            width: 10px;
            border-radius: 5px;
            margin: 0px;
        }
        
        QScrollBar::handle:vertical {
            background: rgba(138, 43, 226, 0.5);
            border-radius: 5px;
            min-height: 30px;
        }
        
        QScrollBar::handle:vertical:hover {
            background: rgba(138, 43, 226, 0.7);
        }
        
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0px;
        }
        
        QToolTip {
            background: #1A1A21;
            color: white;
            border: 1px solid rgba(138, 43, 226, 0.5);
            border-radius: 4px;
            padding: 5px;
        }
    )";
    
    app.setStyleSheet(styleSheet);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("Chorus");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Chorus");
    
    loadStyleSheet(app);
    
    if (!DatabaseManager::instance().initialize()) {
        qCritical() << "Failed to initialize database";
        return 1;
    }
    
    AuthWindow authWindow;
    authWindow.show();
    
    return app.exec();
}
