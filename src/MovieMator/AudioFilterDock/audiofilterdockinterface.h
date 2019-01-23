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
