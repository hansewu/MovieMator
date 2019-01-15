
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property var defaultParameters: ['lift_r', 'lift_g', 'lift_b', 'gamma_r', 'gamma_g', 'gamma_b', 'gain_r', 'gain_g', 'gain_b']
    property var gammaFactor: 2.0
    property var gainFactor: 4.0
    width: 300
    height: 250
    
    function setKeyFrameValue(bKeyFrame)
    {
        var nFrame = keyFrame.getCurrentFrame();
        if(bKeyFrame)
        {
            filter.setKeyFrameParaValue(nFrame, "lift_r", (liftwheel.red / 255.0).toString())
            filter.setKeyFrameParaValue(nFrame, "lift_g", (liftwheel.green / 255.0).toString())
            filter.setKeyFrameParaValue(nFrame, "lift_b", (liftwheel.blue / 255.0).toString())

            filter.setKeyFrameParaValue(nFrame, "gamma_r", ((gammawheel.red / 255.0) * gammaFactor).toString())
            filter.setKeyFrameParaValue(nFrame, "gamma_g", ((gammawheel.green / 255.0) * gammaFactor).toString())
            filter.setKeyFrameParaValue(nFrame, "gamma_b", ((gammawheel.blue / 255.0) * gammaFactor).toString())

            filter.setKeyFrameParaValue(nFrame, "gain_r", ((gainwheel.red / 255.0) * gainFactor).toString())
            filter.setKeyFrameParaValue(nFrame, "gain_g", ((gainwheel.green / 255.0) * gainFactor).toString())
            filter.setKeyFrameParaValue(nFrame, "gain_b", ((gainwheel.blue / 255.0) * gainFactor).toString())

            filter.combineAllKeyFramePara();
        }
        else
        {
            //Todo, delete the keyframe date of the currentframe
            filter.removeKeyFrameParaValue(nFrame);

            filter.set("lift_r", liftwheel.red / 255.0 );
            filter.set("lift_g", liftwheel.green / 255.0 );
            filter.set("lift_b", liftwheel.blue / 255.0 );
            filter.set("gamma_r", (gammawheel.red / 255.0) * gammaFactor);
            filter.set("gamma_g", (gammawheel.green / 255.0) * gammaFactor);
            filter.set("gamma_b", (gammawheel.blue / 255.0) * gammaFactor);
            filter.set("gain_r", (gainwheel.red / 255.0) * gainFactor);
            filter.set("gain_g", (gainwheel.green / 255.0) * gainFactor);
            filter.set("gain_b", (gainwheel.blue / 255.0) * gainFactor);
        }
    }

    function loadWheels() {

        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-lift_r");
        var index=0
        for(index=0; index<keyFrameCount;index++)
        {
            var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-lift_r");
            var liftRkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-lift_r");
            var liftGkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-lift_g");
            var liftBkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-lift_b");

            var gammaRkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-gamma_r");
            var gammaGkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-gamma_g");
            var gammaBkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-gamma_b");

            var gainRkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-gain_r");
            var gainGkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-gain_g");
            var gainBkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-gain_b");

            filter.setKeyFrameParaValue(nFrame, "lift_r", liftRkeyValue.toString())
            filter.setKeyFrameParaValue(nFrame, "lift_g", liftGkeyValue.toString())
            filter.setKeyFrameParaValue(nFrame, "lift_b", liftBkeyValue.toString())

            filter.setKeyFrameParaValue(nFrame, "gamma_r", gammaRkeyValue.toString())
            filter.setKeyFrameParaValue(nFrame, "gamma_g", gammaGkeyValue.toString())
            filter.setKeyFrameParaValue(nFrame, "gamma_b", gammaBkeyValue.toString())

            filter.setKeyFrameParaValue(nFrame, "gain_r", gainRkeyValue.toString())
            filter.setKeyFrameParaValue(nFrame, "gain_g", gainGkeyValue.toString())
            filter.setKeyFrameParaValue(nFrame, "gain_b", gainBkeyValue.toString())

        }

        filter.combineAllKeyFramePara();

        liftwheel.color = Qt.rgba( filter.getKeyFrameOnProjectOnIndex(0, "anim-lift_r"),
                                   filter.getKeyFrameOnProjectOnIndex(0, "anim-lift_g"),
                                   filter.getKeyFrameOnProjectOnIndex(0, "anim-lift_b"),
                                   1.0 )
        gammawheel.color = Qt.rgba( filter.getKeyValueOnProjectOnIndex(0, "anim-gamma_r") / gammaFactor,
                                    filter.getKeyValueOnProjectOnIndex(0, "anim-gamma_g") / gammaFactor,
                                    filter.getKeyValueOnProjectOnIndex(0, "anim-gamma_b") / gammaFactor,
                                    1.0 )
        gainwheel.color = Qt.rgba( filter.getKeyValueOnProjectOnIndex(0, "anim-gain_r") / gainFactor,
                                   filter.getKeyValueOnProjectOnIndex(0, "anim-gain_g") / gainFactor,
                                   filter.getKeyValueOnProjectOnIndex(0, "anim-gain_b") / gainFactor,
                                   1.0 )
    }
    
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set("lift_r", 0.0);
            filter.set("lift_g", 0.0);
            filter.set("lift_b", 0.0);
            filter.set("gamma_r", 1.0);
            filter.set("gamma_g", 1.0);
            filter.set("gamma_b", 1.0);
            filter.set("gain_r", 1.0);
            filter.set("gain_g", 1.0);
            filter.set("gain_b", 1.0);
            filter.savePreset(defaultParameters)
        }
        loadWheels()
    }

    GridLayout {
        columns: 6
        anchors.fill: parent
        anchors.margins: 8

        KeyFrame{
             id: keyFrame
             Layout.columnSpan:6
        // 	currentPosition: filterDock.getCurrentPosition()
             onSetAsKeyFrame:
             {
                setKeyFrameValue(bKeyFrame)
             }

             onLoadKeyFrame:
             {
                 var rValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "lift_r") * 255.0;
                 var gValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "lift_g") * 255.0;
                 var bValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "lift_b") * 255.0;
                 if(rValue != -1.0)
                 {
                     liftwheel.color = Qt.rgba( rValue, gValue, bValue, 1.0 )
                 }


                 rValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "gamma_r") * 255.0;
                 gValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "gamma_g") * 255.0;
                 bValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "gamma_b") * 255.0;
                 if(rValue != -1.0)
                 {
                     gammawheel.color = Qt.rgba( rValue/gammaFactor, gValue/gammaFactor, bValue/gammaFactor, 1.0 )
                 }

                 rValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "gain_r") * 255.0;
                 gValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "gain_r") * 255.0;
                 bValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "gain_r") * 255.0;
                 if(rValue != -1.0)
                 {
                     gainwheel.color = Qt.rgba(rValue/gainFactor, gValue/gainFactor, bValuev, 1.0 )
                 }

             }
         }
        // Row 1
        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
        }
        Preset {
            Layout.columnSpan: 5
            parameters: defaultParameters
            onPresetSelected: {
                loadWheels()
            }
        }

        // Row 2
        Label {
            text: qsTr('Shadows')
            color: '#ffffff'

        }
        UndoButton {
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                liftwheel.color = Qt.rgba( 0.0, 0.0, 0.0, 1.0 )
                setKeyFrameValue(keyFrame.bKeyFrame)

            }
        }
        Label {
            text: qsTr('Midtones')
            color: '#ffffff'

        }
        UndoButton {
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                gammawheel.color = Qt.rgba( 1.0 / gammaFactor, 1.0 / gammaFactor, 1.0 / gammaFactor, 1.0 )
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        Label {
            text: qsTr('Highlights')
            color: '#ffffff'
        }
        UndoButton {
            Layout.alignment: Qt.AlignCenter
            onClicked:{
                gainwheel.color = Qt.rgba( 1.0 / gainFactor, 1.0 / gainFactor, 1.0 / gainFactor, 1.0 )
                setKeyFrameValue(keyFrame.bKeyFrame)
             }
        }
        
        // Row 3
        ColorWheelItem {
            id: liftwheel
            Layout.columnSpan: 2
            implicitWidth: parent.width / 3 - parent.columnSpacing
            implicitHeight: implicitWidth / 1.1
            Layout.alignment : Qt.AlignLeft | Qt.AlignTop
            Layout.minimumHeight: 75
            Layout.maximumHeight: 300
            onColorChanged: {
                var nFrame = keyFrame.getCurrentFrame();
                if(keyFrame.bKeyFrame)
                {
                    filter.setKeyFrameParaValue(nFrame, "lift_r", (liftwheel.red / 255.0).toString())
                    filter.setKeyFrameParaValue(nFrame, "lift_g", (liftwheel.green / 255.0).toString())
                    filter.setKeyFrameParaValue(nFrame, "lift_b", (liftwheel.blue / 255.0).toString())

                    filter.combineAllKeyFramePara()
                }
                else
                {
                    filter.set("lift_r", liftwheel.red / 255.0 );
                    filter.set("lift_g", liftwheel.green / 255.0 );
                    filter.set("lift_b", liftwheel.blue / 255.0 );

                }

            }
        }
        ColorWheelItem {
            id: gammawheel
            Layout.columnSpan: 2
            implicitWidth: parent.width / 3 - parent.columnSpacing
            implicitHeight: implicitWidth / 1.1
            Layout.alignment : Qt.AlignLeft | Qt.AlignTop
            Layout.minimumHeight: 75
            Layout.maximumHeight: 300
            onColorChanged: {
                var nFrame = keyFrame.getCurrentFrame();
                if(keyFrame.bKeyFrame)
                {
                    filter.setKeyFrameParaValue(nFrame, "gamma_r", ((gammawheel.red / 255.0) * gammaFactor).toString())
                    filter.setKeyFrameParaValue(nFrame, "gamma_g", ((gammawheel.green / 255.0) * gammaFactor).toString())
                    filter.setKeyFrameParaValue(nFrame, "gamma_b", ((gammawheel.blue / 255.0) * gammaFactor).toString())
                    filter.combineAllKeyFramePara()
                }
                else
                {
                    filter.set("gamma_r", (gammawheel.red / 255.0) * gammaFactor);
                    filter.set("gamma_g", (gammawheel.green / 255.0) * gammaFactor);
                    filter.set("gamma_b", (gammawheel.blue / 255.0) * gammaFactor);
                }

            }
        }
        ColorWheelItem {
            id: gainwheel
            Layout.columnSpan: 2
            implicitWidth: parent.width / 3 - parent.columnSpacing
            implicitHeight: implicitWidth / 1.1
            Layout.alignment : Qt.AlignLeft | Qt.AlignTop
            Layout.minimumHeight: 75
            Layout.maximumHeight: 300
            onColorChanged: {
                var nFrame = keyFrame.getCurrentFrame();
                if(keyFrame.bKeyFrame)
                {
                    filter.setKeyFrameParaValue(nFrame, "gain_r", ((gainwheel.red / 255.0) * gainFactor).toString())
                    filter.setKeyFrameParaValue(nFrame, "gain_g", ((gainwheel.green / 255.0) * gainFactor).toString())
                    filter.setKeyFrameParaValue(nFrame, "gain_b", ((gainwheel.blue / 255.0) * gainFactor).toString())
                    filter.combineAllKeyFramePara()
                }
                else
                {
                    filter.set("gain_r", (gainwheel.red / 255.0) * gainFactor);
                    filter.set("gain_g", (gainwheel.green / 255.0) * gainFactor);
                    filter.set("gain_b", (gainwheel.blue / 255.0) * gainFactor);
                }

            }
        }

        Item { Layout.fillHeight: true }
    }
}
