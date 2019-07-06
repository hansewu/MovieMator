#ifndef TEXTDOCKINTERFACE_H
#define TEXTDOCKINTERFACE_H

#include <QDockWidget>
#include <maininterface.h>
#include "textdock_global.h"

//初始化模块
//参数，main 主程序接口对象
//返回界面对象
TEXTDOCKSHARED_EXPORT QDockWidget *TextDock_initModule(MainInterface *main);

//销毁模块
TEXTDOCKSHARED_EXPORT void TextDock_destroyModule();

#endif // TEXTDOCKINTERFACE_H
