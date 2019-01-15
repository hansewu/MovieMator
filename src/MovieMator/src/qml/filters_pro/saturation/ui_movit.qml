
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())

    width: 300
    height: 250
    property string saturationParameter: 'saturation'
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set(saturationParameter, 1.0)
            slider.value = 100
        }

        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-saturation");
        if(keyFrameCount > 0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-saturation");
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-saturation");

                filter.setKeyFrameParaValue(nFrame, saturationParameter, keyValue )
            }
            filter.combineAllKeyFramePara();
        }
    }

    function setKeyFrameValue(bKeyFrame)
    {
        var nFrame = keyFrame.getCurrentFrame();
        if(bKeyFrame)
        {

            var saturationValue = slider.value;
            filter.setKeyFrameParaValue(nFrame, saturationParameter, (saturationValue/100).toString() )
            filter.combineAllKeyFramePara();
        }
        else
        {
            //Todo, delete the keyframe date of the currentframe
            filter.removeKeyFrameParaValue(nFrame);
            if(!filter.getKeyFrameNumber())
            {
                filter.anim_set(saturationParameter,"")
            }
            filter.set(saturationParameter, slider.value/100);
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
                var saturationValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, saturationParameter);
                if(saturationValue != -1.0)
                {
                    slider.value = saturationValue * 100.0
                }

            }
        }

        RowLayout {
            anchors.fill: parent

            Label {
                text: qsTr('Saturation')
                color: bEnableControls?'#ffffff': '#828282'
            }
            SliderSpinner {
                id: slider
                enabled: bEnableControls
                minimumValue: 0
                maximumValue: 300
                suffix: ' %'
                value: filter.getDouble(saturationParameter) * 100
                onValueChanged: {
                   setKeyFrameValue(keyFrame.bKeyFrame)

                }
            }
            UndoButton {
                enabled: bEnableControls
                onClicked: {
                    slider.value = 100
                    setKeyFrameValue(keyFrame.bKeyFrame)
                }
            }
        }
        Item {
            Layout.fillHeight: true;
        }
    }
}
