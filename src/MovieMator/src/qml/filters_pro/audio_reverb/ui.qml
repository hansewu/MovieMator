/*
 * Copyright (c) 2015-2016 Meltytech, LLC
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
        sliderRoom.value = filter.getDouble('0')
        sliderTime.value = filter.getDouble('1')
        sliderDamp.value = filter.get('2') * sliderDamp.maximumValue
        sliderInput.value = filter.getDouble('3') * sliderInput.maximumValue
        sliderDry.value = filter.getDouble('4')
        sliderReflection.value = filter.getDouble('5')
        sliderTail.value = filter.getDouble('6')
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
            parameters: ['0', '1', '2', '3', '4', '5', '6']
            Layout.columnSpan: 2
            onPresetSelected: setControls()
        }

        Label {
            text: qsTr('Room size')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('The size of the room, in meters. Excessivly large, and excessively small values will make it sound a bit unrealistic.Values of around 30 sound good.')}
        }
        SliderSpinner {
            id: sliderRoom
            minimumValue: 1
            maximumValue: 300
            suffix: ' m'
            spinnerWidth: 80
            value: filter.getDouble('0')
            onValueChanged: {
                filter.set('0', value)
            }
        }
        UndoButton {
            onClicked: sliderRoom.value = 30
        }

        Label {
            text: qsTr('Reverb time')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: sliderTime
            minimumValue: .1
            maximumValue: 30
            decimals: 1
            suffix: ' s'
            spinnerWidth: 80
            value: filter.getDouble('1')
            onValueChanged: {
                filter.set('1', value)
            }
        }
        UndoButton {
            onClicked: sliderTime.value = 7.5
        }

        Label {
            text: qsTr('Damping')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('This controls the high frequency damping (a lowpass filter), values near 1 will make it sound very bright, values near 0 will make it sound very dark.')}
        }
        SliderSpinner {
            id: sliderDamp
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            spinnerWidth: 80
            suffix: ' %'
            value: filter.getDouble('2') * maximumValue
            onValueChanged: {
                filter.set('2', value / maximumValue)
            }
        }
        UndoButton {
            onClicked: sliderDamp.value = 0.5 * sliderDamp.maximumValue
        }

        Label {
            text: qsTr('Input bandwidth')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('This is like a damping control for the input, it has a similar effect to the damping control, but is subtly different.')}
        }
        SliderSpinner {
            id: sliderInput
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            spinnerWidth: 80
            suffix: ' %'
            value: filter.getDouble('3') * maximumValue
            onValueChanged: {
                filter.set('3', value / maximumValue)
            }
        }
        UndoButton {
            onClicked: sliderInput.value = 0.75 * sliderInput.maximumValue
        }

        Label {
            text: qsTr('Dry signal level')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('The amount of dry signal to be mixed with the reverberated signal.')}
        }
        SliderSpinner {
            id: sliderDry
            minimumValue: -70
            maximumValue: 0
            suffix: ' dB'
            decimals: 1
            spinnerWidth: 80
            value: filter.getDouble('4')
            onValueChanged: {
                filter.set('4', value)
            }
        }
        UndoButton {
            onClicked: sliderDry.value = 0
        }

        Label {
            text: qsTr('Early reflection level')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('The distance from the threshold where the knee curve starts.')}
        }
        SliderSpinner {
            id: sliderReflection
            minimumValue: -70
            maximumValue: 0
            suffix: ' dB'
            spinnerWidth: 80
            value: filter.getDouble('5')
            onValueChanged: {
                filter.set('5', value)
            }
        }

        UndoButton {
            onClicked: sliderReflection.value = -10
        }
        Label {
            text: qsTr('Tail level')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('The quantity of early reflections (scatter reflections directly from the source).')}
        }
        SliderSpinner {
            id: sliderTail
            minimumValue: -70
            maximumValue: 0
            decimals: 1
            suffix: ' dB'
            spinnerWidth: 80
            value: filter.getDouble('6')
            onValueChanged: {
                filter.set('6', value)
            }
        }
        UndoButton {
            onClicked: sliderTail.value = -17.5
        }

        Label {
            Layout.columnSpan: 3
            text: qsTr('About reverb')
            color: '#ffffff'
            font.underline: true
            MouseArea {
                anchors.fill: parent
                onClicked: Qt.openUrlExternally('https://wiki.audacityteam.org/wiki/GVerb')
            }
        }

        Item {
            Layout.fillHeight: true;
        }
    }
}
