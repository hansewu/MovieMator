
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Item {
    width: 300
    height: 250

    function setKeyFrameValue(bKeyFrame)
     {
         var nFrame = keyFrame.getCurrentFrame();
         if(bKeyFrame)
         {

             var contrastValue = waveSlider.value;
             filter.setKeyFrameParaValue(nFrame, "wave", contrastValue.toString())

             contrastValue = speedSlider.value;
             filter.setKeyFrameParaValue(nFrame, "speed", contrastValue.toString())

             contrastValue = deformXCheckBox.value;
             filter.setKeyFrameParaValue(nFrame, "deformX", contrastValue.toString())

             contrastValue = deformYCheckBox.value;
             filter.setKeyFrameParaValue(nFrame, "deformY", contrastValue.toString())

             filter.combineAllKeyFramePara();
         }
         else
         {
             //Todo, delete the keyframe date of the currentframe
             filter.removeKeyFrameParaValue(nFrame);

             filter.set("wave", waveSlider.value);
             filter.set("speed", speedSlider.value);
             filter.set("deformX", deformXCheckBox.value);
             filter.set("deformY",  deformYCheckBox.value);
         }
     }


    Component.onCompleted: {
        if (filter.isNew) {
            filter.set('wave', 10)
            filter.set('speed', 5)
            filter.set('deformX', 1)
            filter.set('deformY', 1)
            filter.savePreset(preset.parameters)
        }

        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-wave");
        var index=0
        for(index=0; index<keyFrameCount;index++)
        {
            var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-wave");
            var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-wave");
            filter.setKeyFrameParaValue(nFrame, "wave", keyValue)

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-speed");
            filter.setKeyFrameParaValue(nFrame, "speed", keyValue)

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-deformX");
            filter.setKeyFrameParaValue(nFrame, "deformX", keyValue)

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-deformY");
            filter.setKeyFrameParaValue(nFrame, "deformY", keyValue)

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
                 gammaKeyValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "gamma_r");
                if(gammaKeyValue != -1.0)
                {
            //    gammaValue = (1.0 - contrastValue/100.0) * gammaFactor;
                    console.log("gammaValue is:")
                    console.log(gammaKeyValue)
                    console.log("constrast slider value is:")
                    console.log((1.0 - gammaKeyValue/gammaFactor) * 100.0)
                  //  bReadKeyValue = true;
                    contrastSlider.value = (1.0 - gammaKeyValue/gammaFactor) * 100.0

                }

            }
        }

        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color:'#ffffff'
        }
        Preset {
            id: preset
            Layout.columnSpan: 2
            parameters: ['wave', 'speed', 'deformX', 'deformX']
            onPresetSelected: {
                waveSlider.value = filter.getDouble('wave')
                speedSlider.value = filter.getDouble('speed')
                deformXCheckBox.checked = filter.get('deformX') === '1'
                deformYCheckBox.checked = filter.get('deformY') === '1'
            }
        }

        Label {
            text: qsTr('Amplitude')
            Layout.alignment: Qt.AlignRight
            color:'#ffffff'
        }
        SliderSpinner {
            id: waveSlider
            minimumValue: 1
            maximumValue: 500
            value: filter.getDouble('wave')
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "wave", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('wave', value)
            }
        }
        UndoButton {
            onClicked: waveSlider.value = 10
        }

        Label {
            text: qsTr('Speed')
            Layout.alignment: Qt.AlignRight
            color:'#ffffff'
        }
        SliderSpinner {
            id: speedSlider
            minimumValue: 0
            maximumValue: 1000
            value: filter.getDouble('speed')
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "speed", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('speed', value)
            }
        }
        UndoButton {
            onClicked: speedSlider.value = 5
        }

        Label {}
        CheckBox {
            id: deformXCheckBox
//            text: qsTr('Deform horizontally?')
            Layout.columnSpan: 2
            checked: filter.get('deformX') === '1'
            property bool isReady: false
            Component.onCompleted: isReady = true
            onClicked: {
                if (isReady)
                {
                    if(keyFrame.bKeyFrame)
                    {
                        var nFrame = keyFrame.getCurrentFrame();
                        filter.setKeyFrameParaValue(nFrame, "deformX", checked.toString())
                        filter.combineAllKeyFramePara()
                    }
                    else
                        filter.set('deformX', checked)
                }
            }

            style: CheckBoxStyle {
                        label: Text {
                            color: "white"
                            text: qsTr('Deform horizontally?')
                        }
            }
        }

        Label {}
        CheckBox {
            id: deformYCheckBox
         //   text: qsTr('Deform vertically?')
            Layout.columnSpan: 2
            checked: filter.get('deformY') === '1'
            property bool isReady: false
            Component.onCompleted: isReady = true
            onClicked: {
                if (isReady)
                {
                    if(keyFrame.bKeyFrame)
                    {
                        var nFrame = keyFrame.getCurrentFrame();
                        filter.setKeyFrameParaValue(nFrame, "deformY", checked.toString())
                        filter.combineAllKeyFramePara()
                    }
                    else
                        filter.set('deformY', checked)
                }
            }
            style: CheckBoxStyle {
                        label: Text {
                            color: "white"
                            text: qsTr('Deform vertically?')
                        }
            }

        }

        Item {
            Layout.fillHeight: true
        }
    }
}
