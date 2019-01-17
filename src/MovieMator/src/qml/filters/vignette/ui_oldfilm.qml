

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Item {
    width: 300
    height: 250
    Component.onCompleted: {
        if (filter.isNew)
            filter.savePreset(preset.parameters)
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        Label{
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        Preset {
            id: preset
            Layout.columnSpan: 2
            parameters: ['radius', 'smooth', 'opacity', 'mode']
            onPresetSelected: {
                radiusSlider.value = filter.getDouble('radius') * 100
                smoothSlider.value = filter.getDouble('smooth') * 100
                opacitySlider.value = (1.0 - filter.getDouble('opacity')) * 100
                modeCheckBox.checked = filter.get('mode') === '1'
            }
        }

        Label {
            text: qsTr('Radius')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: radiusSlider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('radius') * 100
            onValueChanged: filter.set('radius', value / 100)
        }
        UndoButton {
            onClicked: radiusSlider.value = 50
        }

        Label {
            text: qsTr('Feathering')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: smoothSlider
            minimumValue: 0
            maximumValue: 500
            suffix: ' %'
            value: filter.getDouble('smooth') * 100
            onValueChanged: filter.set('smooth', value / 100)
        }
        UndoButton {
            onClicked: smoothSlider.value = 80
        }

        Label {}
        CheckBox {
            id: modeCheckBox
//            text: qsTr('Non-linear feathering')
            Layout.columnSpan: 2
            checked: filter.get('mode') === '1'
            property bool isReady: false
            Component.onCompleted: isReady = true
            onClicked: {
                if (isReady)
                    filter.set('mode', checked)
            }
            style: CheckBoxStyle {
                        label: Text {
                            color: "white"
                            text: qsTr('Non-linear feathering')
                        }
            }
        }

        Label {
            text: qsTr('Opacity')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: opacitySlider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: (1.0 - filter.getDouble('opacity')) * 100
            onValueChanged: filter.set('opacity', 1.0 - value / 100)
        }
        UndoButton {
            onClicked: opacitySlider.value = 100
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
