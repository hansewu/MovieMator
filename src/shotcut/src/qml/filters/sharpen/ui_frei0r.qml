
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property string paramAmount: '0'
    property string paramSize: '1'
    property var defaultParameters: [paramAmount, paramSize]
    width: 300
    height: 250
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set(paramAmount, 0.5)
            filter.set(paramSize, 0.5)
            filter.savePreset(defaultParameters)
            aslider.value = filter.getDouble(paramAmount) * 100.0
            sslider.value = filter.getDouble(paramSize) * 100.0
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
            Layout.columnSpan: 2
            parameters: defaultParameters
            onPresetSelected: {
                aslider.value = filter.getDouble(paramAmount) * 100.0
                sslider.value = filter.getDouble(paramSize) * 100.0
            }
        }

        Label {
            text: qsTr('Amount')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: aslider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            decimals: 1
            value: filter.getDouble(paramAmount) * 100.0
            onValueChanged: filter.set(paramAmount, value / 100.0)
        }
        UndoButton {
            onClicked: aslider.value = 50
        }

        Label {
            text: qsTr('Size')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: sslider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            decimals: 1
            value: filter.getDouble(paramSize) * 100.0
            onValueChanged: filter.set(paramSize, value / 100.0)
        }
        UndoButton {
            onClicked: sslider.value = 50
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
