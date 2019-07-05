#ifndef VIDEOFILTERDOCKINTERFACE_H
#define VIDEOFILTERDOCKINTERFACE_H

#include <QDockWidget>
#include <maininterface.h>
#include "videofilterdock_global.h"

typedef struct {
    char name[256];
    char type[256];
    char imageSourcePath[256];
    bool visible;
} VideoFilter_Info;

//初始化模块
//参数，main 主程序接口对象
//返回界面对象
VIDEOFILTERDOCKSHARED_EXPORT QDockWidget *VideoFilterDock_initModule(MainInterface *main);

//销毁模块
VIDEOFILTERDOCKSHARED_EXPORT void VideoFilterDock_destroyModule();

VIDEOFILTERDOCKSHARED_EXPORT int setVideoFiltersInfo(VideoFilter_Info *filterInfos, int nFilterCount);

#endif // VIDEOFILTERDOCKINTERFACE_H
