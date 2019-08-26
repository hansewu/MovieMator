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
 
import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

Item {
    id: comboBox
    width: 100
    height: 18
    z: 3

    signal catChanged(var index)

    property ListModel model

    function setModel(md){
        model = md
    }

    Button {
        id: comboButton
        anchors.fill: parent
        checkable: true
        text : model.get(0).typename
        style: ButtonStyle {
            background: Rectangle {
                color: '#323232'
                smooth: true
                radius: 1
            }
            label: Text {
                renderType: Text.NativeRendering
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                // font.family: "Courier"
                // font.capitalization: Font.SmallCaps
                font.pixelSize: 12
                color: "white"
                text: comboButton.text
            }
        }
        onVisibleChanged: {
            if(!visible)
                checked = false
        }
        onCheckedChanged: listView.visible = checked
    }
    ListView {
        id: listView
        height :300
        width: comboButton.width
        z: 3
        // 保证在最顶，控件可见
        anchors.top: comboButton.bottom
        visible: false
        model: comboBox.model

        boundsBehavior: Flickable.StopAtBounds

        delegate: Rectangle{
            id: delegateItem
            width: comboButton.width
            height: comboButton.height
            radius: 1
            color: '#525252'
            Text {
                renderType: Text.NativeRendering
                anchors.verticalCenter: parent.verticalCenter
//                anchors.horizontalCenter: parent.horizontalCenter
                anchors.left: parent.left
                anchors.leftMargin: 3
                font.pixelSize: 12
                color: "white"
//                elide: Text.ElideMiddle
                text: typename
            }
            MouseArea {
                hoverEnabled:true
                anchors.fill: parent;
                onClicked: {
                    listView.currentIndex = index
                    comboButton.text = comboBox.model.get(listView.currentIndex).typename
                    comboButton.checked = false
                    catChanged(index)
                }
                onEntered: {
                    parent.color = '#C0482C'
                    timer.stop()
                }
                onExited: {
                    parent.color = '#525252'
                    timer.restart()
                }
            }
        }

    }

    Timer {
        id : timer
        interval: 3000
        repeat: false
        onTriggered: 
        {
            listView.visible = false
        }
    }
}
