
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property var defaultParameters: ['radius','blur_mix','highlight_cutoff']
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())

    width: 300
    height: 250

    function setKeyFrameValue(bKeyFrame)
     {
         var nFrame = keyFrame.getCurrentFrame();
         if(bKeyFrame)
         {

             var glowValue = radiusslider.value;
             filter.setKeyFrameParaValue(nFrame,"radius", glowValue.toString() );

             glowValue = blurslider.value;
             filter.setKeyFrameParaValue(nFrame,"blur_mix", glowValue.toString() )

             glowValue = cutoffslider.value;
             filter.setKeyFrameParaValue(nFrame,"cutoffslider", glowValue.toString() )


             filter.combineAllKeyFramePara();
         }
         else
         {
             //Todo, delete the keyframe date of the currentframe
             filter.removeKeyFrameParaValue(nFrame);
             if(!filter.getKeyFrameNumber())
            {
                filter.anim_set("radius","")
                filter.anim_set("blur_mix","")
                filter.anim_set("cutoffslider","")
            }
             filter.set("radius", radiusslider.value);
             filter.set("blur_mix", blurslider.value);
             filter.set("cutoffslider", cutoffslider.value);

         }
     }

    Component.onCompleted: {
        filter.set('start', 1)
        if (filter.isNew) {
            // Set default parameter values
            filter.set('radius', 20.0)
            filter.set('blur_mix', 1.0)
            filter.set('highlight_cutoff', 0.2)
            filter.savePreset(defaultParameters)
            radiusslider.value = filter.getDouble("radius")
            blurslider.value = filter.getDouble("blur_mix")
            cutoffslider.value = filter.getDouble("highlight_cutoff")
        }

        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-radius");
        if(keyFrameCount>0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-radius");
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-radius");

                filter.setKeyFrameParaValue(nFrame,"radius", keyValue );

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-blur_mix");
                filter.setKeyFrameParaValue(nFrame,"blur_mix", glowValue.toString() )

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-cutoffslider");
                filter.setKeyFrameParaValue(nFrame,"cutoffslider", glowValue.toString() )


            }
            filter.combineAllKeyFramePara();
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
                 var glowValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "radius");
                 if(glowValue != -1.0)
                 {
                     radiusslider.value = glowValue;
                 }

                 glowValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "blur_mix");
                 if(glowValue != -1.0)
                 {
                     blurslider.value = glowValue;
                 }

                 glowValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "cutoffslider");
                 if(glowValue != -1.0)
                 {
                     cutoffslider.value = glowValue;
                 }

             }
         }
        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        Preset {
            Layout.columnSpan: 2
            enabled: bEnableControls
            parameters: defaultParameters
            onPresetSelected: {
                radiusslider.value = filter.getDouble("radius")
                blurslider.value = filter.getDouble("blur_mix")
                cutoffslider.value = filter.getDouble("highlight_cutoff")
            }
        }

        // Row 1
        Label {
            text: qsTr('Radius')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: radiusslider
            enabled: bEnableControls
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            value: filter.getDouble("radius")
            onValueChanged: {
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                radiusslider.value = 20
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        // Row 2
        Label { 
            text: qsTr('Highlight blurriness')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: blurslider
            enabled: bEnableControls
            minimumValue: 0.0
            maximumValue: 1.0
            decimals: 2
            value: filter.getDouble("blur_mix")
            onValueChanged:
            {
                setKeyFrameValue(keyFrame.bKeyFrame)
            }

        }
        UndoButton {
            
            enabled: bEnableControls
            onClicked: {
                blurslider.value = 1.0
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        // Row 3
        Label {
            text: qsTr('Highlight cutoff')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: cutoffslider
            enabled: bEnableControls
            minimumValue: 0.1
            maximumValue: 1.0
            decimals: 2
            value: filter.getDouble("highlight_cutoff")
            onValueChanged: {
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                cutoffslider.value = 0.2
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
