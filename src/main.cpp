#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "🚀 SoundSpace starting...";
    
    QMainWindow window;
    window.setWindowTitle("SoundSpace");
    window.resize(600, 400);
    
    QLabel *label = new QLabel("🎵 SoundSpace MVP\n\nDatabase architecture in progress...");
    label->setAlignment(Qt::AlignCenter);
    window.setCentralWidget(label);
    
    window.show();
    
    return app.exec();
}