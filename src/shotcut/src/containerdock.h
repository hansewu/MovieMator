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

typedef enum{
    TabPosition_Left = 0,
    TabPosition_Bottom,
    TabPosition_Right,
    TabPosition_Top
}TAB_POSITION;

class ContainerDock : public QDockWidget
{
    Q_OBJECT
    Q_PROPERTY(QRect dockPosition READ dockPosition NOTIFY dockPositionChanged)
public:


    ContainerDock(TAB_POSITION tabPosition = TabPosition_Left, QWidget *parent = 0);

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
    TAB_POSITION m_tabPostion;
};

#endif // CONTAINERDOCK_H
