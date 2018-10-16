
import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.4

Rectangle {
    property alias value: slider.value

    SystemPalette { id: activePalette }

    color: 'transparent'
    width: 200
    height: 24

    Slider {
        id: slider
        orientation: Qt.Horizontal
        style: SliderStyle {
                groove: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 3
                    color: "black"
                    radius: 1
                }
                handle: Rectangle {
                    anchors.centerIn: parent
                    color: control.pressed ? "lightgray" : "white"
                    border.color: "gray"
                    border.width: 1
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 8
                }
        }

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            leftMargin: 4
            rightMargin: 4
        }
        minimumValue: 0
        maximumValue: 1.75
        value: 1
        onValueChanged: {
            if (typeof multitrack.scaleFactor != 'undefined')
                multitrack.scaleFactor = Math.pow(value, 3) + 0.01
        }
        onPressedChanged: {
            if (!pressed) {
                for (var i = 0; i < tracksRepeater.count; i++)
                    tracksRepeater.itemAt(i).redrawWaveforms()
            }
        }
    }
}
