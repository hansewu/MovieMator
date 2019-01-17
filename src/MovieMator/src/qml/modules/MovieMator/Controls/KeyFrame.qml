import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.0

RowLayout{

    id: keyFrame

    property bool bKeyFrame: isKeyFrame.checked
    property int currentPosition: 0


    function getCurrentFrame(){
        return currentFrameBox.value;
    }
    //Tell parent UI this frame will be set to key frame or not
    signal setAsKeyFrame(bool bKeyFrame)
    signal loadKeyFrame(double keyFrameNum)

    Item{
        width: 10
    }

    Label{
        text: qsTr('Frame:')
        color: '#ffffff'
        Layout.alignment: Qt.AlignRight
    }

    SpinBox{

        property bool blockSignals: false
        id: currentFrameBox
        implicitWidth: 90

        maximumValue: 65535
        stepSize:1
        onValueChanged: {
            if (!blockSignals)
                filterDock.position = value;
            if(filter.bKeyFrame(currentFrameBox.value))
            {
                isKeyFrame.blockSignals = true
                isKeyFrame.checked = true;
                isKeyFrame.blockSignals = false
                loadKeyFrame(currentFrameBox.value)
            }
            else
            {
                isKeyFrame.blockSignals = true
                isKeyFrame.checked = false;
                isKeyFrame.blockSignals = false
            }
        }
    }

    Component.onCompleted:
    {
        currentFrameBox.blockSignals = true;
        currentFrameBox.value = timeline.getPositionInCurrentClip();
        currentFrameBox.blockSignals = false;
    }

    Connections {
             target: filterDock
             onPositionChanged: {
                 currentFrameBox.blockSignals = true;
                 currentFrameBox.value = timeline.getPositionInCurrentClip();
                 currentFrameBox.blockSignals = false;

             }
    }

    Connections {
        target: multitrack
        onModified:{
            currentFrameBox.blockSignals = true;
            currentFrameBox.value = timeline.getPositionInCurrentClip();
            currentFrameBox.blockSignals = false;
        }
    }


    Item{
        width: 5
    }

    CheckBox{
        id: isKeyFrame
        property bool blockSignals: false
        checked: filter.bKeyFrame(currentFrameBox.value)
        style: CheckBoxStyle {
            label: Text {
                color: "white"
                text: qsTr("KeyFrame")
            }
        }
       
        onCheckedChanged: {
            if (!blockSignals)
                setAsKeyFrame(checked)
        }
    }

    Button{
        id: preKeyFrame
        implicitWidth:15
        implicitHeight:15
        
        iconSource: enabled?'qrc:///icons/light/32x32/previous_keyframe.png' :'qrc:///icons/light/32x32/previous_keyframe_disable.png'
        enabled: filter.bHasPreKeyFrame(currentFrameBox.value)

        style: ButtonStyle {
                padding {
                    left: 0
                    right: 0
                    top: 0
                    bottom: 0
                }
        }

        onClicked: {

            var nFrame = filter.getPreKeyFrameNum(currentFrameBox.value)
            if(nFrame != -1)
            {
                currentFrameBox.value = nFrame
                loadKeyFrame(nFrame)
                isKeyFrame.blockSignals = true
                isKeyFrame.checked = filter.bKeyFrame(nFrame)
                isKeyFrame.blockSignals = false
            }

        }
    }

    Button{
        id: nextKeyFrame
        implicitWidth:15
        implicitHeight:15
        x: preKeyFrame.x+23

      
        iconSource: enabled?'qrc:///icons/light/32x32/next_keyframe.png':'qrc:///icons/light/32x32/next_keyframe_disable.png'
        enabled: filter.bHasNextKeyFrame(currentFrameBox.value)
        style: ButtonStyle {
                padding {
                    left: 0
                    right: 0
                    top: 0
                    bottom: 0
                }
    }
        onClicked: {
            var nFrame = filter.getNextKeyFrameNum(currentFrameBox.value)
            if(nFrame != -1)
            {
                currentFrameBox.value = nFrame
                loadKeyFrame(nFrame)
                isKeyFrame.checked = filter.bKeyFrame(nFrame)
            }

        }
    }



}

