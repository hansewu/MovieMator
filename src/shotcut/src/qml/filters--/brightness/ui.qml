
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    
    function setKeyFrameValue(bKeyFrame)
    {
        var nFrame = keyFrame.getCurrentFrame();
        var contrastValue = brightnessSlider.value/100.0;
        if(bKeyFrame)
        {
            filter.setKeyFrameParaValue(nFrame,"level", contrastValue.toString() );
            filter.combineAllKeyFramePara();
        }
        else
        {
            //Todo, delete the keyframe date of the currentframe
            filter.removeKeyFrameParaValue(nFrame);
            filter.set("level", contrastValue);
        }
    }

    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set("level", 1.0);
        }
        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-level");
        var index=0
        for(index=0; index<keyFrameCount;index++)
        {
            var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-level");
            var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-level");

            filter.setKeyFrameParaValue(nFrame,"level", keyValue);
        }
        filter.combineAllKeyFramePara();

        brightnessSlider.value = filter.getKeyValueOnProjectOnIndex(0, "anim-level") * 100.0
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        KeyFrame{
             id: keyFrame
             Layout.columnSpan:3
        // 	 currentPosition: filterDock.getCurrentPosition()
             onSetAsKeyFrame:
             {
                setKeyFrameValue(bKeyFrame)
             }

             onLoadKeyFrame:
             {
                 var sliderValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "level");
                 if(sliderValue != -1.0)
                 {
                     brightnessSlider.value = sliderValue * 100.0
                 }

             }
         }

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
                setKeyFrameValue(keyFrame.bKeyFrame)
           //   filter.set("level", "0=0.0;200=2.0;400=0.0")//value / 100.0)
             //    filter.anim_set("level", "0=0.0;200=2.0;400=0.0")
            }
        }
        UndoButton {
            onClicked: {
                brightnessSlider.value = 100
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
