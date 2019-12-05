import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

SpinBox
{
    SystemPalette { id: activePalette; colorGroup: SystemPalette.Active }

    style: SpinBoxStyle {
        background: Rectangle {
            implicitWidth: 100
            implicitHeight: 20
            border.color: "gray"
            radius: 2
            color: activePalette.text
        }
        textColor: "#b9b9b9"
    }
}
