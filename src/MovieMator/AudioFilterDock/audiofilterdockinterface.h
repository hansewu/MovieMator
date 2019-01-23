#ifndef AUDIOFILTERDOCKINTERFACE_H
#define AUDIOFILTERDOCKINTERFACE_H

#include <QDockWidget>
#include <maininterface.h>
#include "audiofilterdock_global.h"

typedef struct {
    char name[256];
    char type[256];
    char imageSourcePath[256];
    bool visible;
} AudioFilter_Info;

//初始化模块
//参数，main 主程序接口对象
//返回界面对象
AUDIOFILTERDOCKSHARED_EXPORT QDockWidget *AudioFilterDock_initModule(MainInterface *main);

//销毁模块
AUDIOFILTERDOCKSHARED_EXPORT void AudioFilterDock_destroyModule();


AUDIOFILTERDOCKSHARED_EXPORT int setAudioFiltersInfo(AudioFilter_Info * audiofilterInfos, int nAudioFilterCount);

AUDIOFILTERDOCKSHARED_EXPORT int getCurrentSelectedAudioFilterIndex();


#endif // AUDIOFILTERDOCKINTERFACE_H
