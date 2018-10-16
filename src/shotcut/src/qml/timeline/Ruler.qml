
import QtQuick 2.0
import QtQuick.Controls 1.0

Rectangle {
    property int stepSize: 34
    property int index: 0
    property real timeScale: 1.0

    SystemPalette { id: activePalette }

    id: rulerTop
    enabled: false
    height: 24
    color: normalColor//activePalette.base

    Rectangle {
        width: timeline.timeToFrames("00:05:00:00")*timeScale * parent.width / parent.width > parent.width ? parent.width : timeline.timeToFrames("00:05:00:00")*timeScale * parent.width / parent.width
        height: parent.height
        anchors.left: parent.left
        color:'seagreen'
    }

    Repeater {
        model: parent.width / stepSize
        Rectangle {
            anchors.bottom: rulerTop.bottom
            height: (index % 4)? ((index % 2) ? 3 : 7) : 14
            width: 1
            color: activePalette.windowText//'#707070'//activePalette.windowText
            x: index * stepSize
        }
    }
    Repeater {
        model: parent.width / stepSize / 4
        Label {
            anchors.bottom: rulerTop.bottom
            anchors.bottomMargin: 2
            color: activePalette.windowText
            x: index * stepSize * 4 + 2
            text: timeline.timecode(index * stepSize * 4 / timeScale)
            font.pointSize: 7.5
        }
    }

    Rectangle {
        width: parent.width
        height: 1
        anchors.bottom: parent.bottom
        color: activePalette.windowText//'#707070'//activePalette.windowText
    }
}
