
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    Component.onCompleted: {
        if (filter.isNew)
            filter.savePreset(preset.parameters)

        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-gamma_r");
        var index=0
        for(index=0; index<keyFrameCount;index++)
        {
            var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-u");
            var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-u");
            filter.setKeyFrameParaValue(nFrame, "u", keyValue)

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-v");
            filter.setKeyFrameParaValue(nFrame, "v", keyValue)
        }
        filter.combineAllKeyFramePara();
    }

    function setKeyFrameValue(bKeyFrame)
    {
        var nFrame = keyFrame.getCurrentFrame();
        if(bKeyFrame)
        {

            var sepiaValue = sliderBlue.value;
            filter.setKeyFrameParaValue(nFrame, "u", sepiaValue.toString())

            sepiaValue = sliderRed.value;
            filter.setKeyFrameParaValue(nFrame, "v", sepiaValue.toString())

            filter.combineAllKeyFramePara();
        }
        else
        {
            //Todo, delete the keyframe date of the currentframe
            filter.removeKeyFrameParaValue(nFrame);

            filter.set('u', sliderBlue.value)
            filter.set('v', sliderRed.value)
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
                 var  sepiaValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "u");
                 if(sepiaValue != -1.0)
                 {
                     sliderBlue.value = sepiaValue
                 }

                 sepiaValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "v");
                 if(sepiaValue != -1.0)
                 {
                     sliderRed.value = sepiaValue
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
            parameters: ['u', 'v']
            onPresetSelected: {
                sliderBlue.value = filter.getDouble('u')
                sliderRed.value = filter.getDouble('v')
            }
        }

        Label {
            text: qsTr('Yellow-Blue')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: sliderBlue
            minimumValue: 0
            maximumValue: 255
            value: filter.getDouble('u')
            onValueChanged:{
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "u", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('u', value)
            }
        }

        UndoButton {
            onClicked: sliderBlue.value = 75
        }

        Label {
            text: qsTr('Cyan-Red')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: sliderRed
            minimumValue: 0
            maximumValue: 255
            value: filter.getDouble('v')
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "v", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('v', value)
            }
        }
        UndoButton {
            onClicked: sliderRed.value = 150
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
