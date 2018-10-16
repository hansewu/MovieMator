
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
            filter.set('alpha', 1.0)
            filter.set('opacity', 1.0);
        }
        brightnessSlider.value = filter.getDouble('opacity') * 100.0
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        Label {
            text: qsTr('Brightness')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: brightnessSlider
            minimumValue: 0.0
            maximumValue: 200.0
            decimals: 1
            spinnerWidth: 80
            suffix: ' %'
            onValueChanged: filter.set("opacity", value / 100.0)
        }
        UndoButton {
            onClicked: brightnessSlider.value = 100
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
