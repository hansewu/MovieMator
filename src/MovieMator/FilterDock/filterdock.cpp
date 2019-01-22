#include "filterdock.h"
#include "ui_filterdock.h"
#include <QQuickWidget>
#include <QQuickView>
#include <QQmlContext>
#include <filterdockinterface.h>
#include <QDir>
#include <QStandardPaths>
#include <QtQml>
#include <qmlutilities.h>
#include <qmlview.h>
//#include <Logger.h>

static MainInterface * mainInterface;
QList<FilterItemInfo*> *filterInfoList = new QList<FilterItemInfo*>();

FilterDock::FilterDock(MainInterface *main, QWidget *parent):
    QDockWidget(parent),
//  ui(new Ui::FilterDock),
  m_qview(QmlUtilities::sharedEngine(), this)
{
    m_mainWindow = main;
    m_pFilterInfo = NULL;
    m_qview.setFocusPolicy(Qt::StrongFocus);
    setWidget(&m_qview);

    QmlUtilities::setCommonProperties(m_qview.rootContext());


    UpdateFilters();
}
FilterDock::~FilterDock()
{
    if(m_pFilterInfo) {delete m_pFilterInfo; m_pFilterInfo = NULL;}
}

void FilterDock::resetQview()
{
    QDir viewPath = QmlUtilities::qmlDir();
    viewPath.cd("views");
    viewPath.cd("filter");
    m_qview.engine()->addImportPath(viewPath.path());

    QDir modulePath = QmlUtilities::qmlDir();
    modulePath.cd("modules");
    m_qview.engine()->addImportPath(modulePath.path());

    m_qview.setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qview.quickWindow()->setColor(palette().window().color());
    QUrl source = QUrl::fromLocalFile(viewPath.absoluteFilePath("FiltersUI.qml"));
    m_qview.setSource(source);
}

int FilterDock::UpdateFilters()
{
    if(filterInfoList->size() <= 0)  return -1;

    if(m_pFilterInfo) {delete m_pFilterInfo; m_pFilterInfo = NULL;}

    m_pFilterInfo = new FiltersInfo(filterInfoList);
    qmlRegisterType<FilterItemInfo>("FilterItemInfo", 1, 0, "FilterItemInfo");

    m_qview.rootContext()->setContextProperty("filtersInfo", m_pFilterInfo);
    m_qview.rootContext()->setContextProperty("filtersResDock", this);

    resetQview();

    return 0;
}

void FilterDock::addFilterItem(int index)
{
    m_mainWindow->addFilter(index);
}


//void FiltersInfo::addFilterItem(int index)
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
        filterInfo->setVisible(QString(filterInfos[i].visible));
        filterInfo->setName(QString(filterInfos[i].name));
        filterInfo->setFilterType(QString(filterInfos[i].type));
        filterInfo->setImageSourcePath(QString(filterInfos[i].imageSourcePath));
        filterInfoList->append(filterInfo);
    }
    instance->UpdateFilters();
    return 0;
}

int getCurrentSelectedFilterIndex()
{
    return 0;
}
