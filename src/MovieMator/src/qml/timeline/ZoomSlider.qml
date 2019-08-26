/*
 * Copyright (c) 2013-2014 Meltytech, LLC
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
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.4

// 时间线工具栏上的缩放滑动条
Rectangle {
    // 滑动条的值
    property alias value: slider.value

    // 系统调色板
    SystemPalette { id: activePalette }

    // 颜色
    color: 'transparent'
    // 宽度
    width: 200
    // 高度
    height: 24

    // 滑动条
    Slider {
        id: slider
        // 水平方向滑动
        orientation: Qt.Horizontal
        // 样式
        style: SliderStyle {
                //滑动条
                groove: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 3
                    color: "black"
                    radius: 1
                }
                // 滑块
                handle: Rectangle {
                    anchors.centerIn: parent
                    color: control.pressed ? "lightgray" : "white"
                    border.color: "gray"
                    border.width: 1
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 8
                }
        }

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            leftMargin: 4
            rightMargin: 4
        }
        minimumValue: 0
        maximumValue: 1.75
        value: 1
        // 滑动条值改变时
        onValueChanged: {
            if (typeof multitrack.scaleFactor != 'undefined')
                multitrack.scaleFactor = Math.pow(value, 3) + 0.01
        }
        // ？？？
        onPressedChanged: {
            if (!pressed) {
                console.assert(tracksRepeater);
                if(!tracksRepeater) return;
                for (var i = 0; i < tracksRepeater.count; i++){
                    console.assert(tracksRepeater.itemAt(i));
                    if(tracksRepeater.itemAt(i))
                        tracksRepeater.itemAt(i).redrawWaveforms()
                }
            }
        }
    }
}
