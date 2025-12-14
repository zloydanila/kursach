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
    resize(420, 150);

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QWidget *bg = new QWidget();
    bg->setObjectName("notifyBg");
    bg->setStyleSheet(R"(
        #notifyBg {
            background: #0F0F14;
            border-radius: 14px;
            border: 1px solid rgba(138, 43, 226, 0.5);
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(bg);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(10);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);

    QLabel *iconLabel = new QLabel();
    iconLabel->setFixedSize(20, 20);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet(R"(
        QLabel {
            color: #8A2BE2;
            font-size: 16px;
            font-weight: bold;
            background: transparent;
            border: none;
        }
    )");
    if (type == Success)
        iconLabel->setText("✓");
    else if (type == Error)
        iconLabel->setText("✕");
    else
        iconLabel->setText("ⓘ");

    QLabel *titleLabel = new QLabel("Уведомление");
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #FFFFFF;
            font-size: 15px;
            font-weight: 600;
            background: transparent;
            border: none;
        }
    )");

    headerLayout->addWidget(iconLabel);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    QLabel *messageLabel = new QLabel(message);
    messageLabel->setWordWrap(true);
    messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    messageLabel->setStyleSheet(R"(
        QLabel {
            color: rgba(255, 255, 255, 0.88);
            font-size: 14px;
            background: transparent;
            border: none;
        }
    )");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(0);

    QPushButton *okBtn = new QPushButton("OK");
    okBtn->setFixedHeight(34);
    okBtn->setMinimumWidth(90);
    okBtn->setStyleSheet(R"(
        QPushButton {
            background: #8A2BE2;
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #9B4BFF;
        }
        QPushButton:pressed {
            background: #7B1FA2;
        }
    )");
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okBtn);

    layout->addLayout(headerLayout);
    layout->addWidget(messageLabel);
    layout->addSpacing(6);
    layout->addLayout(buttonLayout);

    rootLayout->addWidget(bg);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(18);
    shadow->setColor(QColor(0, 0, 0, 90));
    shadow->setOffset(0, 4);
    bg->setGraphicsEffect(shadow);
}

NotificationDialog::~NotificationDialog() {}

void NotificationDialog::setupUI(const QString &message, Type type)
{
}
