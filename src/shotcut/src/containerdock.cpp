#include "containerdock.h"
#include <QGridLayout>
#include <QIcon>
#include <QHBoxLayout>
#include <QPushButton>

ContainerDock::ContainerDock(QWidget *parent) : QDockWidget(parent)
{
    setTitleBarWidget(new QWidget());
    setFeatures(QDockWidget::NoDockWidgetFeatures);

    QWidget *layoutWidget = new QWidget;
    layoutWidget->setMinimumWidth(300);
    layoutWidget->setMinimumHeight(320);
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

    m_tabBtnWidget = new QWidget(this);
    m_tabBtnWidget->setStyleSheet(".QWidget{background-color: rgb(82,82,82);border-radius:5px}");
    m_tabBtnWidget->setFixedHeight(33);
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0,0,0,0);
    hLayout->addStretch();
    m_tabBtnWidget->setLayout(hLayout);

    gLayout->addWidget(m_tabBtnWidget,1,0,1,1);

    layoutWidget->setLayout(gLayout);

    setWidget(layoutWidget);
}


void ContainerDock::addDock(QDockWidget *dock, QString tabButtonTitle, QIcon tabButtonNormalIcon, QIcon tabButtonAcitveIcon)
{
    QGridLayout *gLayout = (QGridLayout *)widget()->layout();
    gLayout->addWidget(dock,0,0,1,1);
    if (m_docks.count() > 0)
        dock->hide();
    else
        dock->show();
    m_docks.append(dock);

    QHBoxLayout *hLayout = (QHBoxLayout *)m_tabBtnWidget->layout();
    QPushButton *button = new QPushButton(tabButtonNormalIcon, tabButtonTitle);
    button->setFixedSize(90, 35);
    if (m_buttons.count() > 0)
        button->setStyleSheet("background-color:rgb(82,82,82);color:rgb(225,225,225)");
    else
    {
        button->setStyleSheet("background-color:rgb(51,51,51);color:rgb(225,225,225)");
        button->setIcon(tabButtonAcitveIcon);
    }
    hLayout->insertWidget(hLayout->count()-1,button);
    BUTTON_INFO buttonInfo;
    buttonInfo.button = button;
    buttonInfo.normalIcon = tabButtonNormalIcon;
    buttonInfo.activeIcon = tabButtonAcitveIcon;
    m_buttons.append(buttonInfo);

    connect(button, SIGNAL(clicked()), this, SLOT(onTabButtonClicked()));
    connect(button, SIGNAL(clicked()), dock, SLOT(show()));
    connect(button, SIGNAL(clicked()), this, SLOT(onTabButtonClicked2()));
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
        button->setStyleSheet("background-color:rgb(82,82,82);color:rgb(225,225,225)");
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
    button->setStyleSheet("background-color:rgb(51,51,51);color:rgb(225,225,225)");
}

