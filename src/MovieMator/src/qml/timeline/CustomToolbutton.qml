import QtQuick 2.0


import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

ToolButton
{
    property url customIconSource
    property url pressedIconSource
    property string customText
    property bool bEnabled:true

    style: ButtonStyle {
            background: Rectangle {
                color: 'transparent'
            }
    }
    Image {
        source: parent.pressed? parent.pressedIconSource : parent.customIconSource
        fillMode: Image.PreserveAspectFit // For not stretching image (optional)
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 6 // Leaving space between image and borders (optional)
        anchors.bottomMargin:20 // Leaving space for text in bottom
    }
    Text {
        text: parent.customText
        color: bEnabled?'white':'grey'
        font.pixelSize: 10
        anchors.bottom: parent.bottom // Placing text in bottom
        anchors.margins: 4 // Leaving space between text and borders  (optional)
        anchors.horizontalCenter: parent.horizontalCenter // Centering text
        renderType: Text.NativeRendering // Rendering type (optional)
    }
}
