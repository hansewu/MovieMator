
import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import MovieMator.Controls 1.0

RowLayout {
    property string value: "white"
    property bool alpha: false
    property alias eyedropper: pickerButton.visible
    
    signal pickStarted
    
    SystemPalette { id: activePalette; colorGroup: SystemPalette.Active }
    
    ColorPickerItem {
        id: pickerItem
        onColorPicked: {
            value = color
            pickerButton.checked = false
        }
    }
    
    Button {
        id: colorButton
        implicitWidth: 20
        implicitHeight: 20
        style: ButtonStyle {
            background: Rectangle {
                border.width: 1
                border.color: 'gray'
                radius: 3
                color: value
            }
        }
        onClicked: colorDialog.visible = true
        tooltip: qsTr('Click to open color dialog')
    }
    
    ColorDialog {
        id: colorDialog
        title: qsTr("Please choose a color")
        showAlphaChannel: alpha
        color: value
        onAccepted: {
            if (alpha) {
                var alphaHex = Math.round(255 * currentColor.a).toString(16)
                if (alphaHex.length === 1)
                    alphaHex = '0' + alphaHex
                value = '#' + alphaHex + currentColor.toString().substr(1)
            } else {
                value = currentColor
            }
        }
    }
    
    Button {
        id: pickerButton
     //   iconName: 'color-picker'
        iconSource: 'qrc:///icons/light/32x32/color-picker.png'
        tooltip: '<p>' + qsTr("Pick a color on the screen. By pressing the mouse button and then moving your mouse you can select a section of the screen from which to get an average color.") + '</p>'
        implicitWidth: 20
        implicitHeight: 20
        checkable: true
        onClicked: {
            pickStarted()
            pickerItem.pickColor()
        }
    }
}
