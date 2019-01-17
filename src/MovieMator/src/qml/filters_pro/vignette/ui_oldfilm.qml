

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Item {
    width: 300
    height: 250
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())

    Component.onCompleted: {
        if (filter.isNew)
            filter.savePreset(preset.parameters)

        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-radius");
        console.log("1... keyFrameCount:")
        console.log(keyFrameCount)
        if(keyFrameCount > 0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-radius");
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-radius");
                filter.setKeyFrameParaValue(nFrame, "radius", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-smooth");
                filter.setKeyFrameParaValue(nFrame, "smooth", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-opacity");
                filter.setKeyFrameParaValue(nFrame, "opacity", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-mode");
                filter.setKeyFrameParaValue(nFrame, "mode", keyValue)
            }

            console.log("2.... combineAllKeyFramePara will be called")
            filter.combineAllKeyFramePara();
        }
        else
        {
            filter.set("radius", radiusSlider.value / 100);
            filter.set("smooth", smoothSlider.value/100);
            filter.set("opacity", 1.0 - opacitySlider.value / 100);

            filter.set("mode",  modeCheckBox.checked);
        }
    }

    function setKeyFrameValue(bKeyFrame)
    {
        var nFrame = keyFrame.getCurrentFrame();
        if(bKeyFrame)
        {

            filter.setKeyFrameParaValue(nFrame, "radius", (radiusSlider.value / 100).toString())
            filter.setKeyFrameParaValue(nFrame, "smooth", (smoothSlider.value/100).toString())
            filter.setKeyFrameParaValue(nFrame, "opacity", (1.0 - opacitySlider.value / 100).toString())
            filter.setKeyFrameParaValue(nFrame, "mode", modeCheckBox.checked.toString())

            console.log("7.... combineAllKeyFramePara will be called")
            filter.combineAllKeyFramePara();
        }
        else
        {
            //Todo, delete the keyframe date of the currentframe
            filter.removeKeyFrameParaValue(nFrame);
            if(!filter.getKeyFrameNumber())
             {
                filter.anim_set("radius","")
                filter.anim_set("smooth","")
                filter.anim_set("opacity","")
                filter.anim_set("mode","")
                
             }
            filter.set("radius", radiusSlider.value / 100);
            filter.set("smooth", smoothSlider.value/100);
            filter.set("opacity", 1.0 - opacitySlider.value / 100);

            filter.set("mode",  modeCheckBox.checked);
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
                var vignetteValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "radius");
                if(vignetteValue != -1.0)
                {
                    console.log("3... set radiusSlider value:")
                    console.log(vignetteValue)
                    radiusSlider.value = vignetteValue* 100.0
                }

                vignetteValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "smooth");
                if(vignetteValue != -1.0)
                {
                    console.log("3... set smoothSlider value:")
                    console.log(vignetteValue)
                    smoothSlider.value = vignetteValue* 100.0
                }

                vignetteValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "opacity");
                if(vignetteValue != -1.0)
                {
                    console.log("5... set opacitySlider value:")
                    console.log(vignetteValue)
                     opacitySlider.value = 1-vignetteValue* 100.0
                }

                vignetteValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "mode");
                if(vignetteValue != -1.0)
                {
                    console.log("6... set modeSlider value:")
                    console.log(vignetteValue)
                     modeCheckBox.checked = vignetteValue
                }
            }
        }

        Label{
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        Preset {
            id: preset
            enabled: bEnableControls
            Layout.columnSpan: 2
            parameters: ['radius', 'smooth', 'opacity', 'mode']
            onPresetSelected: {
                radiusSlider.value = filter.getDouble('radius') * 100
                smoothSlider.value = filter.getDouble('smooth') * 100
                opacitySlider.value = (1.0 - filter.getDouble('opacity')) * 100
                modeCheckBox.checked = filter.get('mode') === '1'
            }
        }

        Label {
            text: qsTr('Radius')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: radiusSlider
            enabled: bEnableControls
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: filter.getDouble('radius') * 100
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "radius", (value / 100).toString())
                    filter.combineAllKeyFramePara()

                }
                else
                    filter.set('radius', value / 100)

              //  setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                radiusSlider.value = 50
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {
            text: qsTr('Feathering')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: smoothSlider
            enabled: bEnableControls
            minimumValue: 0
            maximumValue: 500
            suffix: ' %'
            value: filter.getDouble('smooth') * 100
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "smooth", (value / 100).toString())
                    filter.combineAllKeyFramePara()

                }
                else
                    filter.set('smooth', value / 100)
              //  setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                smoothSlider.value = 80
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {}
        CheckBox {
            id: modeCheckBox
            enabled: bEnableControls
//            text: qsTr('Non-linear feathering')
            Layout.columnSpan: 2
            checked: filter.get('mode') === '1'
            property bool isReady: false
            Component.onCompleted: isReady = true
            onClicked: {
                if (isReady)
                {
                    if(keyFrame.bKeyFrame)
                    {
                        var nFrame = keyFrame.getCurrentFrame();
                        filter.setKeyFrameParaValue(nFrame, "mode", checked.toString())
                        filter.combineAllKeyFramePara()

                    }
                    else
                        filter.set('mode', checked)
                   // setKeyFrameValue(keyFrame.bKeyFrame)
                }
            }
            style: CheckBoxStyle {
                        label: Text {
                            color: "white"
                            text: qsTr('Non-linear feathering')
                        }
            }
        }

        Label {
            text: qsTr('Opacity')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: opacitySlider
            enabled: bEnableControls
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            value: (1.0 - filter.getDouble('opacity')) * 100
            onValueChanged:{
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "opacity", (1.0 - opacitySlider.value / 100).toString())
                    filter.combineAllKeyFramePara()

                }
                else
                    filter.set('opacity', 1.0 - value / 100)
              //  setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                opacitySlider.value = 100
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
