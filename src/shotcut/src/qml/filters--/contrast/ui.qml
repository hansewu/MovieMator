
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property var defaultParameters: ['gamma_r', 'gamma_g', 'gamma_b', 'gain_r', 'gain_g', 'gain_b']
    property double gammaFactor: 2.0
    property double gainFactor: 2.0
    property double gammaKeyValue: 0
    property string animValue:""
 //   property bool bReadKeyValue: falses
    width: 300
    height: 250
    
    function setControls() {
        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-gamma_r");
        var index=0
        for(index=0; index<keyFrameCount;index++)
        {
            var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-gamma_r");
            var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-gamma_r");
            var sliderValue = keyValue/gammaFactor*100.0;
            var gammaValue = (1.0 - sliderValue/100.0) * gammaFactor;
            var gainValue = sliderValue/100.0 * gainFactor;

            filter.setKeyFrameParaValue(nFrame,"gamma_r", gammaValue.toString() );
            filter.setKeyFrameParaValue(nFrame,"gamma_g", gammaValue.toString() )
            filter.setKeyFrameParaValue(nFrame,"gamma_b", gammaValue.toString() )

            filter.setKeyFrameParaValue(nFrame,"gain_r", gainValue.toString() )
            filter.setKeyFrameParaValue(nFrame,"gain_g", gainValue.toString() )
            filter.setKeyFrameParaValue(nFrame,"gain_b", gainValue.toString() )

        }
        filter.combineAllKeyFramePara();

        contrastSlider.value = filter.getKeyValueOnProjectOnIndex(0, "anim-gamma_r") / gammaFactor * 100.0
    }

    function setKeyFrameValue(bKeyFrame)
    {
        var nFrame = keyFrame.getCurrentFrame();
        var contrastValue = contrastSlider.value;
        var gammaValue = (1.0 - contrastValue/100.0) * gammaFactor;
        var gainValue = contrastValue/100.0 * gainFactor;
        if(bKeyFrame)
        {
            filter.setKeyFrameParaValue(nFrame,"gamma_r", gammaValue.toString() );
            filter.setKeyFrameParaValue(nFrame,"gamma_g", gammaValue.toString() )
            filter.setKeyFrameParaValue(nFrame,"gamma_b", gammaValue.toString() )

            filter.setKeyFrameParaValue(nFrame,"gain_r", gainValue.toString() )
            filter.setKeyFrameParaValue(nFrame,"gain_g", gainValue.toString() )
            filter.setKeyFrameParaValue(nFrame,"gain_b", gainValue.toString() )

            filter.combineAllKeyFramePara();
        }
        else
        {
            //Todo, delete the keyframe date of the currentframe
            filter.removeKeyFrameParaValue(nFrame);

            filter.set("gamma_r", gammaValue);
            filter.set("gamma_g", gammaValue);
            filter.set("gamma_b", gammaValue);

            filter.set("gain_r",  gainValue);
            filter.set("gain_g",  gainValue);
            filter.set("gain_b",  gainValue);

        }
    }
    
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set("gamma_r", 1.0);
            filter.set("gamma_g", 1.0);
            filter.set("gamma_b", 1.0);
            filter.set("gain_r", 1.0);
            filter.set("gain_g", 1.0);	
            filter.set("gain_b", 1.0);
            filter.savePreset(defaultParameters)
        }
        setControls()
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
            color: '#ffffff'
        }
        Preset {
            Layout.columnSpan: 2
            parameters: defaultParameters
            onPresetSelected: {
                setControls()
            }
        }

        Label {
            text: qsTr('Contrast')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: contrastSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            spinnerWidth: 80
            suffix: ' %'
            onValueChanged: {
                //var v = value / 100.0
              //  if(bReadKeyValue)
                setKeyFrameValue(keyFrame.bKeyFrame)

            }
        }
        UndoButton {
            onClicked: contrastSlider.value = 50
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
