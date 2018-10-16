
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
         var sliderValue = slider.value;
         if(bKeyFrame)
         {

             filter.setKeyFrameParaValue(nFrame,"radius", sliderValue.toString() );

             filter.combineAllKeyFramePara();
         }
         else
         {
             //Todo, delete the keyframe date of the currentframe
             filter.removeKeyFrameParaValue(nFrame);
             filter.set("radius", sliderValue);

         }
     }

    ColumnLayout {
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
                 var sliderValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "radius");
                 if(sliderValue != -1.0)
                 {
                      slider.value = sliderValue
                 }

            }
        }

        RowLayout {
            Label {
                text: qsTr('Radius')
                color: '#ffffff'
            }
            SliderSpinner {
                id: slider
                minimumValue: 0
                maximumValue: 99.99
                value: filter.getDouble('radius')
                decimals: 2
                onValueChanged: {
                    setKeyFrameValue(keyFrame.bKeyFrame)
                   // filter.set('radius', value)
                }
            }
            UndoButton {
                onClicked: {
                    slider.value = 3.0
                    setKeyFrameValue(keyFrame.bKeyFrame)
                }
            }
        }
        
        Item {
            Layout.fillHeight: true;
        }
    }
}
