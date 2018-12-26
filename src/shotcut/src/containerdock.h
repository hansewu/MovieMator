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
    Q_PROPERTY(QRect dockPosition READ dockPosition NOTIFY dockPositionChanged)
public:
    ContainerDock(QWidget *parent = 0);

    void addDock(QDockWidget *dock, QString tabButtonTitle, QIcon tabButtonNormalIcon, QIcon tabButtonAcitveIcon);

    void showDock(QDockWidget *dock);
    QRect dockPosition();

public slots:
    void onTabButtonClicked();
    void onTabButtonClicked2();

signals:
    void dockPositionChanged();

private:
    QWidget *m_tabBtnWidget;
    QList<BUTTON_INFO> m_buttons;
    QList<QDockWidget *> m_docks;
};

#endif // CONTAINERDOCK_H
