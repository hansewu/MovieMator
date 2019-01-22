#ifndef TEMPLATEEDITORDOCK_H
#define TEMPLATEEDITORDOCK_H

#include <QDockWidget>
#include <maininterface.h>
#include "templateeditordock_global.h"


//初始化模块
//参数，main 主程序接口对象
//返回界面对象
TEMPLATEEDITORDOCKSHARED_EXPORT QDockWidget *TemplateEditorDock_initModule(MainInterface *main);

//销毁模块
TEMPLATEEDITORDOCKSHARED_EXPORT void TemplateEditorDock_destroyModule();

//初始化界面
TEMPLATEEDITORDOCKSHARED_EXPORT void TemplateEditorDock_initTemplateEditor(QList<FILE_HANDLE> fileList);

#endif // TEMPLATEEDITORDOCK_H
