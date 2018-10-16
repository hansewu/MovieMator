
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250

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
            id: radiusSlider
            minimumValue: 0
            maximumValue: 2000
            ratio: 100
            decimals: 2
            stepSize: 0.1
            value: filter.getDouble('radius') * 100
            onValueChanged: filter.set('radius', value / 100)
        }
        UndoButton {
            onClicked: radiusSlider.value = 300
        }

        Label {
            text: qsTr('Blurriness')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: mixSlider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('mix') * 100
            onValueChanged: filter.set('mix', value / 100)
        }
        UndoButton {
            onClicked: mixSlider.value = 30
        }

        Item {
            Layout.fillHeight: true;
        }
    }
}
