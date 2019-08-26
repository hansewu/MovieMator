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

// 关键帧指示图标
Rectangle{
    property int frameNumber
    //width: 12
    //height: 12
    color:'red'

    // 关键帧指示图标的左键操作
    // 时间线播放游标定位到这个关键帧位置
    MouseArea {
        id:keyFrameMouseArea
        anchors.fill: parent
        enabled: true
        hoverEnabled: true
        cursorShape: Qt.ArrowCursor
        acceptedButtons: Qt.LeftButton
        onClicked: {
            console.assert(timeline);
            if(timeline)
                timeline.seekToKeyFrame(frameNumber)
        }
    }

    // 关键帧指示图标缩放动画
    SequentialAnimation on scale {
        loops: 1//Animation.Infinite
        running: keyFrameMouseArea.containsMouse
        NumberAnimation {
            from: 1.0
            to: 1.2
            duration: 250
            easing.type: Easing.InOutQuad
        }
//        NumberAnimation {
//            from: 1.5
//            to: 1.0
//            duration: 250
//            easing.type: Easing.InOutQuad
//        }
    }
    // 关键帧指示图标缩放动画
    SequentialAnimation on scale {
        loops: 1//Animation.Infinite
        running: !keyFrameMouseArea.containsMouse
        NumberAnimation {
            from: 1.2
            to: 1.0
            duration: 250
            easing.type: Easing.InOutQuad
        }
    }
}
