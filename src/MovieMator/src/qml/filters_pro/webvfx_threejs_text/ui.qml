/*
 * Copyright (c) 2014-2016 Meltytech, LLC
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
import QtQuick.Dialogs 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Item {
    width: 300
    height: 250
    Component.onCompleted: {
        if (filter.isNew) {
            filter.set('resource', filter.resourcePath + 'threejs_text.html')
            // Set default parameter values
            textField.text = '3D Text'
            filter.set('text', textField.text)
            filter.set('color', '#CCCCCC')
            filter.set('x_rotation', 0.5)
            filter.set('font', 'droid sans')
            filter.set('weight', 'bold')
            filter.set('bevel', 0)
            filter.set('depth', 20)
            filter.set('size', 70)
            filter.set('horizontal', 0.5)
            filter.set('vertical', 0.5)
            filter.savePreset(preset.parameters)

            setControls();
        }
        filter.setInAndOut(filter.producerIn, filter.producerOut)
    }

    function setControls() {
        colorSwatch.value = filter.get('color')
        tiltSlider.value = filter.getDouble('x_rotation') * tiltSlider.maximumValue
        fontCombo.currentIndex = fontCombo.valueToIndex(filter.get('font'))
        boldCheckBox.checked = filter.get('weight') === 'bold'
        bevelCheckBox.checked = filter.getDouble('bevel')
        depthSlider.value = filter.getDouble('depth') * 2
        sizeSlider.value = filter.getDouble('size')
        horizontalSlider.value = filter.getDouble('horizontal') * horizontalSlider.maximumValue
        verticalSlider.value = filter.getDouble('vertical') * verticalSlider.maximumValue
    }

    GridLayout {
        columns: 5
        anchors.fill: parent
        anchors.margins: 8

        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        Preset {
            id: preset
            parameters: ['color', 'x_rotation', 'font', 'weight', 'bevel',
                'depth', 'size', 'horizontal', 'vertical']
            Layout.columnSpan: 4
            onPresetSelected: setControls()
        }

        Label {
            text: qsTr('Text')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        TextField {
            id: textField
            Layout.columnSpan: 4
            text: filter.get('text')
            Layout.minimumWidth: sizeSlider.width
            Layout.maximumWidth: sizeSlider.width
            onTextChanged: {
                filter.set('text', text)
            }
        }

        Label {
            text: qsTr('Font')
            Layout.row: 2
            Layout.column: 0            
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
//        ComboBox {
//            id: fontCombo
//            Layout.row: 2
//            Layout.column: 1
//            model: ['Droid Sans', 'Droid Serif', 'Gentilis', 'Helvetiker', 'Optimer']
//            property var values: ['droid sans', 'droid serif', 'gentilis', 'helvetiker', 'optimer']
//            function valueToIndex() {
//                var w = filter.get('font')
//                for (var i = 0; i < values.length; ++i)
//                    if (values[i] === w) break;
//                if (i === values.length) i = 0;
//                return i;
//            }
//            currentIndex: valueToIndex()
//            onActivated: filter.set('font', values[index])
//        }
        MyComboBox {
            id: fontCombo
            Layout.row: 2
            Layout.column: 1
            listModel: ListModel {
                ListElement {name: 'Droid Sans'}
                ListElement {name: 'Droid Serif'}
                ListElement {name: 'Gentilis'}
                ListElement {name: 'Helvetiker'}
                ListElement {name: 'Optimer'}
            }
            property var values: ['droid sans', 'droid serif', 'gentilis', 'helvetiker', 'optimer']
            function valueToIndex() {
                var w = filter.get('font')
                for (var i = 0; i < values.length; ++i)
                    if (values[i] === w) break;
                if (i === values.length) i = 0;
                return i;
            }
            onCurrentIndexChanged: {
                filter.set('font', values[currentIndex])
            }
        }
        CheckBox {
            id: boldCheckBox
            Layout.row: 2
            Layout.column: 2            
            checked: filter.get('weight') === 'bold'
          //  text: qsTr('Bold')
            onCheckedChanged: filter.set('weight', checked? 'bold' : 'normal')
            style: CheckBoxStyle {
                        label: Text {
                            color: "white"
                            text: qsTr('Bold')
                        }
            }
        }
        CheckBox {
            id: bevelCheckBox
//            Layout.columnSpan: 2
            Layout.row: 2
            Layout.column: 3
            checked: filter.getDouble('bevel')
          //  text: qsTr('Beveled')
            onCheckedChanged: filter.set('bevel', checked)
            style: CheckBoxStyle {
                        label: Text {
                            color: "white"
                            text: qsTr('Beveled')
                        }
            }
        }

        Label {
            text: qsTr('Color')
            Layout.row: 3
            Layout.column: 0            
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        ColorPicker {
            id: colorSwatch
//            Layout.columnSpan: 4
            Layout.row: 3
            Layout.column: 1
            value: filter.get('color')
            property bool isReady: false
            Component.onCompleted: isReady = true
            onValueChanged: {
                if (isReady) {
                    filter.set('color', '' + value)
                    filter.set("disable", 0);
                }
            }
            onPickStarted: {
                filter.set("disable", 1);
            }
        }

        Label {
            text: qsTr('Size')
            Layout.row: 4
            Layout.column: 0            
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: sizeSlider
//            Layout.columnSpan: 3
            Layout.row: 4
            Layout.column: 1
            minimumValue: 0
            maximumValue: 200
            value: filter.getDouble('size')
            onValueChanged: filter.set('size', value)
        }
        UndoButton {
            Layout.row: 4
            Layout.column: 2            
            onClicked: sizeSlider.value = 70
        }

        Label {
            text: qsTr('Depth')
            Layout.row: 5
            Layout.column: 0            
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: depthSlider
//            Layout.columnSpan: 3
            Layout.row: 5
            Layout.column: 1
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('depth') * 2
            onValueChanged: filter.set('depth', value * 0.5)
        }
        UndoButton {
            Layout.row: 5
            Layout.column: 2            
            onClicked: depthSlider.value = 40
        }

        Label {
            text: qsTr('Tilt')
            Layout.row: 6
            Layout.column: 0            
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: tiltSlider
//            Layout.columnSpan: 3
            Layout.row: 6
            Layout.column: 1
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('x_rotation') * maximumValue
            onValueChanged: filter.set('x_rotation', value / maximumValue)
        }
        UndoButton {
            Layout.row: 6
            Layout.column: 2            
            onClicked: tiltSlider.value = 50
        }

        Label {
            text: qsTr('Horizontal')
            Layout.row: 7
            Layout.column: 0            
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: horizontalSlider
//            Layout.columnSpan: 3
            Layout.row: 7
            Layout.column: 1
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('horizontal') * maximumValue
            onValueChanged: filter.set('horizontal', value / maximumValue)
        }
        UndoButton {
            Layout.row: 7
            Layout.column: 2            
            onClicked: horizontalSlider.value = 50
        }

        Label {
            text: qsTr('Vertical')
            Layout.row: 8
            Layout.column: 0            
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: verticalSlider
//            Layout.columnSpan: 3
            Layout.row: 8
            Layout.column: 1
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('vertical') * maximumValue
            onValueChanged: filter.set('vertical', value / maximumValue)
        }
        UndoButton {
            Layout.row: 8
            Layout.column: 2            
            onClicked: verticalSlider.value = 50
        }

        // Item {
        //     Layout.fillHeight: true
        // }
    }
}
