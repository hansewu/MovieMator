#include "filterdock.h"
#include "ui_filterdock.h"
#include <QQuickWidget>
#include <QDir>
#include <QString>
#include <filterdockinterface.h>

FilterDock::FilterDock(MainInterface *main, QWidget *parent):
    QDockWidget(parent),
  ui(new Ui::filterdock)
{
//    ui->setupUi(this);
    QQuickWidget *view = new QQuickWidget(this);
    view->setSource(QUrl::fromLocalFile("myqmlfile.qml"));
    view->show();
}
FilterDock::~FilterDock()
{

}

static FilterDock *instance = 0;
//初始化模块
//参数，main 主程序接口对象
//返回界面对象
QDockWidget *FilterDock_initModule(MainInterface *main)
{
    if (instance == NULL)
        instance = new FilterDock(main);
    return instance;
}

//销毁模块
void FilterDock_destroyModule()
{

}


int setFiltersInfo(Filter_Info * filterInfos, int nFilterCount)
{
    return 0;
}

int getCurrentSelectedFilterIndex()
{
    return 0;
}
