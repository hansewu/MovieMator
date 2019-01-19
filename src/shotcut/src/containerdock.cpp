#include "containerdock.h"
#include <QGridLayout>
#include <QIcon>
#include <QHBoxLayout>
#include <QPushButton>

static QString buttonSytleSheetNormal("background-color:rgb(82,82,82);color:rgb(225,225,225)");
static QString buttonSytleSheetSelected("background-color:rgb(51,51,51);color:rgb(192,72,44)");

#define TABBAR_WIDTH 56
#define TABBAR_HEIGHT 33
#define BUTTON_WIDTH 90
#define BUTTON_HEIGHT 35
#define BUTTON_WIDTH_SQUARE 56
#define BUTTON_HEIGHT_SQUARE 56


ContainerDock::ContainerDock(TAB_POSITION tabPosition, QWidget *parent)
    : QDockWidget(parent)
    , m_tabPostion(tabPosition)
{
    setTitleBarWidget(new QWidget());
    setFeatures(QDockWidget::NoDockWidgetFeatures);

    //Tab bar
    m_tabBtnWidget = new QWidget(this);
    if (m_tabPostion == TabPosition_Left)
    {
        m_tabBtnWidget->setStyleSheet(".QWidget{background-color: rgb(82,82,82);border-radius:1px}");
        m_tabBtnWidget->setFixedWidth(56);
        QVBoxLayout *tabButtonLayout = new QVBoxLayout();
        tabButtonLayout->setSpacing(0);
        tabButtonLayout->setContentsMargins(0,0,0,0);
        tabButtonLayout->addStretch();
        m_tabBtnWidget->setLayout(tabButtonLayout);
    }
    else if (m_tabPostion == TabPosition_Bottom)
    {
        m_tabBtnWidget->setStyleSheet(".QWidget{background-color: rgb(82,82,82);border-radius:5px}");
        m_tabBtnWidget->setFixedHeight(33);
        QHBoxLayout *tabButtonLayout = new QHBoxLayout();
        tabButtonLayout->setSpacing(0);
        tabButtonLayout->setContentsMargins(0,0,0,0);
        tabButtonLayout->addStretch();
        m_tabBtnWidget->setLayout(tabButtonLayout);
    }



    QWidget *layoutWidget = new QWidget;
//    layoutWidget->setMinimumWidth(300);
//    layoutWidget->setMinimumHeight(320);
    layoutWidget->setContentsMargins(0,0,0,0);
    QString strStyle = "QScrollBar::vertical{background-color:rgb(51,51,51);width:14px;border: 3px solid rgb(51,51,51);}";
    strStyle.append("QScrollBar::handle:vertical{background:#787878;border-radius:4px;}");
    strStyle.append("QScrollBar::add-page:vertical{background:none;}");
    strStyle.append("QScrollBar::sub-page:vertical{background:none;}");
    strStyle.append("QScrollBar::add-line:vertical{height: 0px; background:none;}");
    strStyle.append("QScrollBar::sub-line:vertical{height: 0px; background:none;}");
    strStyle.append("QScrollBar::horizontal{background-color:rgb(51,51,51);height:14px;border: 3px solid rgb(51,51,51);}");
    strStyle.append("QScrollBar::handle:horizontal{background:#787878;border-radius:4px;}");
    strStyle.append("QScrollBar::add-page:horizontal{background:none;}");
    strStyle.append("QScrollBar::sub-page:horizontal{background:none;}");
    strStyle.append("QScrollBar::add-line:horizontal{width:0px; background:none;}");
    strStyle.append("QScrollBar::sub-line:horizontal{width:0px; background:none;}");
    strStyle.append(".QWidget{background-color:rgb(51,51,51)}");
    layoutWidget->setStyleSheet(strStyle);


    QGridLayout *gLayout = new QGridLayout(layoutWidget);
    gLayout->setContentsMargins(0,0,0,0);
    gLayout->setSpacing(0);
    if (m_tabPostion == TabPosition_Left)
        gLayout->addWidget(m_tabBtnWidget,0,0,1,1);
    else if (m_tabPostion == TabPosition_Bottom)
        gLayout->addWidget(m_tabBtnWidget, 1,0,1,1);

    layoutWidget->setLayout(gLayout);

    setWidget(layoutWidget);
}


void ContainerDock::addDock(QDockWidget *dock, QString tabButtonTitle, QIcon tabButtonNormalIcon, QIcon tabButtonAcitveIcon)
{
    QGridLayout *gLayout = (QGridLayout *)widget()->layout();
    if (m_tabPostion == TabPosition_Left)
        gLayout->addWidget(dock,0,1,1,1);
    else if (m_tabPostion == TabPosition_Bottom)
        gLayout->addWidget(dock,0,0,1,1);

    if (m_docks.count() > 0)
        dock->hide();
    else
        dock->show();
    m_docks.append(dock);


    QPushButton *button = new QPushButton(tabButtonNormalIcon, tabButtonTitle);

    if (m_buttons.count() > 0)
        button->setStyleSheet(buttonSytleSheetNormal);
    else
    {
        button->setStyleSheet(buttonSytleSheetSelected);
        button->setIcon(tabButtonAcitveIcon);
    }

    if (m_tabPostion == TabPosition_Left)
    {
        button->setFixedSize(BUTTON_WIDTH_SQUARE, BUTTON_HEIGHT_SQUARE);
        QVBoxLayout *vLayout = (QVBoxLayout *)m_tabBtnWidget->layout();
        vLayout->insertWidget(vLayout->count()-1,button);
    }
    else if (m_tabPostion == TabPosition_Bottom)
    {
        button->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
        QHBoxLayout *hLayout = (QHBoxLayout *)m_tabBtnWidget->layout();
        hLayout->insertWidget(hLayout->count()-1,button);
    }

    BUTTON_INFO buttonInfo;
    buttonInfo.button = button;
    buttonInfo.normalIcon = tabButtonNormalIcon;
    buttonInfo.activeIcon = tabButtonAcitveIcon;
    m_buttons.append(buttonInfo);

    connect(button, SIGNAL(clicked()), this, SLOT(onTabButtonClicked()));
    connect(button, SIGNAL(clicked()), dock, SLOT(show()));
    connect(button, SIGNAL(clicked()), this, SLOT(onTabButtonClicked2()));
}

void ContainerDock::showDock(QDockWidget *dock)
{
    if(!m_docks.contains(dock))
        return;

    if (dock->isVisible())
        return;

    onTabButtonClicked();

    dock->show();
    int index = m_docks.indexOf(dock);
    BUTTON_INFO buttonInfo = m_buttons[index];
    QPushButton *button = buttonInfo.button;
    button->setIcon(buttonInfo.activeIcon);
    button->setStyleSheet(buttonSytleSheetSelected);
}


void ContainerDock::onTabButtonClicked()
{
    int i = 0;
    for (i = 0; i < m_docks.count(); i++)
    {
        QDockWidget *dock = m_docks[i];
        dock->setHidden(true);
    }

    for (i = 0; i < m_buttons.count(); i++)
    {
        BUTTON_INFO buttonInfo = m_buttons[i];
        QPushButton *button = buttonInfo.button;
        button->setIcon(buttonInfo.normalIcon);
        button->setStyleSheet(buttonSytleSheetNormal);
    }
}

void ContainerDock::onTabButtonClicked2()
{
    int index = -1;
    int i = 0;
    for (i = 0; i < m_docks.count(); i++)
    {
        QDockWidget *dock = m_docks[i];
        if (dock->isVisible())
        {
            index = i;
            break;
        }
    }
\
    BUTTON_INFO buttonInfo = m_buttons[index];
    QPushButton *button = buttonInfo.button;
    button->setIcon(buttonInfo.activeIcon);
    button->setStyleSheet(buttonSytleSheetSelected);
}

QRect ContainerDock::dockPosition()
{
    return this->geometry();
}

