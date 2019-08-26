/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author:
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

//可以根据需求修改订制的内容
import QtQuick 2.6  //根据QT版本变化
import Qt.labs.controls 1.0
ListView {
    property alias scrollBarVisible: scrollBar.visible  //设置滚动条的可见性
    boundsBehavior: Flickable.StopAtBounds      //设置内容不能被拖动到可滑动的边界之外。
    ScrollBar.vertical: ScrollBar {         //订制滚动条（可以其他方式订制并绑定滑动行为）
        id: scrollBar
        visible: false
        onActiveChanged: {
            active = true;
        }
        Component.onCompleted: {
            scrollBar.handle.color = "#33000000";   //滚动条颜色
            scrollBar.active = true;
            scrollBar.handle.width = 5;         //滚动条宽度
        }
    }
}
