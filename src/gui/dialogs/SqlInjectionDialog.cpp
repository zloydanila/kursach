#include "SqlInjectionDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>

SqlInjectionDialog::SqlInjectionDialog(QWidget* parent)
    : QDialog(parent)
{
    setFixedSize(740, 320);
    setWindowTitle("Обнаружена SQL-инъекция");

    setStyleSheet(R"(
        QDialog { background: #0F0F14; border-radius: 16px; }
        QLabel#title { color: #FFFFFF; font-size: 22px; font-weight: 900; }
        QLabel#text  { color: rgba(255,255,255,0.85); font-size: 15px; }
        QPushButton#okBtn {
            background: #8A2BE2; color: white; border: none;
            border-radius: 12px; padding: 12px 20px; min-width: 160px; font-weight: 700;
        }
        QPushButton#okBtn:hover { background: #9B4BFF; }
    )");

    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(20);

    auto* img = new QLabel;
    img->setFixedSize(300, 300);
    img->setAlignment(Qt::AlignCenter);
    img->setStyleSheet("QLabel { padding: 0px; margin: 0px; background: rgba(255,255,255,0.06); border-radius: 14px; }");
    img->setContentsMargins(0, 0, 0, 0);
    img->setContentsMargins(0, 0, 0, 0);
    img->setStyleSheet("QLabel { padding: 0px; margin: 0px; background: rgba(255,255,255,0.06); border-radius: 14px; }");

    QPixmap px(":/authwindow/image.png");
    if (!px.isNull()) {
        const QSize box = img->contentsRect().size();
        img->setPixmap(px.scaled(box, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        img->setAlignment(Qt::AlignCenter);
    }
    img->setScaledContents(false);

    auto* right = new QVBoxLayout;
    right->setSpacing(12);

    auto* title = new QLabel("Обнаружена SQL-инъекция");
    title->setObjectName("title");

    auto* text = new QLabel("Ввод похож на попытку SQL-инъекции.\nПроверьте данные и попробуйте снова.");
    text->setObjectName("text");
    text->setWordWrap(true);

    auto* ok = new QPushButton("OK");
    ok->setObjectName("okBtn");
    ok->setFixedHeight(46);
    connect(ok, &QPushButton::clicked, this, &QDialog::accept);

    right->addWidget(title);
    right->addWidget(text, 1);
    right->addWidget(ok, 0, Qt::AlignRight);

    root->addWidget(img);
    root->addLayout(right, 1);
}
