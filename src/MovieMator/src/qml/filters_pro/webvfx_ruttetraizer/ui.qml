/*
 * Copyright (c) 2015-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
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

Item {
    width: 300
    height: 250
    Component.onCompleted: {
        if (filter.isNew) {
            filter.set('resource', filter.resourcePath + 'ruttetraizer.html')
            // Set default parameter values
            filter.set('opacity', 1.0)
            filter.set('thickness', 3.0)
            filter.set('density', 10)
            filter.set('depth', 100)
            filter.set('scale', 3.0)
            filter.set('rotation_x', 0)
            filter.set('rotation_y', 0)
            filter.savePreset(preset.parameters)

            setControls();
        }
        filter.setInAndOut(0, timeline.getCurrentClipParentLength()-1)
    }

    function setControls() {
        opacitySlider.value = filter.getDouble('opacity') * opacitySlider.maximumValue
        thicknessSlider.value = filter.getDouble('thickness')
        densitySlider.value = filter.getDouble('density')
        depthSlider.value = filter.getDouble('depth')
        scaleSlider.value = filter.getDouble('scale') / 3.0
        xRotationSlider.value = filter.getDouble('rotation_x') * xRotationSlider.maximumValue
        yRotationSlider.value = filter.getDouble('rotation_y') * yRotationSlider.maximumValue
    }

    GridLayout {
        id: layoutRoot
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

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
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        Preset {
            id: preset
            parameters: ['opacity', 'thickness', 'density', 'depth', 'scale',
                'rotation_x', 'rotation_y']
            Layout.columnSpan: 2
            onPresetSelected: setControls()
        }

        Label {
            text: qsTr('Brightness')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: opacitySlider
            objectName: 'opacitySlider'
            minimumValue: 1
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            value: filter.getDouble('opacity') * maximumValue
            onValueChanged: keyFrame.controlValueChanged(opacitySlider)
        }
        UndoButton {
            onClicked: opacitySlider.value = 100
        }

        Label {
            text: qsTr('Thickness')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: thicknessSlider
            objectName: 'thicknessSlider'
            minimumValue: 1
            maximumValue: 10
            decimals: 1
            value: filter.getDouble('thickness')
            onValueChanged: keyFrame.controlValueChanged(thicknessSlider)
        }
        UndoButton {
            onClicked: thicknessSlider.value = 3.0
        }

        Label {
            text: qsTr('Density')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: densitySlider
            objectName: 'densitySlider'
            minimumValue: 1
            maximumValue: 100
            spinnerWidth: 110
            suffix: ' pixels'
            value: filter.getDouble('density')
            onValueChanged: keyFrame.controlValueChanged(densitySlider)
        }
        UndoButton {
            onClicked: densitySlider.value = 10
        }

        Label {
            text: qsTr('Depth')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: depthSlider
            objectName: 'depthSlider'
            minimumValue: 1
            maximumValue: 500
            spinnerWidth: 110
            suffix: ' pixels'
            value: filter.getDouble('depth')
            onValueChanged: keyFrame.controlValueChanged(depthSlider)
        }
        UndoButton {
            onClicked: depthSlider.value = 100
        }

        Label {
            text: qsTr('Scale')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: scaleSlider
            objectName: 'scaleSlider'
            minimumValue: 0.01
            maximumValue: 3.0
            decimals: 2
            value: filter.getDouble('scale') / 3.0
            onValueChanged: keyFrame.controlValueChanged(scaleSlider)
        }
        UndoButton {
            onClicked: scaleSlider.value = 1.0
        }

        Label {
            text: qsTr('X Axis Rotation')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: xRotationSlider
            objectName: 'xRotationSlider'
            minimumValue: 0
            maximumValue: 360
            spinnerWidth: 110
            suffix: ' degree'
            value: filter.getDouble('rotation_x') * maximumValue
            onValueChanged: keyFrame.controlValueChanged(xRotationSlider)
        }
        UndoButton {
            onClicked: xRotationSlider.value = 0
        }

        Label {
            text: qsTr('Y Axis Rotation')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: yRotationSlider
            objectName: 'yRotationSlider'
            minimumValue: 0
            maximumValue: 360
            spinnerWidth: 110
            suffix: ' degree'
            value: filter.getDouble('rotation_y') * maximumValue
            onValueChanged: keyFrame.controlValueChanged(yRotationSlider)
        }
        UndoButton {
            onClicked: yRotationSlider.value = 0
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
