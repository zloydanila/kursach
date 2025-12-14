#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class Sidebar : public QWidget
{
    Q_OBJECT

public:
    explicit Sidebar(const QString& username, QWidget *parent = nullptr);
    
    void setActivePage(int index);

signals:
    void pageChanged(int index);
    void logoutRequested();

private:
    void setupUI();
    void createNavButton(const QString& icon, const QString& text, int index);
    
    QString m_username;
    QVBoxLayout* m_navLayout;
    QVector<QPushButton*> m_navButtons;
    int m_currentPage;
};

#endif
