
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())

    function setKeyFrameValue(bKeyFrame)
     {
         var nFrame = keyFrame.getCurrentFrame();
         if(bKeyFrame)
         {

             var contrastValue = radiusSlider.value;
             filter.setKeyFrameParaValue(nFrame, "radius", (contrastValue/100).toString())

             contrastValue = innerSlider.value;
             filter.setKeyFrameParaValue(nFrame, "inner_radius", (contrastValue/100).toString())

             filter.combineAllKeyFramePara();
         }
         else
         {
             //Todo, delete the keyframe date of the currentframe
             filter.removeKeyFrameParaValue(nFrame);
             if(!filter.getKeyFrameNumber())
             {
                filter.anim_set("radius","")
                filter.anim_set("inner_radius","")
                
             }
             filter.set("radius", radiusSlider.value/100);
             filter.set("inner_radius", innerSlider.value/100);

         }
     }

    Component.onCompleted: {
        if (filter.isNew)
            filter.savePreset(preset.parameters)

        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-radius");
        if(keyFrameCount > 0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-radius");
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-radius");

                filter.setKeyFrameParaValue(nFrame, "radius", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-inner_radius");
                filter.setKeyFrameParaValue(nFrame, "inner_radius", keyValue)
            }
            filter.combineAllKeyFramePara();
        }
        else
        {
            filter.set("radius", radiusSlider.value/100);
            filter.set("inner_radius", innerSlider.value/100);
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
                setKeyFrameValue(bKeyFrame)
             }

             onLoadKeyFrame:
             {
                 var vignetteValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "radius");
                 if(vignetteValue != -1.0)
                 {
                     radiusSlider.value = vignetteValue* 100.0
                 }

                 vignetteValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "inner_radius");
                 if(vignetteValue != -1.0)
                 {
                      innerSlider.value = vignetteValue* 100.0
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
            Layout.columnSpan: 2
            parameters: ['radius', 'inner_radius']
            onPresetSelected: {
                radiusSlider.value = filter.getDouble('radius') * 100
                innerSlider.value = filter.getDouble('inner_radius') * 100
            }
        }

        Label {
            text: qsTr('Outer radius')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: radiusSlider
            enabled: bEnableControls
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('radius') * 100
            onValueChanged:{
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked:{
                 radiusSlider.value = 30
                 setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {
            text: qsTr('Inner radius')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: innerSlider
            enabled: bEnableControls
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('inner_radius') * 100
            onValueChanged: {
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                innerSlider.value = 30
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
