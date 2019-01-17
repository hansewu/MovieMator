
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Item {
    width: 300
    height: 250
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())


    function setKeyFrameValue(bKeyFrame)
     {
         console.log("wave ui.qml")
         console.log("waveSlider.value")
         console.log(waveSlider.value)

         console.log("speedSlider.value")
         console.log(speedSlider.value)

         console.log("deformXCheckBox.checked")
         console.log(deformXCheckBox.checked)

         console.log("deformYCheckBox.checked")
         console.log(deformYCheckBox.checked)


         var nFrame = keyFrame.getCurrentFrame();
         if(bKeyFrame)
         {

             var contrastValue = waveSlider.value;
             filter.setKeyFrameParaValue(nFrame, "wave", contrastValue.toString())

             contrastValue = speedSlider.value;
             filter.setKeyFrameParaValue(nFrame, "speed", contrastValue.toString())

             contrastValue = deformXCheckBox.checked;
             if(contrastValue)
                filter.setKeyFrameParaValue(nFrame, "deformX", "1");
             else
                filter.setKeyFrameParaValue(nFrame, "deformX", "0");

             contrastValue = deformYCheckBox.checked;
             if(contrastValue)
                filter.setKeyFrameParaValue(nFrame, "deformY", "1");
             else
                filter.setKeyFrameParaValue(nFrame, "deformY", "0");

             filter.combineAllKeyFramePara();
         }
         else
         {
             //Todo, delete the keyframe date of the currentframe
             filter.removeKeyFrameParaValue(nFrame);
             if(!filter.getKeyFrameNumber())
             {
                filter.anim_set("wave","")
                filter.anim_set("speed","")
                filter.anim_set("deformX","")
                filter.anim_set("deformY","")
                
             }
             filter.set("wave", waveSlider.value);
             filter.set("speed", speedSlider.value);
             filter.set("deformX", deformXCheckBox.checked);
             filter.set("deformY",  deformYCheckBox.checked);
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
        if(keyFrameCount > 0)
        {
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
        else
        {
            filter.set("wave", waveSlider.value);
            filter.set("speed", speedSlider.value);
            filter.set("deformX", deformXCheckBox.checked);
            filter.set("deformY",  deformYCheckBox.checked);
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
                var waveValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "wave");
                if(waveValue != -1.0)
                {

                    waveSlider.value = waveValue;

                }

                waveValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "speed");
                if(waveValue != -1.0)
                {

                   speedSlider.value = waveValue;

                }

                waveValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "deformX");
                if(waveValue == 1)
                {

                   deformXCheckBox.checked = true;

                }
                else
                    deformXCheckBox.checked = false;

                waveValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "deformY");
                if(waveValue == 1)
                {

                   deformYCheckBox.checked = true;

                }
                else
                    deformYCheckBox.checked = false;

            }
        }

        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        Preset {
            id: preset
            enabled: bEnableControls
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
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: waveSlider
            enabled: bEnableControls
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
               // setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked:{
                 waveSlider.value = 10
                 setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {
            text: qsTr('Speed')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: speedSlider
            enabled: bEnableControls
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
              //  setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                speedSlider.value = 5
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {}
        CheckBox {
            id: deformXCheckBox
            enabled: bEnableControls
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
                        if(checked)
                            filter.setKeyFrameParaValue(nFrame, "deformX", "1")
                        else
                            filter.setKeyFrameParaValue(nFrame, "deformX", "0")
                        filter.combineAllKeyFramePara()

                    }
                    else
                        filter.set('deformX', checked)
                  // setKeyFrameValue(keyFrame.bKeyFrame)
                }
            }

            style: CheckBoxStyle {
                        label: Text {
                            color: bEnableControls?'#ffffff': '#828282'
                            text: qsTr('Deform horizontally?')
                        }
            }
        }

        Label {}
        CheckBox {
            id: deformYCheckBox
            enabled: bEnableControls
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
                        if(checked)
                            filter.setKeyFrameParaValue(nFrame, "deformY", "1")
                        else
                            filter.setKeyFrameParaValue(nFrame, "deformY", "0")
                        filter.combineAllKeyFramePara()

                    }
                    else
                        filter.set('deformY', checked)
                   // setKeyFrameValue(keyFrame.bKeyFrame)
                }
            }
            style: CheckBoxStyle {
                        label: Text {
                            color: bEnableControls?'#ffffff': '#828282'
                            text: qsTr('Deform vertically?')
                        }
            }

        }

        Item {
            Layout.fillHeight: true
        }
    }
}
