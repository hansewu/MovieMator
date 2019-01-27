/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
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
