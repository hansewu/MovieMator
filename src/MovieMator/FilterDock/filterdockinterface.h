#ifndef FILTERDOCKINTERFACE_H
#define FILTERDOCKINTERFACE_H

#include <QDockWidget>
#include <maininterface.h>
#include "filterdock_global.h"

typedef struct {
    char name[256];
    char type[256];
    char imageSourcePath[256];
} Filter_Info;

//初始化模块
//参数，main 主程序接口对象
//返回界面对象
FILTERDOCKSHARED_EXPORT QDockWidget *FilterDock_initModule(MainInterface *main);

//销毁模块
FILTERDOCKSHARED_EXPORT void FilterDock_destroyModule();


FILTERDOCKSHARED_EXPORT int setFiltersInfo(Filter_Info * filterInfos, int nFilterCount);

FILTERDOCKSHARED_EXPORT int getCurrentSelectedFilterIndex();

#endif // FILTERSELECTDOCKINTERFACE_H
