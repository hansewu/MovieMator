/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
 * Author: fuyunhuaxin <2446010587@qq.com>
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.0

Item {
    width: 300
    height: 330
    Component.onCompleted: {
        keyFrame.initFilter(layoutRoot)
    }

    function setControls() {
        rotationSlider.value = filter.getDouble('transition.fix_rotate_x')
        scaleSlider.value = 100 / filter.getDouble('transition.scale_x')
        xOffsetSlider.value = filter.getDouble('transition.ox') * -1
        yOffsetSlider.value = filter.getDouble('transition.oy') * -1

    }

    GridLayout {
        id: layoutRoot
        anchors.fill: parent
        anchors.margins: 20
        columns: 3
        rowSpacing:15

        YFKeyFrame{
            id: keyFrame
            Layout.columnSpan:3
            onSyncUIDataToProject:{
                keyFrame.syncDataToProject(layoutRoot)
            }
            onRefreshUI:{
                keyFrame.updateParamsUI(layoutRoot)
            }
        }

        Label {
            text: qsTr('Preset') + "        "
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
        }
        Preset {
            id: preset
            parameters: ['transition.fix_rotate_x', 'transition.scale_x', 'transition.ox', 'transition.oy']
            Layout.columnSpan: 2
            onPresetSelected: setControls()
        }

        SeparatorLine {
            Layout.columnSpan: 3
            Layout.minimumWidth: parent.width
            Layout.maximumWidth: parent.width
        }

        Label {
            text: qsTr('Rotation')
            color: '#ffffff'
        }
        SliderSpinner {
            objectName: 'rotationSlider'
            id: rotationSlider
            minimumValue: 0
            maximumValue: 360
            decimals: 1
            spinnerWidth: 80
            suffix: qsTr(' °')//qsTr(' degree')
            onValueChanged:{
               keyFrame.controlValueChanged(rotationSlider) 
            }
        }
        UndoButton {
            onClicked: rotationSlider.value = 0
        }

        Label {
            text: qsTr('Scale')
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
        }
        SliderSpinner {
            objectName: 'scaleSlider'
            id: scaleSlider
            minimumValue: 0.1
            maximumValue: 200
            decimals: 1
            spinnerWidth: 80
            suffix: ' %'
            onValueChanged: {
                keyFrame.controlValueChanged(scaleSlider) 
            }
        }
        UndoButton {
            onClicked: scaleSlider.value = 100
        }

        SeparatorLine {
            Layout.columnSpan: 3
            Layout.minimumWidth: parent.width
            Layout.maximumWidth: parent.width
        }

        Label {
            text: qsTr('X offset')
            color: '#ffffff'
        }
        SliderSpinner {
            objectName: 'xOffsetSlider'
            id: xOffsetSlider
            minimumValue: -1000
            maximumValue: 1000
            spinnerWidth: 80
            onValueChanged:{
                keyFrame.controlValueChanged(xOffsetSlider) 
            }
        }
        UndoButton {
            onClicked: xOffsetSlider.value = 0
        }

        Label {
            text: qsTr('Y offset')
            color: '#ffffff'
        }
        SliderSpinner {
            objectName: 'yOffsetSlider'
            id: yOffsetSlider
            minimumValue: -1000
            maximumValue: 1000
            spinnerWidth: 80
            onValueChanged:{
                keyFrame.controlValueChanged(yOffsetSlider) 
            }
        }
        UndoButton {
            onClicked: yOffsetSlider.value = 0
        }

        SeparatorLine {
            Layout.fillWidth: true 
            Layout.columnSpan: 3
        }

        Button {
            id: rotateButton
            //text: qsTr('90°')
            tooltip: qsTr('Rotate 90°')
            //iconSource: "qrc:///icons/light/32x32/rotate-90.png"
            Layout.alignment: Qt.AlignRight
            onClicked: {
                rotationSlider.value = 80
                rotationSlider.value = 90
            }
            implicitWidth: 56
            implicitHeight: 56
            style: ButtonStyle {
                background: Rectangle {
                    color: 'transparent'
                }  
            }
            Image {
                fillMode: Image.PreserveAspectCrop
                anchors.fill: parent
                source: rotateButton.pressed? "qrc:///icons/light/32x32/rotate-90-a.png" : "qrc:///icons/light/32x32/rotate-90.png"
            }
        }

        Button {
            id: rotateButton2
            //text: qsTr('180°')
            tooltip: qsTr('Rotate 180°')
            //iconSource: "qrc:///icons/light/32x32/rotate-180.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                rotationSlider.value = 170
                rotationSlider.value = 180
            }
            implicitWidth: 56
            implicitHeight: 56
            style: ButtonStyle {
                background: Rectangle {
                    color: 'transparent'
                    //implicitWidth: 50
                    //implicitHeight: 10
                    //border.width: 1
                    //border.color:"#888"
                    //radius:3
                    /*gradient:Gradient {
                        GradientStop {position:0; color: "#ccc"}
                        GradientStop {position:1; color: "#aaa"}
                    }
                    */
                }
                
            }
            Image {
                fillMode: Image.PreserveAspectCrop
                anchors.fill: parent
                source: rotateButton2.pressed? "qrc:///icons/light/32x32/rotate-180-a.png" : "qrc:///icons/light/32x32/rotate-180.png"
            }
        
        }

        Button {
            id: rotateButton3
            //text: qsTr('270°')
            tooltip: qsTr('Rotate 270°')
            //iconSource: "qrc:///icons/light/32x32/rotate-270.png"
            Layout.alignment: Qt.AlignRight
            onClicked: {
                rotationSlider.value = 260
                rotationSlider.value = 270
            }
            implicitWidth: 56
            implicitHeight: 56
            style: ButtonStyle {
                background: Rectangle {
                    color: 'transparent'
                }  
            }
            Image {
                fillMode: Image.PreserveAspectCrop
                anchors.fill: parent
                source: rotateButton3.pressed? "qrc:///icons/light/32x32/rotate-270-a.png" : "qrc:///icons/light/32x32/rotate-270.png"
            }
        }

        Item {
            Layout.fillHeight: true;
        }
    }
}
