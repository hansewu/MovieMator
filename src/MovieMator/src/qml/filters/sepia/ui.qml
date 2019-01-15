
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

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

        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        Preset {
            id: preset
            Layout.columnSpan: 2
            parameters: ['u', 'v']
            onPresetSelected: {
                sliderBlue.value = filter.getDouble('u')
                sliderRed.value = filter.getDouble('v')
            }
        }

        Label {
            text: qsTr('Yellow-Blue')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: sliderBlue
            minimumValue: 0
            maximumValue: 255
            value: filter.getDouble('u')
            onValueChanged:filter.set('u', value)
        }
        UndoButton {
            onClicked: sliderBlue.value = 75
        }

        Label {
            text: qsTr('Cyan-Red')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: sliderRed
            minimumValue: 0
            maximumValue: 255
            value: filter.getDouble('v')
            onValueChanged: filter.set('v', value)
        }
        UndoButton {
            onClicked: sliderRed.value = 150
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
