
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8

        RowLayout {
            Label {
                text: qsTr('Radius')
                color: '#ffffff'
            }
            SliderSpinner {
                id: slider
                minimumValue: 0
                maximumValue: 99.99
                value: filter.getDouble('radius')
                decimals: 2
                onValueChanged: filter.set('radius', value)
            }
            UndoButton {
                onClicked: slider.value = 3.0
            }
        }
        
        Item {
            Layout.fillHeight: true;
        }
    }
}
