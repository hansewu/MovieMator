
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Item {
    width: 300
    height: 250

    Component.onCompleted: {
        if (filter.isNew) {
            filter.set('wave', 10)
            filter.set('speed', 5)
            filter.set('deformX', 1)
            filter.set('deformY', 1)
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
            color:'#ffffff'
        }
        Preset {
            id: preset
            Layout.columnSpan: 2
            parameters: ['wave', 'speed', 'deformX', 'deformX']
            onPresetSelected: {
                waveSlider.value = filter.getDouble('wave')
                speedSlider.value = filter.getDouble('speed')
                deformXCheckBox.checked = filter.get('deformX') === '1'
                deformYCheckBox.checked = filter.get('deformY') === '1'
            }
        }

        Label {
            text: qsTr('Amplitude')
            Layout.alignment: Qt.AlignRight
            color:'#ffffff'
        }
        SliderSpinner {
            id: waveSlider
            minimumValue: 1
            maximumValue: 500
            value: filter.getDouble('wave')
            onValueChanged: filter.set('wave', value)
        }
        UndoButton {
            onClicked: waveSlider.value = 10
        }

        Label {
            text: qsTr('Speed')
            Layout.alignment: Qt.AlignRight
            color:'#ffffff'
        }
        SliderSpinner {
            id: speedSlider
            minimumValue: 0
            maximumValue: 1000
            value: filter.getDouble('speed')
            onValueChanged: filter.set('speed', value)
        }
        UndoButton {
            onClicked: speedSlider.value = 5
        }

        Label {}
        CheckBox {
            id: deformXCheckBox
//            text: qsTr('Deform horizontally?')
            Layout.columnSpan: 2
            checked: filter.get('deformX') === '1'
            property bool isReady: false
            Component.onCompleted: isReady = true
            onClicked: {
                if (isReady)
                    filter.set('deformX', checked)
            }

            style: CheckBoxStyle {
                        label: Text {
                            color: "white"
                            text: qsTr('Deform horizontally?')
                        }
            }
        }

        Label {}
        CheckBox {
            id: deformYCheckBox
         //   text: qsTr('Deform vertically?')
            Layout.columnSpan: 2
            checked: filter.get('deformY') === '1'
            property bool isReady: false
            Component.onCompleted: isReady = true
            onClicked: {
                if (isReady)
                    filter.set('deformY', checked)
            }
            style: CheckBoxStyle {
                        label: Text {
                            color: "white"
                            text: qsTr('Deform vertically?')
                        }
            }

        }

        Item {
            Layout.fillHeight: true
        }
    }
}
