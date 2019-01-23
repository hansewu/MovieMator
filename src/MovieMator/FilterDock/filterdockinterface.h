/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl1987527 <wyl1987527@163.com>
 * Author: Author: fuyunhuaxin <2446010587@qq.com>
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

#ifndef FILTERDOCKINTERFACE_H
#define FILTERDOCKINTERFACE_H

#include <QDockWidget>
#include <maininterface.h>
#include "filterdock_global.h"

typedef struct {
    char name[256];
    char type[256];
    char imageSourcePath[256];
    bool visible;
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
