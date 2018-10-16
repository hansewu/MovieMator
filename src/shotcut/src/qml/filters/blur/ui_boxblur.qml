
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
   width: 300
    height: 250
    Component.onCompleted: {
        filter.set('start', 1)
        if (filter.isNew) {
            // Set default parameter values
            filter.set('hori', 2)
            wslider.value = 2;
            filter.set('vert', 2)
            hslider.value = 2
        }
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        Label {
            text: qsTr('Width')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: wslider
            minimumValue: 1
            maximumValue: 99
            suffix: ' px'
            value: filter.getDouble('hori')
            onValueChanged: filter.set('hori', value)
        }
        UndoButton {
            onClicked: wslider.value = 2
        }

        Label {
            text: qsTr('Height')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: hslider
            minimumValue: 1
            maximumValue: 99
            suffix: ' px'
            value: filter.getDouble('vert')
            onValueChanged: filter.set('vert', value)
        }
        UndoButton {
            onClicked: hslider.value = 2
        }
        
        Item {
            Layout.fillHeight: true
        }
    }
}
