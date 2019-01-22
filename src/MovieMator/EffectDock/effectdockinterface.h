#ifndef EFFECTDOCK_H
#define EFFECTDOCK_H

#include <QDockWidget>
#include <maininterface.h>
#include "effectdock_global.h"

//初始化模块
//参数，main 主程序接口对象
//返回界面对象
EFFECTDOCKSHARED_EXPORT QDockWidget *EffectDock_initModule(MainInterface *main);

//销毁模块
EFFECTDOCKSHARED_EXPORT void EffectDock_destroyModule();

#endif // EFFECTDOCK_H
