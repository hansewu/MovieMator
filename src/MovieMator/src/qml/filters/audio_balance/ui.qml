
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
            filter.set('start', 0.5)
            slider.value = filter.getDouble('start') * 1000
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8

        RowLayout {
            Label {
                text: qsTr('Left')
                color: '#ffffff'
            }
            SliderSpinner {
                id: slider
                minimumValue: 0
                maximumValue: 1000
                ratio: 1000
                decimals: 2
                label: qsTr('Right')
//color: '#ffffff'
                value: filter.getDouble('start') * maximumValue
                onValueChanged: filter.set('start', value / maximumValue)
            }
            UndoButton {
                onClicked: slider.value = 500
            }
        }
        Item {
            Layout.fillHeight: true;
        }
    }
}
