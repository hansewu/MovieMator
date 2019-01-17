
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property var defaultParameters: ['lift_r', 'lift_g', 'lift_b', 'gamma_r', 'gamma_g', 'gamma_b', 'gain_r', 'gain_g', 'gain_b']
    property var gammaFactor: 2.0
    property var gainFactor: 4.0
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())

    width: 300
    height: 250
    
    function setKeyFrameValue(bKeyFrame)
    {
        console.log(liftwheel.color)
        console.log(gammawheel.color)
        console.log(gainwheel.color)
        var nFrame = keyFrame.getCurrentFrame();
        if(bKeyFrame)
        {
            console.log("1.......")

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
            console.log("2......")
            filter.removeKeyFrameParaValue(nFrame);

            if(!filter.getKeyFrameNumber())
            {
                filter.anim_set("lift_r","")
                filter.anim_set("lift_g","")
                filter.anim_set("lift_b","")
                filter.anim_set("gamma_r","")
                filter.anim_set("gamma_g","")
                filter.anim_set("gamma_b","")
                filter.anim_set("gain_r","")
                filter.anim_set("gain_g","")
                filter.anim_set("gain_b","")
                
            }
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
        if(keyFrameCount>0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-lift_r");
                var liftRkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "anim-lift_r");
                var liftGkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "anim-lift_g");
                var liftBkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "anim-lift_b");

                var gammaRkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "anim-gamma_r");
                var gammaGkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "anim-gamma_g");
                var gammaBkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "anim-gamma_b");

                var gainRkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "anim-gain_r");
                var gainGkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "anim-gain_g");
                var gainBkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "anim-gain_b");

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
        else
        {
            console.log("1.... loadwheels is called")
            console.log(filter.getDouble("lift_r"))
            console.log(filter.getDouble("lift_g"))
            console.log(filter.getDouble("lift_b"))
            console.log(filter.getDouble("gamma_r"))
            console.log(filter.getDouble("gamma_g"))
            console.log(filter.getDouble("gamma_b"))
            console.log(filter.getDouble("gain_r"))
            console.log(filter.getDouble("gain_g"))
            console.log(filter.getDouble("gain_b"))


            var liftColor = Qt.rgba( filter.getDouble("lift_r"),
                                       filter.getDouble("lift_g"),
                                       filter.getDouble("lift_b"),
                                       1.0 )
            console.log(liftColor)



            var gammaColor = Qt.rgba( filter.getDouble("gamma_r") / gammaFactor,
                                        filter.getDouble("gamma_g") / gammaFactor,
                                        filter.getDouble("gamma_b") / gammaFactor,
                                        1.0 )
            console.log(gammaColor)



           var gainColor = Qt.rgba( filter.getDouble("gain_r") / gainFactor,
                                       filter.getDouble("gain_g") / gainFactor,
                                       filter.getDouble("gain_b") / gainFactor,
                                       1.0 )
            console.log(gainColor)

           liftwheel.color = liftColor;
           gammawheel.color = gammaColor;
           gainwheel.color = gainColor;




        }
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
            console.log("8.... new color filter color:")
            console.log(filter.getDouble("lift_r"))
            console.log(filter.getDouble("lift_g"))
            console.log(filter.getDouble("lift_b"))
            console.log(filter.getDouble("gamma_r"))
            console.log(filter.getDouble("gamma_g"))
            console.log(filter.getDouble("gamma_b"))
            console.log(filter.getDouble("gain_r"))
            console.log(filter.getDouble("gain_g"))
            console.log(filter.getDouble("gain_b"))
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
                     gainwheel.color = Qt.rgba(rValue/gainFactor, gValue/gainFactor, bValue, 1.0 )
                 }

             }
         }
        // Row 1
        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignLeft
            color: bEnableControls?'#ffffff': '#828282'
        }
        Preset {
            enabled: bEnableControls
            Layout.columnSpan: 5
            parameters: defaultParameters
            onPresetSelected: {
                loadWheels()
            }
        }

        // Row 2
        Label {
            text: qsTr('Shadows')
            color: bEnableControls?'#ffffff': '#828282'

        }
        UndoButton {
            
            enabled: bEnableControls
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                liftwheel.color = Qt.rgba( 0.0, 0.0, 0.0, 1.0 )
                setKeyFrameValue(keyFrame.bKeyFrame)

            }
        }
        Label {
            text: qsTr('Midtones')
            color: bEnableControls?'#ffffff': '#828282'

        }
        UndoButton {
            enabled: bEnableControls
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                gammawheel.color = Qt.rgba( 1.0 / gammaFactor, 1.0 / gammaFactor, 1.0 / gammaFactor, 1.0 )
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        Label {
            text: qsTr('Highlights')
            color: bEnableControls?'#ffffff': '#828282'
        }
        UndoButton {
            enabled: bEnableControls
            Layout.alignment: Qt.AlignCenter
            onClicked:{
                gainwheel.color = Qt.rgba( 1.0 / gainFactor, 1.0 / gainFactor, 1.0 / gainFactor, 1.0 )
                setKeyFrameValue(keyFrame.bKeyFrame)
             }
        }
        
        // Row 3
        ColorWheelItem {
            enabled: bEnableControls
            id: liftwheel
            Layout.columnSpan: 2
            implicitWidth: parent.width / 3 - parent.columnSpacing
            implicitHeight: implicitWidth / 1.1
            Layout.alignment : Qt.AlignLeft | Qt.AlignTop
            Layout.minimumHeight: 75
            Layout.maximumHeight: 300
            onColorChanged: {
                setKeyFrameValue(keyFrame.bKeyFrame)

            }
        }
        ColorWheelItem {
            enabled: bEnableControls
            id: gammawheel
            Layout.columnSpan: 2
            implicitWidth: parent.width / 3 - parent.columnSpacing
            implicitHeight: implicitWidth / 1.1
            Layout.alignment : Qt.AlignLeft | Qt.AlignTop
            Layout.minimumHeight: 75
            Layout.maximumHeight: 300
            onColorChanged: {
                setKeyFrameValue(keyFrame.bKeyFrame)

            }
        }
        ColorWheelItem {
            enabled: bEnableControls
            id: gainwheel
            Layout.columnSpan: 2
            implicitWidth: parent.width / 3 - parent.columnSpacing
            implicitHeight: implicitWidth / 1.1
            Layout.alignment : Qt.AlignLeft | Qt.AlignTop
            Layout.minimumHeight: 75
            Layout.maximumHeight: 300
            onColorChanged: {
                setKeyFrameValue(keyFrame.bKeyFrame)

            }
        }

        Item { Layout.fillHeight: true }
    }
}
