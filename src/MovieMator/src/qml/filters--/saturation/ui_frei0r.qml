
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    property string saturationParameter: '0'
    property real frei0rMaximum: 800
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set(saturationParameter, 100 / frei0rMaximum)
            slider.value = filter.getDouble(saturationParameter) * frei0rMaximum
            filter.savePreset(preset.parameters)
        }
        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-0");
        var index=0
        for(index=0; index<keyFrameCount;index++)
        {
             var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-0");
             var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-0");
             filter.setKeyFrameParaValue(nFrame, saturationParameter ,  keyValue)
        }

        filter.combineAllKeyFramePara();
    }

    function setKeyFrameValue(bKeyFrame)
     {
         var nFrame = keyFrame.getCurrentFrame();
         if(bKeyFrame)
         {

             filter.setKeyFrameParaValue(nFrame,saturationParameter ,  (slider.value / frei0rMaximum).toString())
             filter.combineAllKeyFramePara();
         }
         else
         {
             //Todo, delete the keyframe date of the currentframe
             filter.removeKeyFrameParaValue(nFrame);

             filter.set(saturationParameter, slider.value / frei0rMaximum)
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
                var saturationValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, saturationParameter);
                if(saturationValue != -1.0)
                {
                    slider.value = saturationValue * frei0rMaximum
                }

            }
        }

        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        Preset {
            id: preset
            Layout.columnSpan: 2
            parameters: [saturationParameter]
            onPresetSelected: {
                slider.value = filter.getDouble(saturationParameter) * frei0rMaximum
            }
        }

        Label {
            text: qsTr('Saturation')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: slider
            minimumValue: 0
            maximumValue: 300
            suffix: ' %'
            value: filter.getDouble(saturationParameter) * frei0rMaximum
            onValueChanged:{
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame,saturationParameter ,  (value / frei0rMaximum).toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set(saturationParameter, value / frei0rMaximum)
            }
        }
        UndoButton {
            onClicked: slider.value = 100
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
