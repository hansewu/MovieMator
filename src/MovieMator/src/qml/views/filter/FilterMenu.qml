/*
 * Copyright (c) 2014-2015 Meltytech, LLC
 * Author: Brian Matherly <code@brianmatherly.com>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
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
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import 'FilterMenu.js' as Logic
import com.moviemator.qml 1.0 as MovieMator

Window {
    id: filterWindow
    visible: false

    property bool hasFocus: activeFocusItem != null

    signal filterSelected(int index)

    function popup(triggerItem, pos) {
        var menuRect = Logic.calcMenuRect(triggerItem, toolBar.height + 2, pos)
        filterWindow.x = menuRect.x
        filterWindow.y = menuRect.y
        filterWindow.height = menuRect.height

        // 时间线工具栏添加滤镜按钮和滤镜界面的添加按钮会互相影响，
        // 会导致激活的滤镜类型与列表对应不上；
        // 判断是哪种滤镜，把对应按钮的选中状态和图片切换一下。
        if((typeof metadatamodel == 'undefined')||(typeof metadatamodel.metadataFilterType == 'undefined')){
            throw new Error("metadata is abnormal"+metadatamodel)
        }

        if (metadatamodel.metadataFilterType == MovieMator.MetadataModel.FavoritesFilter) {
            favButton.checked = true
            favButton.iconSource = 'qrc:///icons/light/32x32/bookmarks-highlight.png'
            vidButton.iconSource = 'qrc:///icons/light/32x32/video-television.png'
            audButton.iconSource = 'qrc:///icons/light/32x32/speaker.png'
        }        
        else if(metadatamodel.metadataFilterType == MovieMator.MetadataModel.VideoFilter) {
            vidButton.checked = true
            vidButton.iconSource = 'qrc:///icons/light/32x32/video-television-highlight.png'
            favButton.iconSource = 'qrc:///icons/light/32x32/bookmarks.png'
            audButton.iconSource = 'qrc:///icons/light/32x32/speaker.png'
        }
        else if(metadatamodel.metadataFilterType == MovieMator.MetadataModel.AudioFilter) {
            audButton.checked = true
            audButton.iconSource = 'qrc:///icons/light/32x32/speaker-highlight.png'
            vidButton.iconSource = 'qrc:///icons/light/32x32/video-television.png'
            favButton.iconSource = 'qrc:///icons/light/32x32/bookmarks.png'
        }
        // End

        filterWindow.show()
        filterWindow.requestActivate()
    }

    color: 'transparent'//#323232'//Qt.darker(activePalette.window, 1.5) // Border color
    flags: Qt.Sheet
    width: 250
    height: 200
    onHasFocusChanged: if (!hasFocus) filterWindow.close()

    SystemPalette {
        id: activePalette

    }

    Rectangle {
        id: menuColorRect


        anchors.fill: parent
        anchors.margins: 1
        color: '#525252'//activePalette.base
        radius: 5

        Column {
            anchors.fill: parent
//            anchors.left: parent.left
//            anchors.right: parent.right
           anchors.margins: 6
//            anchors.leftMargin: 10
//            anchors.rightMargin: 1
//            anchors.topMargin: 10
//            anchors.bottomMargin: 10
           spacing: 2


            Component {
                id: sectionDelegate

                Item {
                    height: sectionText.implicitHeight + 4
                    width: parent ? parent.width : undefined
                    Rectangle {
                        anchors.fill: parent
                        color: activePalette.alternateBase
                    }
                    Label {
                        id: sectionText
                        anchors.fill: parent
                        anchors.topMargin: 2
                        anchors.leftMargin: 4
                        text: section
                        color: activePalette.windowText
                        font.bold: true
                    }
                }
            }



            ScrollView {
                id: scrollView
                width: parent.width
                height: parent.height - toolBar.height - 6

                ListView {
                    id: menuListView


                    function itemSelected(index) {
                            filterWindow.close()
                            filterSelected(index)
                    }

                    anchors.fill: parent
                    model: filterWindow.visible ? metadatamodel : 0 // 消除 metadatamodel未定义的警告
                    delegate: FilterMenuDelegate{}
                    section.property: vidButton.checked ? "filterTypeDisplay" : ""
                    section.delegate: sectionDelegate
                    boundsBehavior: Flickable.StopAtBounds
                    snapMode: ListView.SnapToItem
                    currentIndex: -1
                    focus: true
                }

                style: ScrollViewStyle {
                        transientScrollBars: false
                      //  scrollToClickedPosition:true
                        handle: Item {
                            implicitWidth: 14
                            implicitHeight: 14
                            Rectangle {
                                color: "#787878"
                                anchors.fill: parent
                                anchors.margins: 3
                                radius: 4
                            }
                        }
                        scrollBarBackground: Item {
                            implicitWidth: 14
                            implicitHeight: 14
                        }
                        decrementControl: Rectangle {
                                    implicitWidth: 0
                                    implicitHeight: 0
                        }
                        incrementControl: Rectangle {
                                    implicitWidth: 0
                                    implicitHeight: 0
                        }

                    }

            }



            Rectangle {
                id: separatorBar
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: 1
                color: Qt.darker(activePalette.window, 1.5)
            }

            RowLayout {
                id: toolBar
                height: 30
                width: parent.width

                ExclusiveGroup { id: typeGroup }

                ToolButton {
                    id: favButton
                    implicitWidth: 28
                    implicitHeight: 28
                    checkable: true
                    checked: true
                  //  iconName: 'bookmarks'
                    iconSource: 'qrc:///icons/light/32x32/bookmarks-highlight.png'
                    tooltip: qsTr('Show favorite filters')

                    exclusiveGroup: typeGroup
                    onCheckedChanged: {
                        if (checked) {
                            metadatamodel.metadataFilterType = MovieMator.MetadataModel.FavoritesFilter
                            favButton.iconSource = 'qrc:///icons/light/32x32/bookmarks-highlight.png'

                            vidButton.iconSource = 'qrc:///icons/light/32x32/video-television.png'
                            audButton.iconSource = 'qrc:///icons/light/32x32/speaker.png'
                        }

                    }
                }
                ToolButton {
                    id: vidButton
                    implicitWidth: 28
                    implicitHeight: 28
                    checkable: true
              //      iconName: 'video-television'
                    iconSource: 'qrc:///icons/light/32x32/video-television.png'
                    tooltip: qsTr('Show video filters')
                    exclusiveGroup: typeGroup
                    onCheckedChanged: {
                        if (checked) {
                            metadatamodel.metadataFilterType = MovieMator.MetadataModel.VideoFilter
                            vidButton.iconSource = 'qrc:///icons/light/32x32/video-television-highlight.png'
                            favButton.iconSource = 'qrc:///icons/light/32x32/bookmarks.png'
                            audButton.iconSource = 'qrc:///icons/light/32x32/speaker.png' 
                        }
                    }
                }
                ToolButton {
                    id: audButton
                    implicitWidth: 28
                    implicitHeight: 28
                    checkable: true
                 //   iconName: 'speaker'
                    iconSource: 'qrc:///icons/light/32x32/speaker.png'
                    tooltip: qsTr('Show audio filters')
                    exclusiveGroup: typeGroup
                    onCheckedChanged: {
                        if (checked) {
                            metadatamodel.metadataFilterType = MovieMator.MetadataModel.AudioFilter
                            audButton.iconSource = 'qrc:///icons/light/32x32/speaker-highlight.png'

                            vidButton.iconSource = 'qrc:///icons/light/32x32/video-television.png'
                            favButton.iconSource = 'qrc:///icons/light/32x32/bookmarks.png'
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
                ToolButton {
                    id: closeButton
                    implicitWidth: 28
                    implicitHeight: 28
                 //   iconName: 'window-close'
                    iconSource: 'qrc:///icons/light/32x32/window-close.png'
                    tooltip: qsTr('Close menu')
                    onClicked: filterWindow.close()
                }
            }
        }
    }
}
