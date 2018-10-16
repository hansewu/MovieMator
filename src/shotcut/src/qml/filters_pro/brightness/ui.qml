
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())

    function setKeyFrameValue(bKeyFrame)
    {
        var nFrame = keyFrame.getCurrentFrame();
        var contrastValue = brightnessSlider.value/100.0;
        if(bKeyFrame)
        {
            filter.setKeyFrameParaValue(nFrame,"level", contrastValue.toString() );
            console.log("4.... combineAllKeyFramePara is called")
            console.log(contrastValue)
            filter.combineAllKeyFramePara();
        }
        else
        {
            //Todo, delete the keyframe date of the currentframe
            filter.removeKeyFrameParaValue(nFrame);
            if(!filter.getKeyFrameNumber())
            {
                console.log("5.... anim_set is called")
                filter.anim_set("level","")
            }
            else
                 filter.set("level", contrastValue);
        }
    }

    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set("level", 1.0);
        }



        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-level");
        if(keyFrameCount>0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-level");
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-level");
                console.log("1......")
                console.log(nFrame)
                console.log(keyValue)
                filter.setKeyFrameParaValue(nFrame,"level", keyValue);
            }
            filter.combineAllKeyFramePara();


            brightnessSlider.value = filter.getKeyValueOnProjectOnIndex(0, "anim-level") * 100.0
            console.log("6...brightnessSlider.value: ")
            console.log(brightnessSlider.value)
        }
        else
            brightnessSlider.value = filter.getDouble("level") * 100.0
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
                 console.log("10... bKeyFrame:")
                 console.log(bKeyFrame)

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
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: brightnessSlider
            enabled: bEnableControls
            minimumValue: 0.0
            maximumValue: 200.0
            decimals: 1
            spinnerWidth: 80
            suffix: ' %'
            onValueChanged: {
                console.log("8... setKeyFrameValue is called")
                setKeyFrameValue(keyFrame.bKeyFrame)
           
            }
        }
        UndoButton {
            enabled: bEnableControls
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
