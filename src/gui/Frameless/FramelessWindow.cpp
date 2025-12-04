#include "FramelessWindow.h"
#include <QApplication>
#include <QDebug>

FramelessWindow::FramelessWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Включаем сглаживание
    setAttribute(Qt::WA_NoSystemBackground, false);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    
    // Добавляем тень окну с более мягкими настройками
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(25); // Увеличиваем размытие
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setColor(QColor(0, 0, 0, 40)); // Более прозрачная тень
    
    QWidget *central = new QWidget();
    central->setObjectName("windowContent");
    central->setGraphicsEffect(shadow);
    
    // Устанавливаем сглаживание для центрального виджета
    central->setAttribute(Qt::WA_NoSystemBackground, false);
    central->setAttribute(Qt::WA_OpaquePaintEvent, true);
    
    // Устанавливаем фоновую кисть с сглаживанием
    QPalette pal = central->palette();
    pal.setColor(QPalette::Window, QColor(15, 15, 20));
    central->setPalette(pal);
    central->setAutoFillBackground(true);
    
    setCentralWidget(central);
}

void FramelessWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_dragging) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}