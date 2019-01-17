
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set('transition.fix_rotate_x', 0)
            filter.set('transition.scale_x', 1)
            filter.set('transition.ox', 0)
            filter.set('transition.oy', 0)
            filter.savePreset(preset.parameters)
        }
        setControls()
    }

    function setControls() {
        rotationSlider.value = filter.getDouble('transition.fix_rotate_x')
        scaleSlider.value = 100 / filter.getDouble('transition.scale_x')
        xOffsetSlider.value = filter.getDouble('transition.ox') * -1
        yOffsetSlider.value = filter.getDouble('transition.oy') * -1
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
            parameters: ['transition.fix_rotate_x', 'transition.scale_x', 'transition.ox', 'transition.oy']
            Layout.columnSpan: 2
            onPresetSelected: setControls()
        }

        Label {
            text: qsTr('Rotation')
            color: '#ffffff'
        }
        SliderSpinner {
            id: rotationSlider
            minimumValue: 0
            maximumValue: 360
            decimals: 1
            spinnerWidth: 110
            suffix: qsTr(' degree')
            onValueChanged: filter.set('transition.fix_rotate_x', value)
        }
        UndoButton {
            onClicked: rotationSlider.value = 0
        }

        Label {
            text: qsTr('Scale')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: scaleSlider
            minimumValue: 0.1
            maximumValue: 200
            decimals: 1
            spinnerWidth: 110
            suffix: ' %'
            onValueChanged: {
                filter.set('transition.scale_x', 100 / value)
                filter.set('transition.scale_y', 100 / value)
            }
        }
        UndoButton {
            onClicked: scaleSlider.value = 100
        }

        Label {
            text: qsTr('X offset')
            color: '#ffffff'
        }
        SliderSpinner {
            id: xOffsetSlider
            minimumValue: -1000
            maximumValue: 1000
            spinnerWidth: 110
            onValueChanged: filter.set('transition.ox', -value)
        }
        UndoButton {
            onClicked: xOffsetSlider.value = 0
        }

        Label {
            text: qsTr('Y offset')
            color: '#ffffff'
        }
        SliderSpinner {
            id: yOffsetSlider
            minimumValue: -1000
            maximumValue: 1000
            spinnerWidth: 110
            onValueChanged: filter.set('transition.oy', -value)
        }
        UndoButton {
            onClicked: yOffsetSlider.value = 0
        }

        Item {
            Layout.fillHeight: true;
        }
    }
}
