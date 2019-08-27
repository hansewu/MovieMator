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
        sliderPeak.value = filter.getDouble('0') * sliderPeak.maximumValue
        sliderAttack.value = filter.getDouble('1')
        sliderRelease.value = filter.get('2')
        sliderThreshold.value = filter.getDouble('3')
        sliderRatio.value = filter.getDouble('4')
        sliderRadius.value = filter.getDouble('5')
        sliderGain.value = filter.getDouble('6')
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
            text: qsTr('RMS')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('The balance between the RMS and peak envelope followers. RMS is generally better for subtle, musical compression and peak is better for heavier, fast compression and percussion.')}
        }
        SliderSpinner {
            id: sliderPeak
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            spinnerWidth: 80
            label: qsTr('Peak')
            suffix: ' %'
            value: filter.getDouble('0') * maximumValue
            onValueChanged: {
                filter.set('0', value / maximumValue)
            }
        }
        UndoButton {
            onClicked: sliderPeak.value = sliderPeak.minimumValue
        }

        Label {
            text: qsTr('Attack')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: sliderAttack
            minimumValue: 2
            maximumValue: 400
            suffix: ' ms'
            spinnerWidth: 80
            value: filter.getDouble('1')
            onValueChanged: {
                filter.set('1', value)
            }
        }
        UndoButton {
            onClicked: sliderAttack.value = 100
        }

        Label {
            text: qsTr('Release')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: sliderRelease
            minimumValue: 2
            maximumValue: 800
            spinnerWidth: 80
            suffix: ' ms'
            value: filter.getDouble('2')
            onValueChanged: {
                filter.set('2', value)
            }
        }
        UndoButton {
            onClicked: sliderRelease.value = 400
        }

        Label {
            text: qsTr('Threshold')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('The point at which the compressor will start to kick in.')}
        }
        SliderSpinner {
            id: sliderThreshold
            minimumValue: -30
            maximumValue: 0
            decimals: 1
            spinnerWidth: 80
            suffix: ' dB'
            value: filter.getDouble('3')
            onValueChanged: {
                filter.set('3', value)
            }
        }
        UndoButton {
            onClicked: sliderThreshold.value = 0
        }

        Label {
            text: qsTr('Ratio')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('The gain reduction ratio used when the signal level exceeds the threshold.')}
        }
        SliderSpinner {
            id: sliderRatio
            minimumValue: 1
            maximumValue: 20
            prefix: ' 1:'
            spinnerWidth: 80
            value: filter.getDouble('4')
            onValueChanged: {
                filter.set('4', value)
            }
        }
        UndoButton {
            onClicked: sliderRatio.value = 1
        }

        Label {
            text: qsTr('Knee radius')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('The distance from the threshold where the knee curve starts.')}
        }
        SliderSpinner {
            id: sliderRadius
            minimumValue: 1
            maximumValue: 10
            decimals: 1
            suffix: ' dB'
            spinnerWidth: 80
            value: filter.getDouble('5')
            onValueChanged: {
                filter.set('5', value)
            }
        }

        UndoButton {
            onClicked: sliderRadius.value = 3.25
        }
        Label {
            text: qsTr('Attenuation')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('The gain of the output signal. Used to correct for excessive amplitude caused by the extra dynamic range.')}
        }
        SliderSpinner {
            id: sliderGain
            minimumValue: -24
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
            onClicked: sliderGain.value = 0
        }

        Item {
            Layout.fillHeight: true;
        }
    }
}
