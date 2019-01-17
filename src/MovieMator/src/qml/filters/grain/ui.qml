
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set('noise', 40)
            filter.set('contrast', 100)
            filter.set('brightness', 83)
            filter.savePreset(preset.parameters)
        }
        setControls()
    }

    function setControls() {
        noiseSlider.value = filter.get('noise')
        brightnessSlider.value = filter.get('brightness')
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
            parameters: ['noise', 'brightness']
            Layout.columnSpan: 2
            onPresetSelected: setControls()
        }

        Label {
            text: qsTr('Noise')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: noiseSlider
            minimumValue: 1
            maximumValue: 200
            suffix: ' %'
            value: filter.get('noise')
            onValueChanged: filter.set('noise', value)
        }
        UndoButton {
            onClicked: noiseSlider.value = 40
        }

        Label {
            text: qsTr('Brightness')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: brightnessSlider
            minimumValue: 0
            maximumValue: 400
            value: filter.get('brightness')
            onValueChanged: filter.set('brightness', value)
        }
        UndoButton {
            onClicked: brightnessSlider.value = 83

        }

        Item {
            Layout.fillHeight: true
        }
    }
}
