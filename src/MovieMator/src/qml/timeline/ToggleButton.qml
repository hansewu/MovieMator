
import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import MovieMator.Controls 1.0 as MovieMator

CheckBox {
    property string iconName
    property url iconSource
    property alias tooltip: tooltip.text

    anchors.verticalCenter: parent.verticalCenter
    style: CheckBoxStyle {
        background: Rectangle {
            implicitWidth: 28
            implicitHeight: 24
            radius: 3
            SystemPalette { id: activePalette }
            color: control.checked? activePalette.highlight : activePalette.button
            border.color: activePalette.shadow
            border.width: 1
        }
        indicator: ToolButton {
            x: 3
            implicitWidth: 24
            implicitHeight: 20
            iconName: control.iconName
            iconSource: control.iconSource
        }
    }
    MovieMator.ToolTip { id: tooltip }
}
