
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property var defaultParameters: ['radius','blur_mix','highlight_cutoff']
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
            color: '#ffffff'
        }
        Preset {
            Layout.columnSpan: 2
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
            color: '#ffffff'
        }
        SliderSpinner {
            id: radiusslider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            value: filter.getDouble("radius")
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "radius", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set("radius", value)
            }
        }
        UndoButton {
            onClicked: radiusslider.value = 20
        }

        // Row 2
        Label { 
            text: qsTr('Highlight blurriness')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: blurslider
            minimumValue: 0.0
            maximumValue: 1.0
            decimals: 2
            value: filter.getDouble("blur_mix")
            onValueChanged:
            {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "blur_mix", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set("blur_mix", value)
            }

        }
        UndoButton {
            onClicked: blurslider.value = 1.0
        }

        // Row 3
        Label {
            text: qsTr('Highlight cutoff')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: cutoffslider
            minimumValue: 0.1
            maximumValue: 1.0
            decimals: 2
            value: filter.getDouble("highlight_cutoff")
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "highlight_cutoff", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set("highlight_cutoff", value)
            }
        }
        UndoButton {
            onClicked: cutoffslider.value = 0.2
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
