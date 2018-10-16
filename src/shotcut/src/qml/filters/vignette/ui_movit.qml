
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
            parameters: ['radius', 'inner_radius']
            onPresetSelected: {
                radiusSlider.value = filter.getDouble('radius') * 100
                innerSlider.value = filter.getDouble('inner_radius') * 100
            }
        }

        Label {
            text: qsTr('Outer radius')
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
            onClicked: radiusSlider.value = 30
        }

        Label {
            text: qsTr('Inner radius')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: innerSlider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('inner_radius') * 100
            onValueChanged: filter.set('inner_radius', value / 100)
        }
        UndoButton {
            onClicked: innerSlider.value = 30
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
