import QtQuick 2.0


import QtQuick.Controls 1.4

ToolButton
{
    property url customIconSource

    style: flat

    Image {
        source: parent.customIconSource
        fillMode: Image.PreserveAspectFit // For not stretching image (optional)
        anchors.fill: parent
        anchors.margins: 0 // Leaving space between image and borders (optional)
     //   anchors.bottomMargin:10 // Leaving space for text in bottom
    }

}
