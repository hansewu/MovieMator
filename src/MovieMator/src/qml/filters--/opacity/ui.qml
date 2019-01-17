
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
            filter.set('level', 1)
            filter.set('alpha', 100.0 / 100.0)
            filter.set('opacity', filter.getDouble('alpha'))
            slider.value = filter.getDouble('alpha') * 100.0
        }
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        Label {
            text: qsTr('Opacity')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: slider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('alpha') * 100.0
            onValueChanged: {
                filter.set('alpha', value / 100.0)
                filter.set('opacity', filter.getDouble('alpha'))
             //   filter.anim_set("alpha", "0=0.0;200=1.0;400=0.0")
             //   filter.anim_set("opacity", "0=0.0;200=1.0;400=0.0")
            }
        }
        UndoButton {
            onClicked: slider.value = 100
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
