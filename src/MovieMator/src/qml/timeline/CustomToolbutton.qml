/*
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

import QtQuick 2.0


import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4


// 自定义的时间线工具栏菜单
ToolButton
{
    // 普通状态下的图标
    property url customIconSource
    // 按下状态的图标
    property url pressedIconSource
    // 禁用状态下的图标
    property url disabledIconSource
    // 按钮标题
    property string customText
    // 按钮是否可用
    property bool bEnabled:true

    // 按钮背景样式
    style: ButtonStyle {
            background: Rectangle {
                color: 'transparent'
            }
    }
    // 按钮图标
    Image {
        source: bEnabled ? (parent.pressed ? parent.pressedIconSource : parent.customIconSource) : parent.disabledIconSource
        fillMode: Image.PreserveAspectFit // For not stretching image (optional)
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 6 // Leaving space between image and borders (optional)
        anchors.bottomMargin:20 // Leaving space for text in bottom
    }
    // 按钮标题
    Text {
        text: parent.customText
        color: bEnabled?'white':'grey'
        font.pixelSize: 10
        anchors.bottom: parent.bottom // Placing text in bottom
        anchors.margins: 4 // Leaving space between text and borders  (optional)
        anchors.horizontalCenter: parent.horizontalCenter // Centering text
        renderType: Text.NativeRendering // Rendering type (optional)
    }
}
