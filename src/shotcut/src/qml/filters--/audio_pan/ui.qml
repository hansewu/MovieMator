
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
            combo.currentIndex = 0
            filter.set('start', 0)
            slider.value = 0
        }
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        Label {
            text: qsTr('Channel')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        ComboBox {
            id: combo
            Layout.columnSpan: 2
            model: [qsTr('Left'), qsTr('Right')]
            onCurrentIndexChanged:{
                filter.set('channel', currentIndex)
             //   filter.anim_set("channel", "0=-50;200=-10;400=-50")
            } 
        }

        Label {
            text: qsTr('Left')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: slider
            minimumValue: 0
            maximumValue: 1000
            label: qsTr('Right')
            ratio: maximumValue
            decimals: 2
            value: filter.getDouble('start') * maximumValue
            onValueChanged: filter.set('start', value / maximumValue)
        }
        UndoButton {
            onClicked: slider.value = 0
        }

        Item {
            Layout.fillHeight: true;
        }
    }
}
