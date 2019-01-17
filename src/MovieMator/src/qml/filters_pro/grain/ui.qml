
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())

    width: 300
    height: 250
    Component.onCompleted: {
        console.log("11... ")
        if (filter.isNew) {
            // Set default parameter values
            filter.set('noise', 40)
            filter.set('contrast', 100)
            filter.set('brightness', 83)
            filter.savePreset(preset.parameters)
        }
        setControls()
    }

    function setControls() {



        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-noise");
        console.log("1....., keyFrameCount:")
        console.log(keyFrameCount)
        if(keyFrameCount > 0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-noise");
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-noise");

                filter.setKeyFrameParaValue(nFrame, "noise", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-brightness");
                filter.setKeyFrameParaValue(nFrame, "brightness", keyValue)
            }

            filter.combineAllKeyFramePara();
            console.log(filter.getKeyValueOnProjectOnIndex(0, "anim-noise"))
            console.log(filter.getKeyValueOnProjectOnIndex(0, "anim-brightness"))

            noiseSlider.value = filter.getKeyValueOnProjectOnIndex(0, "anim-noise")
            brightnessSlider.value = filter.getKeyValueOnProjectOnIndex(0, "anim-brightness")
        }
        else
        {
            console.log("12...")
            console.log(filter.get('noise'))
            console.log(filter.get('brightness'))

            noiseSlider.value = filter.get('noise')
            brightnessSlider.value = filter.get('brightness')
        }
    }

    function setKeyFrameValue(bKeyFrame)
    {
        var nFrame = keyFrame.getCurrentFrame();
        if(bKeyFrame)
        {
            console.log("3....")
            console.log(noiseSlider.value)
            console.log(brightnessSlider.value)
            filter.setKeyFrameParaValue(nFrame, "noise", noiseSlider.value.toString())
            filter.setKeyFrameParaValue(nFrame, "brightness", brightnessSlider.value.toString())

            filter.combineAllKeyFramePara();
        }
        else
        {
            //Todo, delete the keyframe date of the currentframe
            filter.removeKeyFrameParaValue(nFrame);
             if(!filter.getKeyFrameNumber())
            {
                console.log("4......")
                filter.anim_set("noise","")
                filter.anim_set("brightness","")
            }
            filter.set("noise", noiseSlider.value);
            filter.set("brightness", brightnessSlider.value);

        }
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        KeyFrame{
             id: keyFrame
             Layout.columnSpan:3
        // 	currentPosition: filterDock.getCurrentPosition()
             onSetAsKeyFrame:
             {
                 console.log("9.... setKeyFrameValue will be called")
                setKeyFrameValue(bKeyFrame)
             }

             onLoadKeyFrame:
             {
                 var grainValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "noise");
                 if(grainValue != -1.0)
                 {
                     console.log("5.... grainValue:")
                     console.log(grainValue)
                     noiseSlider.value = grainValue
                 }

                 grainValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "brightness");
                 if(grainValue != -1.0)
                 {
                      console.log("12....")
                      console.log(grainValue)
                      brightnessSlider.value = grainValue
                 }
             }
         }
        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        Preset {
            id: preset
            enabled: bEnableControls
            parameters: ['noise', 'brightness']
            Layout.columnSpan: 2
            onPresetSelected: setControls()
        }

        Label {
            text: qsTr('Noise')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: noiseSlider
            enabled: bEnableControls
            minimumValue: 1
            maximumValue: 200
            suffix: ' %'
            value: filter.get('noise')
            onValueChanged: {
                console.log("6... setKeyFrameValue will be called")
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "noise",value.toString())
                    filter.combineAllKeyFramePara()

                }
                else
                    filter.set('noise', value)

               // setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                noiseSlider.value = 40
                setKeyFrameValue(keyFrame.bKeyFrame)
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
            minimumValue: 0
            maximumValue: 400
            value: filter.get('brightness')
            onValueChanged:{
                console.log("7... setKeyFrameValue will be called")
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "brightness",value.toString())
                    filter.combineAllKeyFramePara()

                }
                else
                    filter.set('brightness', value)
              //  setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                brightnessSlider.value = 83
                setKeyFrameValue(keyFrame.bKeyFrame)
            }

        }

        Item {
            Layout.fillHeight: true
        }
    }
}
