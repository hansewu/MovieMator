
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Item {
    property var defaultParameters: ['left', 'right', 'top', 'bottom', 'center', 'center_bias']
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())

    width: 300
    height: 250
    
    function setEnabled() {
        if (centerCheckBox.checked)//filter.get('center') == 1) 
        {
            console.log('center == 1')
            console.log(bEnableControls)
      //      biasslider.enabled = true
      //      biasundo.enabled = true 
       //     topslider.enabled = false 
       //     topundo.enabled = false
       //     bottomslider.enabled = false
        //    bottomundo.enabled = false
       //     leftslider.enabled = false
        //    leftundo.enabled = false
        //    rightslider.enabled = false
        //    rightundo.enabled = false
        } else {
       //     console.log('center == 0')
       //     console.log(bEnableControls)
        //    biasslider.enabled = false
        //    biasundo.enabled = false
        //    topslider.enabled = true 
        //    topundo.enabled = true 
        //    bottomslider.enabled = true 
        //    bottomundo.enabled = true 
        //    leftslider.enabled = true 
        //    leftundo.enabled = true 
         //   rightslider.enabled = true 
         //   rightundo.enabled = true 
        }
    }

    function setKeyFrameValue(bKeyFrame)
     {
   //      bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())
         var nFrame = keyFrame.getCurrentFrame();
         console.log("1.....")
         console.log(nFrame)
         console.log(bKeyFrame)
         if(bKeyFrame)
         {

             var cropValue = centerCheckBox.checked;
             filter.setKeyFrameParaValue(nFrame,"center", cropValue.toString() );

             cropValue = biasslider.value
             filter.setKeyFrameParaValue(nFrame,"center_bias", cropValue.toString() )

             cropValue = topslider.value
             filter.setKeyFrameParaValue(nFrame,"top", cropValue.toString() )

             cropValue = bottomslider.value
             filter.setKeyFrameParaValue(nFrame,"bottom", cropValue.toString() )

             cropValue = leftslider.value
             filter.setKeyFrameParaValue(nFrame,"left", cropValue.toString() )

             cropValue = rightslider.value
             filter.setKeyFrameParaValue(nFrame,"right", cropValue.toString() )

             filter.combineAllKeyFramePara();
         }
         else
         {
             //Todo, delete the keyframe date of the currentframe
             console.log("2....")
             filter.removeKeyFrameParaValue(nFrame);
             if(!filter.getKeyFrameNumber())
             {
                filter.anim_set("center","")
                filter.anim_set("center_bias","")
                filter.anim_set("top","")
                filter.anim_set("bottom","")
                filter.anim_set("left","")
                filter.anim_set("right","")

             }

             filter.set("center", centerCheckBox.checked);
             filter.set("center_bias", biasslider.value);
             filter.set("top", topslider.value);

             filter.set("bottom",  bottomslider.value);
             filter.set("left",   leftslider.value);
             filter.set("right",  rightslider.value);

         }
     }
    
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set("center", 0);
            filter.set("center_bias", 0);
            filter.set("top", 0);
            filter.set("bottom", 0);
            filter.set("left", 0);
            filter.set("right", 0);
            centerCheckBox.checked = false
            filter.savePreset(defaultParameters)


        }


        

        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-center");
        if(keyFrameCount>0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
              var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-center");
              var centerkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-center");

              var centerBiaskeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-center_bias");
              var topkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-top");
              var bottomkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-bottom");
              var leftkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-left");
              var rightkeyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-right");

              filter.setKeyFrameParaValue(nFrame,"center", centerkeyValue.toString() );
              filter.setKeyFrameParaValue(nFrame,"center_bias", centerBiaskeyValue.toString() )
              filter.setKeyFrameParaValue(nFrame,"top", topkeyValue.toString() )
              filter.setKeyFrameParaValue(nFrame,"bottom", bottomkeyValue.toString() )
              filter.setKeyFrameParaValue(nFrame,"left", leftkeyValue.toString() )
              filter.setKeyFrameParaValue(nFrame,"right", rightkeyValue.toString() )

            }

            filter.combineAllKeyFramePara();

            centerCheckBox.checked = filter.getKeyFrameOnProjectOnIndex(0, "anim-center") == '1'
        }
        else
        {
            biasslider.value = +filter.get('center_bias')
            topslider.value = +filter.get('top')
            bottomslider.value = +filter.get('bottom')
            leftslider.value = +filter.get('left')
            rightslider.value = +filter.get('right')
            centerCheckBox.checked = filter.get('center') == '1'
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
                 var cropValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "center");
                 if(cropValue != -1.0)
                 {
                     centerCheckBox.checked = cropValue
                 }

                 cropValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "center_bias");
                 if(cropValue != -1.0)
                 {
                     biasslider.value = cropValue
                 }

                 cropValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "top");
                 if(cropValue != -1.0)
                 {
                     topslider.value = cropValue
                 }

                 cropValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "bottom");
                 if(cropValue != -1.0)
                 {
                     bottomslider.value = cropValue
                 }

                 cropValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "left");
                 if(cropValue != -1.0)
                 {
                     leftslider.value = cropValue
                 }

                 cropValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "right");
                 if(cropValue != -1.0)
                 {
                     rightslider.value = cropValue
                 }

             }
         }

        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        Preset {
            Layout.columnSpan: 2
            enabled: bEnableControls
            parameters: defaultParameters
            onPresetSelected: {
                centerCheckBox.checked = filter.get('center') == '1'
                biasslider.value = +filter.get('center_bias')
                topslider.value = +filter.get('top')
                bottomslider.value = +filter.get('bottom')
                leftslider.value = +filter.get('left')
                rightslider.value = +filter.get('right')
                console.log('Preset setEnabled is called')
                console.log(filter.get('center'))
                setEnabled()
            }
        }

        CheckBox {
            id: centerCheckBox
            enabled: bEnableControls
//            text: qsTr('Center')
            checked: filter.get('center') == '1'
            property bool isReady: false
            Component.onCompleted: isReady = true
            onClicked: {
                console.log('isReady:')
                console.log(isReady)
                if (isReady) {
                    console.log('CheckBox setEnabled is called')
                    setKeyFrameValue(keyFrame.bKeyFrame)
                    setEnabled()
                }
            }

            style: CheckBoxStyle {
                        label: Text {
                            color: bEnableControls?'#ffffff': '#828282'
                            text: qsTr('Center')
                        }
            }
        }
        Item {
            Layout.fillWidth: true;
        }
        UndoButton {
            
            onClicked: {
                centerCheckBox.checked = false

                setKeyFrameValue(keyFrame.bKeyFrame)

            }
        }

        Label {
            text: qsTr('Center bias')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: biasslider
            enabled: bEnableControls? centerCheckBox.checked: false
            minimumValue: -Math.max(profile.width, profile.height) / 2
            maximumValue: Math.max(profile.width, profile.height) / 2
            suffix: ' px'
            value: +filter.get('center_bias')
            onValueChanged: {
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            id: biasundo
            enabled: bEnableControls? centerCheckBox.checked: false
            onClicked: {
                biasslider.value = 0
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {
            text: qsTr('Top')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: topslider
            enabled: bEnableControls? (!centerCheckBox.checked): false 
            minimumValue: 0
            maximumValue: profile.height
            suffix: ' px'
            value: +filter.get('top')
            onValueChanged: {
                console.log('.......')
                console.log(bEnableControls)
                console.log(filter.get('center') == 0)
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            id: topundo
            enabled: bEnableControls? (!centerCheckBox.checked): false
            onClicked: {
                topslider.value = 0
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {
            text: qsTr('Bottom')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: bottomslider
            enabled: bEnableControls? (!centerCheckBox.checked): false
            minimumValue: 0
            maximumValue: profile.height
            suffix: ' px'
            value: +filter.get('bottom')
            onValueChanged:
            {
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            id: bottomundo
            enabled: bEnableControls? (!centerCheckBox.checked): false 
            onClicked: {
                bottomslider.value = 0
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {
            text: qsTr('Left')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: leftslider
            enabled: bEnableControls? (!centerCheckBox.checked): false 
            minimumValue: 0
            maximumValue: profile.width
            suffix: ' px'
            value: +filter.get('left')
            onValueChanged:
            {
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            id: leftundo
            enabled: bEnableControls? (!centerCheckBox.checked): false 
            onClicked: {
                leftslider.value = 0
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {
            text: qsTr('Right')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: rightslider
            enabled: bEnableControls? (!centerCheckBox.checked): false 
            minimumValue: 0
            maximumValue: profile.width
            suffix: ' px'
            value: +filter.get('right')
            onValueChanged: {
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            id: rightundo
            enabled: bEnableControls? (!centerCheckBox.checked): false 
            onClicked: {
                rightslider.value = 0
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        
        Item {
            Layout.fillHeight: true;
        }
    }
}
