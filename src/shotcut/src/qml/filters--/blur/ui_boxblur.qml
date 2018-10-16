
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
   width: 300
    height: 250
    Component.onCompleted: {
        filter.set('start', 1)
     //   filter.set('blur', '0=0.1;100=1;200=0.1')
        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-hori");
        var index=0
        for(index=0; index<keyFrameCount;index++)
        {
            var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-hori");
            var horikeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-hori");
            var verKeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-vert");

            filter.setKeyFrameParaValue(nFrame, "hori", horikeyValue.toString())
            filter.setKeyFrameParaValue(nFrame, "vert", verKeyValue.toString())

        }

        filter.combineAllKeyFramePara();
        wslider.value = filter.getKeyValueOnProjectOnIndex(0, "anim-hori");
        hslider.value = filter.getKeyValueOnProjectOnIndex(0, "anim-vert");

        if (filter.isNew) {
            // Set default parameter values
            filter.set('hori', 2)
            wslider.value = 2;
            filter.set('vert', 2)
            hslider.value = 2

        }
    }

    function setKeyFrameValue(bKeyFrame)
     {
         var nFrame = keyFrame.getCurrentFrame();
         if(bKeyFrame)
         {
             filter.setKeyFrameParaValue(nFrame, "hori", wslider.value.toString())
             filter.setKeyFrameParaValue(nFrame, "vert", hslider.value.toString())

             filter.combineAllKeyFramePara();
         }
         else
         {
             //Todo, delete the keyframe date of the currentframe
             filter.removeKeyFrameParaValue(nFrame);

             filter.set("hori", wslider.value);
             filter.set("vert", hslider.value);

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
                var blurValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "hori");
                if(blurValue != -1.0)
                {
                    wslider.value = blurValue
                }
                blurValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "vert");
                if(blurValue != -1.0)
                {
                    hslider.value = blurValue
                }
            }
        }

        Label {
            text: qsTr('Width')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: wslider
            minimumValue: 1
            maximumValue: 99
            suffix: ' px'
            value: filter.getDouble('hori')
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "hori", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('hori', value)

            }
        }
        UndoButton {
            onClicked: wslider.value = 2
        }

        Label {
            text: qsTr('Height')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: hslider
            minimumValue: 1
            maximumValue: 99
            suffix: ' px'
            value: filter.getDouble('vert')
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "vert", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                  filter.set('vert', value)

                
            }
        }
        UndoButton {
            onClicked: hslider.value = 2
        }
        
        Item {
            Layout.fillHeight: true
        }
    }
}
