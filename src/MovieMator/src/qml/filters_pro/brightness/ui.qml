
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
            filter.set("level", 1.0);
        }

        var keyFrameCount = filter.getKeyFrameCountOnProject("level");
        if(keyFrameCount>0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "level");
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, "level");
                console.log("1......")
                console.log(nFrame)
                console.log(keyValue)
                filter.setKeyFrameParaValue(nFrame,"level", keyValue.toString());
            }



            brightnessSlider.value = filter.getKeyValueOnProjectOnIndex(0, "level") * 100.0
            console.log("6...brightnessSlider.value: ")
            console.log(brightnessSlider.value)

            filter.combineAllKeyFramePara();
        }
        else
            brightnessSlider.value = filter.getDouble("level") * 100.0
    }

    Connections {
        target: filterDock
        onPositionChanged:
        {
            var position        = timeline.getPositionInCurrentClip()
            var bKeyFrame       = filter.bKeyFrame(position)
            if (bKeyFrame)
            {
                var sliderValue = filter.getKeyFrameParaDoubleValue(position, "level");
                if(sliderValue >= 0.0)
                {
                    brightnessSlider.value = sliderValue * 100.0
                }
            }
        }
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
            onValueChanged: {
                console.log("8... setKeyFrameValue is called")

                    var position        = timeline.getPositionInCurrentClip()
                    var bKeyFrame       = filter.bKeyFrame(position)
                    if (bKeyFrame)
                    {
                        filter.setKeyFrameParaValue(position, "level", brightnessSlider.value/100.0)
                        filter.combineAllKeyFramePara();
                    }
                    else
                    {
                        filter.set("level", brightnessSlider.value/100.0);
                    }
            }

        }
        UndoButton {
            onClicked: {
                brightnessSlider.value = 100
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
