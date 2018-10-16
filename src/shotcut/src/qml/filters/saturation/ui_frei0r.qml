
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    property string saturationParameter: '0'
    property real frei0rMaximum: 800
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set(saturationParameter, 100 / frei0rMaximum)
            slider.value = filter.getDouble(saturationParameter) * frei0rMaximum
            filter.savePreset(preset.parameters)
        }
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        Preset {
            id: preset
            Layout.columnSpan: 2
            parameters: [saturationParameter]
            onPresetSelected: {
                slider.value = filter.getDouble(saturationParameter) * frei0rMaximum
            }
        }

        Label {
            text: qsTr('Saturation')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: slider
            minimumValue: 0
            maximumValue: 300
            suffix: ' %'
            value: filter.getDouble(saturationParameter) * frei0rMaximum
            onValueChanged: filter.set(saturationParameter, value / frei0rMaximum)
        }
        UndoButton {
            onClicked: slider.value = 100
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
