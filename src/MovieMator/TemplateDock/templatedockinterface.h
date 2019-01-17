#ifndef TEMPLATEDOCK_H
#define TEMPLATEDOCK_H


#include <QDockWidget>
#include <maininterface.h>
#include "templatedock_global.h"

//初始化模块
//参数，main 主程序接口对象
//返回界面对象
TEMPLATEDOCKSHARED_EXPORT QDockWidget *TemplateDock_initModule(MainInterface *main);

//销毁模块
TEMPLATEDOCKSHARED_EXPORT void TemplateDock_destroyModule();

#endif // TEMPLATEDOCK_H
