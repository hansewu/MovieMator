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

import QtQuick 2.1
import Qt.labs.controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    property ListModel listModel
    property int currentIndex
    property string currentText

    id: comboBox
    Layout.fillWidth: true
    border.width: 1
    color: "#525252"
    border.color: 'gray'
    width: 100
    height: 32

    function selectItemOfIndex(index) {
        if (listModel.count > 0) {
            listView.currentIndex = index
            currentIndex = index
            currentText = listModel.get(index).name
        }
    }

    Component.onCompleted: {
        selectItemOfIndex(0)
    }

    RowLayout {
        anchors.fill: parent
        Layout.fillWidth: true

        Text {
            id: labelText
            anchors.verticalCenter: parent.verticalCenter
            text: currentText
            anchors.left: parent.left
            anchors.leftMargin: 5
            color: "#FFFFFF"
        }

        //下拉箭头
        Image {
            anchors.right: parent.right
            anchors.leftMargin: 5
            width: 32
            height: 32
            fillMode: Image.PreserveAspectCrop
            source: "qrc:///icons/filters/icon/down-arrow.png"
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
//        onEntered: {
//            comboBox.border.color = "blue"
//        }
//        onExited: {
//            comboBox.border.color = "lightblue"
//        }
        onClicked: {
            popup.open()
        }
    }

    Popup {
        id: popup
        x: labelText.x
        y: comboBox.height
        width: comboBox.width
        height: comboBox.height * (listModel.count <= 10 ? listModel.count : 10)
        closePolicy: Popup.OnEscape | Popup.OnPressOutside    //单击弹窗外关闭弹窗
        background: Rectangle {
            color: "white"
            border.color: "#bbbbbb"
            border.width: 1
            clip: true//防止滚动的时候内容超出边界
            MyListView {//1、中订制了滚动条的ListView
                id: listView
                anchors.fill: parent
                anchors.margins: 2
                clip: true//防止滚动的时候内容超出边界
                model: listModel
                delegate: Rectangle {//基于1、中订制的ListView实现一个简单的代理
                    id: listViewDelegate
                    width: parent.width
                    height: comboBox.height
                    color: "white"
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        text: name
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            listView.scrollBarVisible = true//鼠标进入代理中显示滚动条
                            listViewDelegate.color = "lightblue"
                        }
                        onExited: {
                            listView.scrollBarVisible = false//鼠标离开代理后滚动条消失
                            listViewDelegate.color = "white"
                        }
                        onClicked: {
                            selectItemOfIndex(index)
                            popup.close()//关闭弹窗
                        }
                    }
                }
            }
        }
    }
}
