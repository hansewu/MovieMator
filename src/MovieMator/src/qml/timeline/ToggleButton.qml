/*
 * Copyright (c) 2014 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
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

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import MovieMator.Controls 1.0 as MovieMator

// ？？？？
// 没有地方用到
// 切换什么功能的按钮
CheckBox {
    // 复选框名称
    property string iconName
    // 复选框图标
    property url iconSource
    // 复选框的小提示
    property alias tooltip: tooltip.text

    anchors.verticalCenter: parent.verticalCenter
    style: CheckBoxStyle {
        background: Rectangle {
            implicitWidth: 28
            implicitHeight: 24
            radius: 3
            SystemPalette { id: activePalette }
            color: control.checked? activePalette.highlight : activePalette.button
            border.color: activePalette.shadow
            border.width: 1
        }
        indicator: ToolButton {
            x: 3
            implicitWidth: 24
            implicitHeight: 20
            iconName: control.iconName
            iconSource: control.iconSource
        }
    }
    MovieMator.ToolTip { id: tooltip }
}
