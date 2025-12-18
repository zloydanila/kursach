#include "NotificationDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>

NotificationDialog::NotificationDialog(const QString &message, Type type, QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    setupUI(message, type);
}

NotificationDialog::~NotificationDialog() {}

void NotificationDialog::setupUI(const QString &message, Type type)
{
    setStyleSheet(R"(
        QToolTip {
            background: #12121A;
            color: rgba(255,255,255,0.92);
            border: 1px solid rgba(138, 43, 226, 0.45);
            padding: 6px 8px;
            border-radius: 8px;
            font-size: 12px;
        }
    )");

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QWidget *bg = new QWidget();
    bg->setObjectName("notifyBg");
    bg->setStyleSheet(R"(
        #notifyBg {
            background: #0F0F14;
            border-radius: 16px;
            border: 1px solid rgba(255, 255, 255, 0.08);
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(bg);
    layout->setContentsMargins(18, 16, 18, 14);
    layout->setSpacing(12);

    QString accent = "#8A2BE2";
    if (type == Error) accent = "#FF4D4D";
    if (type == Success) accent = "#8A2BE2";

    QHBoxLayout *header = new QHBoxLayout();
    header->setContentsMargins(0, 0, 0, 0);
    header->setSpacing(10);

    QLabel *dot = new QLabel();
    dot->setFixedSize(10, 10);
    dot->setStyleSheet(QString("background: %1; border-radius: 5px;").arg(accent));

    QLabel *title = new QLabel();
    title->setText(type == Error ? "Ошибка" : (type == Success ? "Успех" : "Информация"));
    title->setStyleSheet(R"(
        QLabel {
            color: rgba(255, 255, 255, 0.95);
            font-size: 15px;
            font-weight: 700;
            background: transparent;
        }
    )");

    header->addWidget(dot);
    header->addWidget(title);
    header->addStretch();

    QLabel *messageLabel = new QLabel(message);
    messageLabel->setWordWrap(true);
    messageLabel->setTextFormat(Qt::PlainText);
    messageLabel->setStyleSheet(R"(
        QLabel {
            color: rgba(255, 255, 255, 0.86);
            font-size: 14px;
            background: transparent;
        }
    )");

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setContentsMargins(0, 0, 0, 0);
    btnRow->setSpacing(10);

    QPushButton *okBtn = new QPushButton("OK");
    okBtn->setFixedHeight(36);
    okBtn->setMinimumWidth(96);
    okBtn->setCursor(Qt::PointingHandCursor);
    okBtn->setStyleSheet(QString(R"(
        QPushButton {
            background: %1;
            color: #FFFFFF;
            border: none;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton:hover { background: rgba(155, 75, 255, 1.0); }
        QPushButton:pressed { background: rgba(123, 31, 162, 1.0); }
    )").arg(accent));
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    btnRow->addStretch();
    btnRow->addWidget(okBtn);

    layout->addLayout(header);
    layout->addWidget(messageLabel);
    layout->addLayout(btnRow);

    rootLayout->addWidget(bg);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(22);
    shadow->setColor(QColor(0, 0, 0, 120));
    shadow->setOffset(0, 6);
    bg->setGraphicsEffect(shadow);

    int h = 140;
    int w = 440;
    int lines = message.count('\n') + 1;
    h += (lines > 2 ? 18 : 0);
    resize(w, h);
}
