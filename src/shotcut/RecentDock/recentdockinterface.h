#ifndef RECENTDOCK_H
#define RECENTDOCK_H


#include <QDockWidget>
#include <maininterface.h>
#include "recentdock_global.h"


//初始化模块
//参数，main 主程序接口对象
//返回界面对象
RECENTDOCKSHARED_EXPORT QDockWidget *RecentDock_initModule(MainInterface *main);

//销毁模块
RECENTDOCKSHARED_EXPORT void RecentDock_destroyModule();

//获取选中的文件列表
RECENTDOCKSHARED_EXPORT QList<QString> RecentDock_getSelectedFiles();

//添加文件
RECENTDOCKSHARED_EXPORT void RecentDock_add(QString filePath);

//删除文件
RECENTDOCKSHARED_EXPORT void RecentDock_remove(QString filePath);



#endif // RECENTDOCK_H
