import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

    RowLayout
    {
        property alias value: slider.value
        property alias minimumValue: slider.minimumValue
        property alias maximumValue: slider.maximumValue
        property alias stepSize: slider.stepSize
        property alias tickmarksEnabled: slider.tickmarksEnabled
        property alias textColor: label.color

        Slider
        {
            id: slider
        }

        Label
        {
            id: label
            Layout.preferredWidth: 30
            horizontalAlignment: Text.AlignRight
            text:slider.value + 's'
        }
    }

