#include "filterdock.h"
#include "ui_filterdock.h"
#include <QQuickWidget>
#include <QQuickView>
#include <QQmlContext>
#include <filterdockinterface.h>
#include <QDir>
#include <QStandardPaths>
#include <QtQml>

QList<FilterItemInfo*> *filterInfoList = new QList<FilterItemInfo*>();

FilterDock::FilterDock(MainInterface *main, QWidget *parent):
    QDockWidget(parent),
  ui(new Ui::filterdock)
{

    UpdateFilters();

//    QQuickView *view = new QQuickView;
//    FiltersInfo *data = new FiltersInfo(filterInfoList);
//    view->rootContext()->setContextProperty("metadatamodel", data);
//    view->setSource(QUrl::fromLocalFile(filterUIPath));
//    QWidget *container = QWidget::createWindowContainer(view);
//    ui->gridLayout->addWidget(container);

}
FilterDock::~FilterDock()
{

}
int FilterDock::UpdateFilters()
{
    if(filterInfoList->size() <= 0){
        return -1;
    }else{
        QString filterUIPath = qApp->applicationDirPath();

        #if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
            filterUIPath = qApp->applicationDirPath() + "/../share/moviemator/qml/views/filter/FiltersLoader.qml”;
        #elif defined(Q_OS_MAC)
            QDir dir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
            filterUIPath = dir.absolutePath() + "/share/moviemator/qml/views/filter/FiltersLoader.qml";
        #endif

        ui->setupUi(this);
        FiltersInfo *data = new FiltersInfo(filterInfoList);
        qmlRegisterType<FilterItemInfo>("FilterItemInfo", 1, 0, "FilterItemInfo");
        ui->quickWidget->rootContext()->setContextProperty("metadatamodel", data);
        ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
        ui->quickWidget->setSource(QUrl::fromLocalFile(filterUIPath));
    }
    return 0;
}
//FilterItemInfo::FilterItemInfo(QObject *parent){}
//FilterItemInfo::FilterItemInfo(QObject *parent)
//{
//}


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
    filterInfoList->clear();
    for(int i=0;i<nFilterCount;i++){
        FilterItemInfo *filterInfo = new FilterItemInfo();
        filterInfo->setName(QString(filterInfos[i].name));
        filterInfo->setFilterType(QString(filterInfos[i].type));
        filterInfo->setImageSourcePath(QString(filterInfos[i].imageSourcePath));
        filterInfoList->append(filterInfo);
    }
    instance->UpdateFilters();
    return 0;
}

int getCurrentedFilterIndex()
{
    return 0;
}
