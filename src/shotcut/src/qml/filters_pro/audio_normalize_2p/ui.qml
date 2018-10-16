
import QtQuick 2.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())

    function setStatus( inProgress ) {
        if (inProgress) {
            status.text = qsTr('Analyzing...')
        }
        else if (filter.get("results").length > 0 ) {
            status.text = qsTr('Analysis complete.')
        }
        else
        {
            status.text = qsTr('Click "Analyze" to use this filter.')
        }
    }

    function setKeyFrameValue(bKeyFrame)
    {
        var nFrame = keyFrame.getCurrentFrame();
        var contrastValue = programSlider.value;
        if(bKeyFrame)
        {

            filter.setKeyFrameParaValue(nFrame,"program", contrastValue.toString() );
            filter.combineAllKeyFramePara();
        }
        else
        {
            //Todo, delete the keyframe date of the currentframe
            filter.removeKeyFrameParaValue(nFrame);
            if(!filter.getKeyFrameNumber())
            {
                filter.anim_set("program","")
            }

            filter.set("program", contrastValue);
        }
    }

    Connections {
        target: filter
        onAnalyzeFinished: {
            setStatus(false)
            button.enabled = true
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
                  var sliderValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "program");
                  if(gammaKeyValue != -1.0)
                  {
                      programSlider.value = sliderValue
                  }

             }
        }
        RowLayout {
            Label {
                text: qsTr('Target Loudness')
                color: bEnableControls?'#ffffff': '#828282'
            }
            SliderSpinner {
                id: programSlider
                enabled: bEnableControls
                minimumValue: -50.0
                maximumValue: -10.0
                decimals: 1
                suffix: ' LUFS'
                spinnerWidth: 100
                value: filter.getDouble('program')
                onValueChanged: {
                   // filter.set('program', value)
                    setKeyFrameValue(keyFrame.bKeyFrame)
                }
            }
            UndoButton {
                enabled: bEnableControls
                onClicked: {
                    programSlider.value = -23.0
                    setKeyFrameValue(keyFrame.bKeyFrame)
                }
            }
        }

        RowLayout {
            Button {
                id: button
                enabled: bEnableControls
                text: qsTr('Analyze')

                onClicked: {
                    button.enabled = false
                    filter.analyze(true);
                }
            }
            Label {
                id: status
                color: bEnableControls?'#ffffff': '#828282'
                Component.onCompleted: {
                    setStatus(false)
                }
            }
        }

        Item {
            Layout.fillHeight: true;
        }
    }
}
