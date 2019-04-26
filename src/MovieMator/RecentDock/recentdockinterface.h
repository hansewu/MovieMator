/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: WanYuanCN <ebthon@hotmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
RECENTDOCKSHARED_EXPORT QList<FILE_HANDLE> RecentDock_getSelectedFiles();

//添加文件
RECENTDOCKSHARED_EXPORT void RecentDock_add(QString filePath);

//删除文件
RECENTDOCKSHARED_EXPORT void RecentDock_remove(QString filePath);

//删除选中项
RECENTDOCKSHARED_EXPORT void RecentDock_removeSelectedItem();



#endif // RECENTDOCK_H
