
import QtQuick 2.2
import QtQml.Models 2.1
import QtQuick.Controls 1.0
import MovieMator.Controls 1.0
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import 'Timeline.js' as Logic
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: root

    SystemPalette { id: activePalette }
    //color: activePalette.window
    color: backgroundColor

    signal clipClicked()

    function zoomIn() {
//        scaleSlider.value += 0.0625
        toolbar.scaleSliderValue += 0.0625
        for (var i = 0; i < tracksRepeater.count; i++)
            tracksRepeater.itemAt(i).redrawWaveforms()
    }

    function zoomOut() {
//        scaleSlider.value -= 0.0625
        toolbar.scaleSliderValue -= 0.0625
        for (var i = 0; i < tracksRepeater.count; i++)
            tracksRepeater.itemAt(i).redrawWaveforms()
    }

    function resetZoom() {
//        scaleSlider.value = 1.0
        toolbar.scaleSliderValue = 1.0
        for (var i = 0; i < tracksRepeater.count; i++)
            tracksRepeater.itemAt(i).redrawWaveforms()
    }

    function makeTracksTaller() {
        multitrack.trackHeight += 20
    }

    function makeTracksShorter() {
        multitrack.trackHeight = Math.max(30, multitrack.trackHeight - 20)
    }

    function pulseLockButtonOnTrack(index) {
        trackHeaderRepeater.itemAt(index).pulseLockButton()
    }

    function selectMultitrack() {
        for (var i = 0; i < trackHeaderRepeater.count; i++)
            trackHeaderRepeater.itemAt(i).selected = false
        cornerstone.selected = true
    }

    property int headerWidth: 92
    property int currentTrack: 0
    property color selectedTrackColor: Qt.rgba(80/255, 81/255, 82/255, 1.0)
    property color normalColor: Qt.rgba(49/255, 50/255, 52/255, 1.0)
    property color handleColor: Qt.rgba(92/255, 93/255, 94/255, 1.0)
    property color backgroundColor: Qt.rgba(37/255, 38/255, 39/255, 1.0)
    property alias trackCount: tracksRepeater.count
    property bool stopScrolling: false

    property color moviematorBlue: Qt.rgba(153/255, 216/255, 234/255, 1.0)//Qt.rgba(23/255, 92/255, 118/255, 1.0)
//    property var selection: []
    property alias ripple: toolbar.ripple

    onCurrentTrackChanged: timeline.selection = []

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: menu.popup()
    }

    DropArea {
        id: dropArea
        property bool hasUrls
        property bool hasAbstractItem
        property string teststring: 'dropArea'
        anchors.fill: parent

        function endDrop()
        {
            Drag.cancel()
        }
        onEntered: {
            console.log("onEntered --- --- --- --- --- ---")
            if (drag.formats.indexOf('application/mlt+xml') >= 0 || drag.hasUrls || drag.formats.indexOf('application/x-qabstractitemmodeldatalist') >= 0)
            {
                drag.acceptProposedAction()
                hasUrls = false
                hasAbstractItem = false
            }
            if (drag.formats.indexOf('application/x-qabstractitemmodeldatalist') >= 0)
            {
                 hasAbstractItem =  true
            }
            if (drag.hasUrls)
               hasUrls = true
        }
        onExited: {
            console.log("onExited --- --- --- --- --- ---")
            Logic.dropped()
        }
        onPositionChanged: {
            console.log("onPositionChanged --- --- --- --- --- ---")
            if (drag.formats.indexOf('application/mlt+xml') >= 0 || drag.hasUrls || drag.formats.indexOf('application/x-qabstractitemmodeldatalist') >= 0)
            {

                  Logic.dragging(drag, drag.text)
            }
        }
        onDropped: {
            console.log("onDropped --- --- --- --- --- ---")
            if (drop.formats.indexOf('application/mlt+xml') >= 0) //数据来自于media窗口
            {
                if (currentTrack >= 0)
                {
                    Logic.acceptDrop(drop.getDataAsString('application/mlt+xml'))
                    drop.acceptProposedAction()
                    toolbar.hasClipOrTrackSelected = true

                }
            }
            else if (drop.hasUrls) {

                if (currentTrack >= 0 ) {
                    Logic.acceptDropUrls(drop.urls)
                    drop.acceptProposedAction()
                    toolbar.hasClipOrTrackSelected = true
                }
            }
            else if (drop.formats.indexOf('application/x-qabstractitemmodeldatalist') >= 0) //数据来自于recent窗口
            {
                   if (currentTrack >= 0 )
                   {
                       console.log(drop.source.objectName)

                       Logic.acceptDropListItem(drop.getDataAsArrayBuffer('application/x-qabstractitemmodeldatalist'))
                       drop.acceptProposedAction()
                       toolbar.hasClipOrTrackSelected = true
                    }
            }
            Logic.dropped()
        }

    }



    TimelineToolbar {
        id: toolbar
        width: parent.width - 2
        height: 51//ruler.height + 15

        hasClipOrTrackSelected: false

        anchors {
            top: parent.top
//            topMargin: 6
            left:parent.left
            leftMargin: 1
            rightMargin: 1
        }
        z: 0
    }

    Row {
        anchors {
            top: toolbar.bottom
            left: parent.left
            leftMargin: 1
            rightMargin: 1
            bottomMargin: 1
        }
        width: root.width

        Column {
            z: 0
            spacing: 1
            Rectangle {
                id: cornerstone
                property bool selected: false
                // Padding between toolbar and track headers.
                width: headerWidth
                height: ruler.height

                border.color: selected? 'red' : 'transparent'
                border.width: selected? 1 : 0
                color: selected? moviematorBlue : normalColor//activePalette.window
                z: 0
            }


            MouseArea {
                width: headerWidth
                height: root.height

                // This provides continuous scrubbing and scimming at the left/right edges.
                focus: true
                hoverEnabled: true

            ScrollView {
                id: headerScrollView
                // Non-slider scroll area for the track headers.
                //flickableItem.contentY: scrollView.flickableItem.contentY
                width: headerWidth
                height: root.height - ruler.height - toolbar.height - 1 - 1 //下留白1 spacing 1
                //interactive: false
                //boundsBehavior: Flickable.StopAtBounds
                horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff



                Item {
                    width: headerWidth
                    height: trackHeaders.height + 30 // 30 is padding
                Column {
                    id: trackHeaders
                    spacing: 1
                    Repeater {
                        id: trackHeaderRepeater
                        model: multitrack
                        TrackHead {
                            trackName: model.name
                            isMute: model.mute
                            isHidden: model.hidden
                            isComposite: model.composite
                            isLocked: model.locked
                            isVideo: model.video//!model.audio
                            isText: model.text
                            width: headerWidth - 1
                            height: Logic.trackHeight(model.video)
                            selected: false
                            current: index === currentTrack
                            onIsLockedChanged: tracksRepeater.itemAt(index).isLocked = isLocked
                            onClicked: {
//                                timeline.selection = []
                                currentTrack = index
                                timeline.selectTrackHead(currentTrack)
                                toolbar.hasClipOrTrackSelected = true
//                                cornerstone.selected = false
//                                for (var i = 0; i < trackHeaderRepeater.count; i++)
//                                    trackHeaderRepeater.itemAt(i).selected = false
//                                selected = true
                            }
                        }
                    }
                }
                Rectangle {
                    // thin dividing line between headers and tracks
                    color: activePalette.window//activePalette.windowText
                    width: 1
                    x: parent.x + parent.width
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                }
              }
           }


         }

       }
        MouseArea {
            id: tracksArea
            width: root.width - headerWidth - 2
            height: root.height

            // This provides continuous scrubbing and scimming at the left/right edges.
            focus: true
            hoverEnabled: true
            onClicked: timeline.position = (scrollView.flickableItem.contentX + mouse.x) / multitrack.scaleFactor
            property bool scim: false
            onReleased: scim = false
            onExited: scim = false
            onPositionChanged: {
                if (mouse.modifiers === Qt.ShiftModifier || mouse.buttons === Qt.LeftButton) {
                    timeline.position = (scrollView.flickableItem.contentX + mouse.x) / multitrack.scaleFactor
                    scim = true
                }
                else
                    scim = false
            }

            Timer {
                id: scrubTimer
                interval: 25
                repeat: true
                running: parent.scim && parent.containsMouse && (parent.mouseX < 50 || parent.mouseX > parent.width - 50)
                onTriggered: {
                    if (parent.mouseX < 50)
                        timeline.position -= 10
                    else
                        timeline.position += 10
                }
            }

            Column {
                width:root.width - headerWidth - 2
                spacing: 1
                ScrollView {
                    id: rulerScrollView
                    // Non-slider scroll area for the Ruler.
                    //flickableItem.contentX: scrollView.flickableItem.contentX
                    width: root.width - headerWidth - 2
                    height: ruler.height
                    //interactive: false
                    //boundsBehavior: Flickable.StopAtBounds
                    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                    verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                    Ruler {
                        id: ruler
                        width: (tracksContainer.width < width) ? width : tracksContainer.width
                        index: index
                        timeScale: multitrack.scaleFactor
                    }
                }
                ScrollView {
                    id: scrollView
                    width: root.width - headerWidth - 2
                    height: root.height - ruler.height - toolbar.height - 1 - 1//下留白 与ruler的space
                    //flickableItem.interactive: false  //wzq


                    style: ScrollViewStyle {
                        handle:
                            Rectangle {
                                implicitWidth: 14
                                implicitHeight: 14
//                                border.width: 3
//                                border.color:'transparent'// activePalette.window
                                color: 'transparent'//'#989898'
//                                radius: 7
                                Rectangle {
                                    anchors.margins: 3
                                    anchors.fill: parent
                                    color: '#787878'
                                    radius: 4
                                }

                            }
                        scrollBarBackground: Rectangle {
                            implicitWidth: 14
                            implicitHeight: 14
                       //     border.width: 2
                       //     border.color: normalColor//activePalette.window
                            color: 'transparent'//'#505050'//normalColor
                        }
                        decrementControl: Rectangle {
                            implicitWidth: 0
                            implicitHeight: 0

//                            border.width: 2
//                            border.color: activePalette.window

                        }
                        incrementControl: Rectangle {
                            implicitWidth: 0
                            implicitHeight: 0
//                            border.width: 2
//                            border.color: activePalette.window
                        }
                        corner: Rectangle {
                            implicitWidth: 14
                            implicitHeight: 14

                            border.width: 0
                            color: 'transparent'//normalColor
                        }

                    }

                    Item {
                        width: ((tracksContainer.width + headerWidth) < width) ? width : (tracksContainer.width + headerWidth)
                        height: trackHeaders.height + 30 // 30 is padding

                        MouseArea
                        {
                                                   anchors.fill: parent
                                                   propagateComposedEvents: true
                                                   onPressed: mouse.accepted = false
                                                   onReleased: mouse.accepted = false
                                                   onClicked: mouse.accepted = false
                                                   onDoubleClicked: mouse.accepted = false
                                                   onPressAndHold: mouse.accepted = false
                                                   onWheel:
                                                   {
                                                       if (wheel.angleDelta.y < 0)
                                                       {
                                                            root.zoomIn()
                                                       }
                                                       else if(wheel.angleDelta.y > 0)
                                                       {
                                                           root.zoomOut()
                                                        }

                                                   }

                        }

                        Column {
                            // These make the striped background for the tracks.
                            // It is important that these are not part of the track visual hierarchy;
                            // otherwise, the clips will be obscured by the Track's background.
                            spacing: 1
                            Repeater {
                                model: multitrack
                                delegate: Rectangle {
                                    width: (tracksContainer.width < width) ? width : tracksContainer.width
                                    color: (index === currentTrack)? selectedTrackColor : normalColor//(index % 2)? activePalette.alternateBase : activePalette.base
                                    height: Logic.trackHeight(video)
                                }
                            }
                        }
                        Column {
                            id: tracksContainer
                            spacing: 1
                            Repeater { id: tracksRepeater; model: trackDelegateModel }
                        }
                    }
                }
            }

            CornerSelectionShadow {
                y: tracksRepeater.count ? tracksRepeater.itemAt(currentTrack).y + ruler.height - scrollView.flickableItem.contentY : 0
                clip: timeline.selection.length ?
                        tracksRepeater.itemAt(currentTrack).clipAt(timeline.selection[0]) : null
                opacity: clip && clip.x + clip.width < scrollView.flickableItem.contentX ? 1 : 0
            }

            CornerSelectionShadow {
                y: tracksRepeater.count ? tracksRepeater.itemAt(currentTrack).y + ruler.height - scrollView.flickableItem.contentY : 0
                clip: timeline.selection.length ?
                        tracksRepeater.itemAt(currentTrack).clipAt(timeline.selection[timeline.selection.length - 1]) : null
                opacity: clip && clip.x > scrollView.flickableItem.contentX + scrollView.width ? 1 : 0
                anchors.right: parent.right
                mirrorGradient: true
            }

            Rectangle {
                id: cursor
                visible: timeline.position > -1
                color: activePalette.text
                width: 1
                height: root.height - scrollView.__horizontalScrollBar.height - toolbar.height
                x: timeline.position * multitrack.scaleFactor - scrollView.flickableItem.contentX
                y: 0
            }
            TimelinePlayhead {
                id: playhead
                visible: timeline.position > -1
                x: timeline.position * multitrack.scaleFactor - scrollView.flickableItem.contentX - 5
                y: 0
                width: 11
                height: 5
            }
        }
    }


    Rectangle {
        id: dropTarget
        height: multitrack.trackHeight
        opacity: 0.5
        visible: false
        Text {
            anchors.fill: parent
            anchors.leftMargin: 100
            //text: toolbar.ripple? qsTr('Insert') : qsTr('Overwrite')
            text: (dropArea.hasUrls || dropArea.hasAbstractItem) ? qsTr('Append') :qsTr('Insert')
            style: Text.Outline
            styleColor: 'white'
            font.pixelSize: Math.min(Math.max(parent.height * 0.8, 15), 30)
            verticalAlignment: Text.AlignVCenter
        }
    }

    Rectangle {
        id: bubbleHelp
        property alias text: bubbleHelpLabel.text
        color: application.toolTipBaseColor
        width: bubbleHelpLabel.width + 8
        height: bubbleHelpLabel.height + 8
        radius: 4
        states: [
            State { name: 'invisible'; PropertyChanges { target: bubbleHelp; opacity: 0} },
            State { name: 'visible'; PropertyChanges { target: bubbleHelp; opacity: 1} }
        ]
        state: 'invisible'
        transitions: [
            Transition {
                from: 'invisible'
                to: 'visible'
                OpacityAnimator { target: bubbleHelp; duration: 200; easing.type: Easing.InOutQuad }
            },
            Transition {
                from: 'visible'
                to: 'invisible'
                OpacityAnimator { target: bubbleHelp; duration: 200; easing.type: Easing.InOutQuad }
            }
        ]
        Label {
            id: bubbleHelpLabel
            color: application.toolTipTextColor
            anchors.centerIn: parent
        }
        function show(x, y, text) {
            bubbleHelp.x = x + tracksArea.x - scrollView.flickableItem.contentX - bubbleHelpLabel.width
            bubbleHelp.y = y + tracksArea.y - scrollView.flickableItem.contentY - bubbleHelpLabel.height+18
            bubbleHelp.text = text
            if (bubbleHelp.state !== 'visible')
                bubbleHelp.state = 'visible'
        }
        function hide() {
            bubbleHelp.state = 'invisible'
            bubbleHelp.opacity = 0
        }
    }
    DropShadow {
        source: bubbleHelp
        anchors.fill: bubbleHelp
        opacity: bubbleHelp.opacity
        horizontalOffset: 3
        verticalOffset: 3
        radius: 8
        color: '#80000000'
        transparentBorder: true
        fast: true
    }

    Menu {
        id: menu
        MenuItem {
            text: qsTr('Add Audio Track')
            shortcut: 'Ctrl+U'
            onTriggered: timeline.addAudioTrack()
        }
        MenuItem {
            text: qsTr('Add Video Track')
            shortcut: 'Ctrl+Y'
            onTriggered: timeline.addVideoTrack()
        }
        MenuItem {
            text: qsTr('Insert Track')
            onTriggered: timeline.insertTrack()
        }
        MenuItem {
            enabled: !multitrack.defaultTrack
            text: qsTr('Remove Track')
            onTriggered: timeline.removeTrack()
        }

    }


    DelegateModel {
        id: trackDelegateModel
        model: multitrack
        Track {
            model: multitrack
            rootIndex: trackDelegateModel.modelIndex(index)
            height: Logic.trackHeight(video)
            isAudio: audio
            isCurrentTrack: currentTrack === index
            timeScale: multitrack.scaleFactor
            selection: timeline.selection
            onClipClicked: {
                currentTrack = track.DelegateModel.itemsIndex
                timeline.selection = [ clip.DelegateModel.itemsIndex ]
                root.clipClicked()
                toolbar.hasClipOrTrackSelected = true

            }

            onClipDragged: {
                // This provides continuous scrolling at the left/right edges.
                if (x > scrollView.flickableItem.contentX + scrollView.width - 50) {
                    scrollTimer.item = clip
                    scrollTimer.backwards = false
                    scrollTimer.start()
                } else if (x < 50) {
                    scrollView.flickableItem.contentX = 0
                    scrollTimer.stop()
                } else if (x < scrollView.flickableItem.contentX + 50) {
                    scrollTimer.item = clip
                    scrollTimer.backwards = true
                    scrollTimer.start()
                } else {
                    scrollTimer.stop()
                }
                // Show distance moved as time in a "bubble" help.
                var track = tracksRepeater.itemAt(clip.trackIndex)
                var delta = Math.round((clip.x - clip.originalX) / multitrack.scaleFactor)
                var s = timeline.timecode(Math.abs(delta))
                // remove leading zeroes
                if (s.substring(0, 3) === '00:')
                    s = s.substring(3)
                s = ((delta < 0)? '-' : (delta > 0)? '+' : '') + s
                bubbleHelp.show(x, track.y + height, s)
            }
            onClipDropped: {
                scrollTimer.running = false
                bubbleHelp.hide()
            }
            onClipDraggedToTrack: {
                var i = clip.trackIndex + direction
                if (i >= 0  && i < tracksRepeater.count) {
                    var track = tracksRepeater.itemAt(i)
                    clip.reparent(track)
                    clip.trackIndex = track.DelegateModel.itemsIndex
                }
            }
            onCheckSnap: {
                for (var i = 0; i < tracksRepeater.count; i++)
                    tracksRepeater.itemAt(i).snapClip(clip)
            }
            Image {
                anchors.fill: parent
                source: "qrc:///icons/light/16x16/track-locked.png"
                fillMode: Image.Tile
                opacity: parent.isLocked
                visible: opacity
                Behavior on opacity { NumberAnimation {} }
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        mouse.accepted = true
                        trackHeaderRepeater.itemAt(index).pulseLockButton()
                    }
                }
            }
        }
    }

    Connections {
        target: timeline
        onPositionChanged: if (!stopScrolling) Logic.scrollIfNeeded()
        onDragging: Logic.dragging(pos, duration)
        onDropped: Logic.dropped()
        onDropAccepted: Logic.acceptDrop(xml)
        onSelectionChanged: {
            cornerstone.selected = timeline.isMultitrackSelected()
            var selectedTrack = timeline.selectedTrack()
            for (var i = 0; i < trackHeaderRepeater.count; i++)
                trackHeaderRepeater.itemAt(i).selected = (i === selectedTrack)
        }
    }

    Connections {
        target: multitrack
        onLoaded: toolbar.scaleSliderValue = Math.pow(multitrack.scaleFactor - 0.01, 1.0 / 3.0)//scaleSlider.value = Math.pow(multitrack.scaleFactor - 0.01, 1.0 / 3.0)
    }

    Connections {
        target: scrollView.flickableItem
        onContentYChanged:
        {
            headerScrollView.flickableItem.contentY = scrollView.flickableItem.contentY
        }
        onContentXChanged:
        {
            rulerScrollView.flickableItem.contentX =  scrollView.flickableItem.contentX
        }
    }

    Connections {
        target: headerScrollView.flickableItem
        onContentYChanged:
        {
            scrollView.flickableItem.contentY = headerScrollView.flickableItem.contentY
        }
    }


    // This provides continuous scrolling at the left/right edges.
    Timer {
        id: scrollTimer
        interval: 25
        repeat: true
        triggeredOnStart: true
        property var item
        property bool backwards
        onTriggered: {
            var delta = backwards? -10 : 10
            if (item) item.x += delta
            scrollView.flickableItem.contentX += delta
            if (scrollView.flickableItem.contentX <= 0)
                stop()
        }
    }

}
