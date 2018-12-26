#ifndef CONTAINERDOCK_H
#define CONTAINERDOCK_H

#include<QDockWidget>
#include<QList>
#include<QPushButton>

typedef struct button_info{
    QPushButton *button;
    QIcon normalIcon;
    QIcon activeIcon;
} BUTTON_INFO;

class ContainerDock : public QDockWidget
{
    Q_OBJECT

public:
    ContainerDock(QWidget *parent = 0);

    void addDock(QDockWidget *dock, QString tabButtonTitle, QIcon tabButtonNormalIcon, QIcon tabButtonAcitveIcon);

public slots:
    void onTabButtonClicked();
    void onTabButtonClicked2();

private:
    QWidget *m_tabBtnWidget;
    QList<BUTTON_INFO> m_buttons;
    QList<QDockWidget *> m_docks;
};

#endif // CONTAINERDOCK_H
