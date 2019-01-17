
import QtQuick 2.1
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Rectangle {
    id:keyFrame

    color: '#353535'//activePalette.window
    width: 400
    height: 32

//    KeyFramesInfo {
//        id: keyFramesInfo
//    }


    function setKeyFrameValue()
    {
        var position = timeline.getPositionInCurrentClip()
        if (position < 0) return

        var paramCount = metadata.keyframes.parameterCount
        for(var i = 0; i < paramCount; i++)
        {
            var key = metadata.keyframes.parameters[i].property
            var value = filter.get(key)

            if (filter.getKeyFrameNumber() <= 0)
            {
                var position2 = filter.producerOut - filter.producerIn + 1 - 5
                filter.setKeyFrameParaValue(position2, key, value.toString() );

                filter.setKeyFrameParaValue(0, key, value.toString() );
            }

            filter.setKeyFrameParaValue(position, key, value.toString() );

            filter.combineAllKeyFramePara();
        }

     }

    CustomFilterButton {
        id:addKeyFrameButton
        anchors {
            top: parent.bottom - addKeyFrameButton.height
            left: parent.left
            leftMargin: 10
            topMargin: 0
        }
        implicitWidth: 32
        implicitHeight: 32

        enabled: (metadata && (metadata.keyframes.parameterCount > 0)) ? true : false   //filter
        opacity: enabled ? 1.0 : 0.5
        tooltip: qsTr('Add a key frame')
        customIconSource: 'qrc:///icons/light/32x32/list-add.png'
        onClicked: {
            setKeyFrameValue()
        }
        onEnabledChanged: {
            var position = timeline.getPositionInCurrentClip()
            preKeyFrameButton.enabled      = metadata && (metadata.keyframes.parameterCount > 0)
            nextKeyFrameButton.enabled     = metadata && (metadata.keyframes.parameterCount > 0)
            removeKeyFrameButton.enabled   = metadata && (metadata.keyframes.parameterCount > 0)
        }
    }

    Connections {
        target: filterDock
        onPositionChanged: {
             preKeyFrameButton.enabled      = filter.bHasPreKeyFrame(timeline.getPositionInCurrentClip())
             nextKeyFrameButton.enabled     = filter.bHasNextKeyFrame(timeline.getPositionInCurrentClip())
             removeKeyFrameButton.enabled   = filter.bKeyFrame(timeline.getPositionInCurrentClip())
        }
    }

    CustomFilterButton {
        id:preKeyFrameButton
        anchors {
            top: addKeyFrameButton.top
            left: addKeyFrameButton.right
            leftMargin: 20
            topMargin: 0
        }
        implicitWidth: 32
        implicitHeight: 32

        enabled: filter.bHasPreKeyFrame(timeline.getPositionInCurrentClip())
        opacity: enabled ? 1.0 : 0.5
        tooltip: qsTr('Prev key frame')
        customIconSource: enabled?'qrc:///icons/light/32x32/previous_keyframe.png' :'qrc:///icons/light/32x32/previous_keyframe_disable.png'
        onClicked: {
            var nFrame = filter.getPreKeyFrameNum(timeline.getPositionInCurrentClip())
            if(nFrame != -1)
            {
                filterDock.position = nFrame
            }
        }
    }

    CustomFilterButton {
        id:nextKeyFrameButton
        anchors {
            top: addKeyFrameButton.top
            left: preKeyFrameButton.right
            leftMargin: 0
            topMargin: 0
        }
        implicitWidth: 32
        implicitHeight: 32

        enabled: filter.bHasNextKeyFrame(timeline.getPositionInCurrentClip())
        opacity: enabled ? 1.0 : 0.5
        tooltip: qsTr('Next key frame')
        customIconSource: enabled?'qrc:///icons/light/32x32/next_keyframe.png':'qrc:///icons/light/32x32/next_keyframe_disable.png'
        onClicked: {
            var nFrame = filter.getNextKeyFrameNum(timeline.getPositionInCurrentClip())
            if(nFrame != -1)
            {
                filterDock.position = nFrame
            }
        }
    }

    CustomFilterButton {
        id:removeKeyFrameButton
        anchors {
            top: addKeyFrameButton.top
            left: nextKeyFrameButton.right
            leftMargin: 20
            topMargin: 0
        }
        implicitWidth: 32
        implicitHeight: 32

        enabled: filter.bKeyFrame(timeline.getPositionInCurrentClip())
        opacity: enabled ? 1.0 : 0.5
        tooltip: qsTr('Remove current frame')
        customIconSource: enabled?'qrc:///icons/light/32x32/next_keyframe.png':'qrc:///icons/light/32x32/next_keyframe_disable.png'
        onClicked: {
            var position        = timeline.getPositionInCurrentClip()
            var bKeyFrame       = filter.bKeyFrame(position)
            if (bKeyFrame)
                filter.removeKeyFrameParaValue(position)
        }
    }



//    CustomFilterButton {
//        id:viewKeyFrameInfosButton
//        anchors {
//            top: addKeyFrameButton.top
//            left: addKeyFrameButton.right
//            leftMargin: 10
//            topMargin: 0
//        }
//        implicitWidth: 32
//        implicitHeight: 32

//        enabled: (metadata) ? true : false
//        opacity: enabled ? 1.0 : 0.5
//        tooltip: qsTr('View key frame infos')
//        customIconSource: 'qrc:///icons/light/32x32/list-add.png'
//        onClicked: {
//            keyFramesInfo.popup(viewKeyFrameInfosButton)
//        }
//    }
}
