#include <QApplication>
#include <QFile>
#include <QDebug>

#include "gui/AuthWindow/AuthWindow.h"
#include "database/DatabaseManager.h"

static void loadStyleSheet(QApplication& app)
{
    QFile f("src/gui/style/app.qss");
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Cannot open stylesheet:" << f.errorString();
        return;
    }
    app.setStyleSheet(QString::fromUtf8(f.readAll()));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setQuitOnLastWindowClosed(false);
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
