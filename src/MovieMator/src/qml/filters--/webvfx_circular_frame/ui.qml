
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import MovieMator.Controls 1.0

Rectangle {
    width: 300
    height: 250
    color: 'transparent'
    Component.onCompleted: {
        if (filter.isNew) {
            filter.set('resource', filter.path + 'filter-demo.html')
            // Set default parameter values
            colorSwatch.value = 'black'
            filter.set('radius', 0.5)
            slider.value = filter.getDouble('radius') * slider.maximumValue
        }
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        Label {
            text: qsTr('Radius')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: slider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('radius') * slider.maximumValue
            onValueChanged: filter.set('radius', value / maximumValue)
        }
        UndoButton {
            onClicked: slider.value = 50
        }

        Label {
            text: qsTr('Color')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        ColorPicker {
            id: colorSwatch
            Layout.columnSpan: 2
            value: filter.get('color')
            property bool isReady: false
            Component.onCompleted: isReady = true
            onValueChanged: {
                if (isReady) {
                    filter.set('color', '' + value)
                    filter.set("disable", 0);
                }
            }
            onPickStarted: {
                filter.set("disable", 1);
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
