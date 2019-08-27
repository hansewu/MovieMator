/*
 * Copyright (c) 2015 Meltytech, LLC
 * Author: Lauren Dennedy
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    Component.onCompleted: {

    }

    function setControls() {
        sliderCutoff.value = filter.getDouble('0')
        sliderStages.value = filter.get('1')
        sliderWetness.value = filter.getDouble('wetness') * sliderWetness.maximumValue

    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 8
        columns: 3


        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        Preset {
            id: preset
            parameters: ['0', '1', '2', 'wetness']
            Layout.columnSpan: 2
            onPresetSelected: setControls()
        }

        Label {
            text: qsTr('Cutoff frequency')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }

        SliderSpinner {
            id: sliderCutoff
            minimumValue: 5
            maximumValue: 21600
            suffix: ' Hz'
            spinnerWidth: 80
            value: filter.getDouble('0')
            onValueChanged: {
                filter.set('0', value)
            }
        }
        UndoButton {
                onClicked: sliderCutoff.value = 2637
            }

        Label {
            text: qsTr('Rolloff rate')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: sliderStages
            minimumValue: 1
            maximumValue: 10
            spinnerWidth: 80
            value: filter.get('1')
            onValueChanged: {
                filter.set('1', value)
            }
        }
        UndoButton {
                onClicked: sliderStages.value = 1
            }

        Label {
            text: qsTr('Dry')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: sliderWetness
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            spinnerWidth: 80
            label: qsTr('Wet')
            suffix: ' %'
            value: filter.getDouble('wetness') * maximumValue
            onValueChanged: {
                filter.set('wetness', value / maximumValue)
            }
        }
        UndoButton {
            onClicked: sliderWetness.value = sliderWetness.maximumValue
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
