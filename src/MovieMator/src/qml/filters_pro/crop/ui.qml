/*
 * Copyright (c) 2014-2015 Meltytech, LLC
 * Author: Brian Matherly <pez4brian@yahoo.com>
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
 
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Item {
    property var defaultParameters: ['left', 'right', 'top', 'bottom', 'center', 'center_bias']
    width: 300
    height: 280
    
    function setEnabled() {
        if (centerCheckBox.checked) {
            biasslider.enabled = true
            biasundo.enabled = true
            topslider.enabled = false
            topundo.enabled = false
            bottomslider.enabled = false
            bottomundo.enabled = false
            leftslider.enabled = false
            leftundo.enabled = false
            rightslider.enabled = false
            rightundo.enabled = false
        } else {
            biasslider.enabled = false
            biasundo.enabled = false
            topslider.enabled = true
            topundo.enabled = true
            bottomslider.enabled = true
            bottomundo.enabled = true
            leftslider.enabled = true
            leftundo.enabled = true
            rightslider.enabled = true
            rightundo.enabled = true
        }
    }
    
    Component.onCompleted: {
        keyFrame.initFilter(layoutRoot)
        setEnabled()
    }

    GridLayout {
        id:layoutRoot
        columns: 3
        anchors.fill: parent
        anchors.margins: 20
        rowSpacing:12

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
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
        }
        Preset {
            Layout.columnSpan: 2
            parameters: defaultParameters
            onPresetSelected: {
                centerCheckBox.checked = filter.get('center') === '1'
                biasslider.value = +filter.get('center_bias')
                topslider.value = +filter.get('top')
                bottomslider.value = +filter.get('bottom')
                leftslider.value = +filter.get('left')
                rightslider.value = +filter.get('right')
                setEnabled()
            }
        }

        SeparatorLine {
            Layout.columnSpan: 3
            Layout.minimumWidth: parent.width
            Layout.maximumWidth: parent.width
        }

        Label {}

        CheckBox {
            id: centerCheckBox
            Layout.columnSpan: 1
            objectName:'centerCheckBox'
//            text: qsTr('Center')
            checked: false
            property bool isReady: false
            Component.onCompleted: isReady = true
            onClicked: {
                if (isReady) {
                    keyFrame.controlValueChanged(centerCheckBox)
                    setEnabled()
                }
            }

            style: CheckBoxStyle {
                        label: Text {
                            color: "white"
                            text: qsTr('Center ')
                        }
            }
        }

        UndoButton {
            onClicked: {
                centerCheckBox.checked = false
                filter.set('center', false)
            }
        }

        Label {
            text: qsTr('Center bias')
            Layout.alignment: Qt.AlignLeft
        }
        SliderSpinner {
            id: biasslider
            objectName:'biasslider'
            minimumValue: -Math.max(profile.width, profile.height) / 2
            maximumValue: Math.max(profile.width, profile.height) / 2
            suffix: ' px'
            // value: +filter.get('center_bias')
            onValueChanged: keyFrame.controlValueChanged(biasslider)
        }
        UndoButton {
            id: biasundo
            onClicked: biasslider.value = 0
        }

        SeparatorLine {
            Layout.columnSpan: 3
            Layout.minimumWidth: parent.width
            Layout.maximumWidth: parent.width
        }

        Label {
            text: qsTr('Top')
            Layout.alignment: Qt.AlignLeft
        }
        SliderSpinner {
            id: topslider
            objectName:'topslider'
            minimumValue: 0
            maximumValue: profile.height
            suffix: ' px'
            // value: +filter.get('top')
            onValueChanged: keyFrame.controlValueChanged(topslider)
        }
        UndoButton {
            id: topundo
            onClicked: topslider.value = 0
        }

        Label {
            text: qsTr('Bottom')
            Layout.alignment: Qt.AlignLeft
        }
        SliderSpinner {
            id: bottomslider
            objectName:'bottomslider'
            minimumValue: 0
            maximumValue: profile.height
            suffix: ' px'
            // value: +filter.get('bottom')
            onValueChanged: keyFrame.controlValueChanged(bottomslider)
        }
        UndoButton {
            id: bottomundo
            onClicked: bottomslider.value = 0
        }

        Label {
            text: qsTr('Left')
            Layout.alignment: Qt.AlignLeft
        }
        SliderSpinner {
            id: leftslider
            objectName: 'leftslider'
            minimumValue: 0
            maximumValue: profile.width
            suffix: ' px'
            // value: +filter.get('left')
            onValueChanged: keyFrame.controlValueChanged(leftslider)
        }
        UndoButton {
            id: leftundo
            onClicked: leftslider.value = 0
        }

        Label {
            text: qsTr('Right')
            Layout.alignment: Qt.AlignLeft
        }
        SliderSpinner {
            id: rightslider
            objectName: 'rightslider'
            minimumValue: 0
            maximumValue: profile.width
            suffix: ' px'
            // value: +filter.get('right')
            onValueChanged: keyFrame.controlValueChanged(rightslider)
        }
        UndoButton {
            id: rightundo
            onClicked: rightslider.value = 0
        }
        
        Item {
            Layout.fillHeight: true;
        }
    }
}
