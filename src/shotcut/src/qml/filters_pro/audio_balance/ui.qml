
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber()) 

    width: 300
    height: 250

    function setKeyFrameValue(bKeyFrame)
    {
        var nFrame = keyFrame.getCurrentFrame();
        var sliderValue = slider.value/1000.0;
        if(bKeyFrame)
        {
            filter.setKeyFrameParaValue(nFrame,"start", sliderValue.toString() );
            filter.combineAllKeyFramePara();
        }
        else
        {
            filter.removeKeyFrameParaValue(nFrame);
            if(!filter.getKeyFrameNumber())
            {
                filter.anim_set("start","")
            }
            filter.set("start", sliderValue);
        }
    }
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set('start', 0.5)

            slider.value = filter.getDouble('start') * 1000
        }
        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-start");
        if(keyFrameCount>0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-start");
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-start");
                var sliderValue = keyValue * 1000.0;
                filter.setKeyFrameParaValue(nFrame,"start", sliderValue.toString() );

            }
            filter.combineAllKeyFramePara();

            slider.value = filter.getKeyValueOnProjectOnIndex(0,"anim-start" ) * 1000.0
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
                   var sliderValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "start");
                   if(gammaKeyValue != -1.0)
                   {
                        slider.value = sliderValue * 1000
                   }

             }
        }

        RowLayout {



            Label {
                text: qsTr('Left')
                color: bEnableControls?'#ffffff': '#828282'
            }
            SliderSpinner {

                id: slider
                enabled: bEnableControls 
                minimumValue: 0
                maximumValue: 1000
                ratio: 1000
                decimals: 2
                label: qsTr('Right')

                value: filter.getDouble('start') * maximumValue
                onValueChanged: {
                  
                    setKeyFrameValue(keyFrame.bKeyFrame)
                }
                
            }
            UndoButton {
                onClicked:
                {
                    slider.value = 500
                    setKeyFrameValue(keyFrame.bKeyFrame)
                }
            }
        }
        Item {
            Layout.fillHeight: true;
        }
    }
}
