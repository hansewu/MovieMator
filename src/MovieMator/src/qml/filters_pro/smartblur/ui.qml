
import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import MovieMator.Controls 1.0

Item {
    property double radiusDefault: 2.5
    property double strengthDefault: 0.5
    property double thresholdDefault: 3.0

    property var defaultParameters: ["av.luma_radius", "av.chroma_radius", "av.luma_strength", "av.chroma_strength", "av.luma_threshold", "av.chroma_threshold"]
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())

    width: 300
    height: 250
    Component.onCompleted: {
        if (filter.isNew) {
            filter.set("av.luma_radius", radiusDefault)
            filter.set("av.chroma_radius", radiusDefault)
            filter.set("av.luma_strength", strengthDefault)
            filter.set("av.chroma_strength", strengthDefault)
            filter.set("av.luma_threshold", thresholdDefault)
            filter.set("av.chroma_threshold", thresholdDefault)
            filter.savePreset(defaultParameters)
        }

        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-av.luma_radius");
        console.log("1......")
        console.log(keyFrameCount)
        if(keyFrameCount > 0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-av.luma_radius");
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-av.luma_radius");
                filter.setKeyFrameParaValue(nFrame, "av.luma_radius", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-av.chroma_radius");
                filter.setKeyFrameParaValue(nFrame, "av.chroma_radius", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-av.luma_strength");
                filter.setKeyFrameParaValue(nFrame, "av.luma_strength", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-av.chroma_strength");
                filter.setKeyFrameParaValue(nFrame, "av.chroma_strength", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-av.luma_threshold");
                filter.setKeyFrameParaValue(nFrame, "av.luma_threshold",  keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-av.chroma_threshold");
                filter.setKeyFrameParaValue(nFrame, "av.chroma_threshold", keyValue)
            }

            filter.combineAllKeyFramePara();

            radiusSlider.value = filter.getKeyValueOnProjectOnIndex(0, "anim-av.luma_radius")
            strengthSlider.value = filter.getKeyValueOnProjectOnIndex(0, "anim-av.luma_strength")
            thresholdSlider.value = filter.getKeyValueOnProjectOnIndex(0, "anim-av.luma_threshold")
        }
        else
        {
            setControls()
        }
    }

    function setKeyFrameValue(bKeyFrame)
     {
        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-av.luma_radius");
        console.log("2......")
        console.log(keyFrameCount)
         var nFrame = keyFrame.getCurrentFrame();
         if(bKeyFrame)
         {

             var blurValue = radiusSlider.value;
             filter.setKeyFrameParaValue(nFrame,"av.luma_radius", blurValue.toString())
             filter.setKeyFrameParaValue(nFrame,"av.chroma_radius", blurValue.toString())

             blurValue = strengthSlider.value;
             filter.setKeyFrameParaValue(nFrame, "av.luma_strength", blurValue.toString())
             filter.setKeyFrameParaValue(nFrame, "av.chroma_strength", blurValue.toString())

             blurValue = thresholdSlider.value;
             filter.setKeyFrameParaValue(nFrame, "av.luma_threshold",  blurValue.toString())
             filter.setKeyFrameParaValue(nFrame, "av.chroma_threshold", blurValue.toString())

             filter.combineAllKeyFramePara();
         }
         else
         {
             //Todo, delete the keyframe date of the currentframe
             filter.removeKeyFrameParaValue(nFrame);
             if(!filter.getKeyFrameNumber())
            {
                filter.anim_set("av.luma_radius","")
                filter.anim_set("av.chroma_radius","")
                filter.anim_set("av.luma_strength","")
                filter.anim_set("av.chroma_strength","")
                filter.anim_set("av.luma_threshold","")
                filter.anim_set("av.chroma_threshold","")
            }
             filter.set("av.luma_radius", radiusSlider.value)
             filter.set("av.chroma_radius", radiusSlider.value)

             filter.set("av.luma_strength", strengthSlider.value)
             filter.set("av.chroma_strength", strengthSlider.value)

             filter.set("av.luma_threshold", thresholdSlider.value)
             filter.set("av.chroma_threshold", thresholdSlider.value)
         }
     }

    function setControls() {
        radiusSlider.value = filter.getDouble("av.luma_radius")
        strengthSlider.value = filter.getDouble("av.luma_strength")
        thresholdSlider.value = filter.getDouble("av.luma_threshold")



    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        KeyFrame{
             id: keyFrame
             Layout.columnSpan:3
        //   currentPosition: filterDock.getCurrentPosition()
             onSetAsKeyFrame:
             {
                setKeyFrameValue(bKeyFrame)
             }

             onLoadKeyFrame:
             {
                 var blurValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "av.luma_radius");
                 if(blurValue != -1.0)
                 {
                     radiusSlider.value = blurValue
                 }

                 blurValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "av.luma_threshold");
                 if(blurValue != -1.0)
                 {
                     thresholdSlider.value = blurValue

                 }

                 blurValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "av.luma_strength");
                 if(blurValue != -1.0)
                 {
                     strengthSlider.value = blurValue

                 }


             }
         }
        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        Preset {
            id: presetItem
            enabled: bEnableControls
            Layout.columnSpan: 2
            onPresetSelected: setControls()
        }

        Label {
            text: qsTr('Blur Radius')
            Layout.alignment: Qt.AlignRight
            ToolTip {text: qsTr('The radius of the gaussian blur.')}
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: radiusSlider
            enabled: bEnableControls
            minimumValue: 0.1
            maximumValue: 5.0
            decimals: 1
            spinnerWidth: 80
            onValueChanged: {
                setKeyFrameValue(keyFrame.bKeyFrame)

            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                radiusSlider.value = radiusDefault
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {
            text: qsTr('Blur Strength')
            Layout.alignment: Qt.AlignRight
            ToolTip {text: qsTr('The strength of the gaussian blur.')}
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: strengthSlider
            enabled: bEnableControls
            minimumValue: 0.0
            maximumValue: 1.0
            decimals: 1
            spinnerWidth: 80
            onValueChanged: {
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                strengthSlider.value = strengthDefault
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        
        Label {
            text: qsTr('Threshold')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
            ToolTip {text: qsTr('If the difference between the original pixel and the blurred pixel is less than threshold, the pixel will be replaced with the blurred pixel.')}
        }
        SliderSpinner {
            id: thresholdSlider
            enabled: bEnableControls
            minimumValue: 0.0
            maximumValue: 30
            decimals: 0
            spinnerWidth: 80
            onValueChanged: {
                setKeyFrameValue(keyFrame.bKeyFrame)


            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                thresholdSlider.value = thresholdDefault
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Item { Layout.fillHeight: true }
    }
}
