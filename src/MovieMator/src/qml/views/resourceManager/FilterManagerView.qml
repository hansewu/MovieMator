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

import QtQuick 2.1
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0


Rectangle {
    width: 500
    height: 400
    color: "white"

        RowLayout {
                id: toolBar
                height: 30
                width: parent.width

                ExclusiveGroup { id: typeGroup }

                ToolButton {
                    id: favButton
                    implicitWidth: 28
                    implicitHeight: 24
                    checkable: true
                    checked: true
                    iconName: 'bookmarks'
                    iconSource: 'qrc:///icons/oxygen/32x32/places/bookmarks.png'
                    tooltip: qsTr('Show favorite filters')
                    exclusiveGroup: typeGroup
                    onCheckedChanged: {
                        if (checked) {
                            if((typeof metadatamodel == 'undefined')||(metadatamodel == null)){
                                throw new Error("metadatamodel is undefined")
                            }
                            metadatamodel.metadataFilterType = MovieMator.MetadataModel.FavoritesFilter
                        }
                    }
                }
                ToolButton {
                    id: vidButton
                    implicitWidth: 28
                    implicitHeight: 24
                    checkable: true
                    iconName: 'video-television'
                    iconSource: 'qrc:///icons/oxygen/32x32/devices/video-television.png'
                    tooltip: qsTr('Show video filters')
                    exclusiveGroup: typeGroup
                    onCheckedChanged: {
                        if (checked) {
                            metadatamodel.metadataFilterType = MovieMator.MetadataModel.VideoFilter
                        }
                    }
                }
                ToolButton {
                    id: audButton
                    implicitWidth: 28
                    implicitHeight: 24
                    checkable: true
                    iconName: 'speaker'
                    iconSource: 'qrc:///icons/oxygen/32x32/actions/speaker.png'
                    tooltip: qsTr('Show audio filters')
                    exclusiveGroup: typeGroup
                    onCheckedChanged: {
                        if (checked) {
                            metadatamodel.metadataFilterType = MovieMator.MetadataModel.AudioFilter
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
                ToolButton {
                    id: closeButton
                    implicitWidth: 28
                    implicitHeight: 24
                    iconName: 'window-close'
                    iconSource: 'qrc:///icons/oxygen/32x32/actions/window-close.png'
                    tooltip: qsTr('Close menu')
                    onClicked: filterWindow.close()
                }
    }

    GridView {
        anchors.fill: parent
        anchors.margins: 20
        clip: true
        model: metadatamodel
        cellWidth: 100
        cellHeight: 100
        currentIndex: -1
        delegate: numberDelegate

        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        focus: true
    }

    Component {
        id: numberDelegate
        Rectangle {
            id: container
            width: 80
            height: 80
            color: "transparent"

            Column {

                anchors.fill: parent

                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 60
                    height: 60
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: thumbnail

                }

                Text {
                    width:80
                    height:20
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 10
                    text: name
                }

            }


            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton

                onClicked: {
                    container.GridView.view.currentIndex = index
                    container.forceActiveFocus()
                    filterWidget.previewFilter(metadatamodel.get(index))
                }
            }

            MouseArea {
                anchors.fill: parent
                enabled: true
                acceptedButtons: Qt.RightButton
                onClicked: {

                    menu.popup()
                }
            }

            Menu {
                id: menu
                MenuItem {
                    text: qsTr('Add to timeline')
                    onTriggered: {
                        filterWidget.addFilterToTimeline(metadatamodel.get(index))
                    }
                }
                MenuItem {
                    text: qsTr('Preview')
                    onTriggered: {}
                }
            }
        }
    }

}

