/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: WanYuanCN <ebthon@hotmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

    // 单击轨道上 clip的信号
    signal clipClicked()

    // 时间线滚轮缩放之放大
    function zoomIn(wheelx) {
//        scaleSlider.value += 0.0625

        toolbar.flag = true
        toolbar.wheelx = wheelx
        toolbar.scaleValue = multitrack.scaleFactor

        // toolbar.scaleSliderValue += 0.0625
        toolbar.scaleSliderValue = toolbar.scaleCopy + 0.0625
        toolbar.scaleCopy = toolbar.scaleSliderValue

        console.assert(tracksRepeater);
        if(!tracksRepeater) return;
        for (var i = 0; i < tracksRepeater.count; i++){
            console.assert(tracksRepeater.itemAt(i));
            if(tracksRepeater.itemAt(i))
                tracksRepeater.itemAt(i).redrawWaveforms()
        }
    }

    // 时间线滚轮缩放之缩小
    function zoomOut(wheelx) {
//        scaleSlider.value -= 0.0625

        toolbar.flag = true
        toolbar.wheelx = wheelx
        toolbar.scaleValue = multitrack.scaleFactor
        
        // toolbar.scaleSliderValue -= 0.0625
        if(toolbar.scaleCopy != 0){
            toolbar.scaleSliderValue = toolbar.scaleCopy - 0.0625
            toolbar.scaleCopy = toolbar.scaleSliderValue
        }

        console.assert(tracksRepeater);
        if(!tracksRepeater) return;
        for (var i = 0; i < tracksRepeater.count; i++){
            console.assert(tracksRepeater.itemAt(i));
            if(tracksRepeater.itemAt(i))
                tracksRepeater.itemAt(i).redrawWaveforms()
        }
    }

    // 把时间线重置为原来大小
    function resetZoom() {
//        scaleSlider.value = 1.0
        toolbar.scaleSliderValue = 1.0
        console.assert(tracksRepeater);
        if(!tracksRepeater) return;
        for (var i = 0; i < tracksRepeater.count; i++){
            console.assert(tracksRepeater.itemAt(i));
            if(tracksRepeater.itemAt(i))
                tracksRepeater.itemAt(i).redrawWaveforms()
        }
    }

    // 增加时间线轨道的高度
    function makeTracksTaller() {
        multitrack.trackHeight += 20
    }

    // 降低时间线轨道的高度
    function makeTracksShorter() {
        multitrack.trackHeight = Math.max(30, multitrack.trackHeight - 20)
    }

    // 当操作被锁定的轨道时播放轨道头的锁定按钮缩放动画
    // timelinedock.cpp/clip.qml调用
    function pulseLockButtonOnTrack(index) {
        console.assert(trackHeaderRepeater.itemAt(index));
        if(trackHeaderRepeater.itemAt(index))
            trackHeaderRepeater.itemAt(index).pulseLockButton()
    }

    // 选择多个轨道
    function selectMultitrack() {
        console.assert(trackHeaderRepeater);
        if(!trackHeaderRepeater) return;
        for (var i = 0; i < trackHeaderRepeater.count; i++){
            console.assert(trackHeaderRepeater.itemAt(i));
            if(trackHeaderRepeater.itemAt(i))
                trackHeaderRepeater.itemAt(i).selected = false
        }
        cornerstone.selected = true
    }

    // 轨道头的宽度（定值）
    property int headerWidth: 108    //92
    // 当前（选中）轨道的序号
    property int currentTrack: 0
    // 被选中的轨道的背景颜色
    property color selectedTrackColor: Qt.rgba(80/255, 81/255, 82/255, 1.0)
    // 轨道的普通状态下的颜色
    property color normalColor: Qt.rgba(49/255, 50/255, 52/255, 1.0)
    // ？？？
    property color handleColor: Qt.rgba(92/255, 93/255, 94/255, 1.0)
    // 背景色？？？
    property color backgroundColor: Qt.rgba(37/255, 38/255, 39/255, 1.0)
    // 轨道数量
    property alias trackCount: tracksRepeater.count
    // 禁用鼠标滚动（滚动条/缩放功能）
    property bool stopScrolling: false

    // MovieMator的主题色（蓝色）
    property color moviematorBlue: Qt.rgba(153/255, 216/255, 234/255, 1.0)//Qt.rgba(23/255, 92/255, 118/255, 1.0)
//    property var selection: []
    // 波纹
    property alias ripple: toolbar.ripple

    // 当前轨道发生变化时响应的函数
    onCurrentTrackChanged: {
        console.assert(timeline);
        if(timeline)
            timeline.selection = []
    }

    // 时间线右键时弹出菜单
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: trackmenu.popup()    // menu.popup()
    }

    // 时间线可接收拖放内容的区域
    DropArea {
        id: dropArea
        property bool hasUrls
        property bool hasAbstractItem
        property string teststring: 'dropArea'
        keys: ["application/mlt+xml", "application/x-qabstractitemmodeldatalist", "text/uri-list"]//只接收整个三个源来的拖拽
        anchors.fill: parent
        // 取消拖放
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



    // 时间线上方的工具栏菜单
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

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: menu.popup()
        }
    }

    // 刻度尺 Ruler
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
                                currentTrack = index
                                console.assert(timeline);
                                if(timeline){
                                    timeline.selectTrackHead(currentTrack)
                                }
                                toolbar.hasClipOrTrackSelected = true
                                for (var i = 0; i < trackHeaderRepeater.count; i++) {//设置当前选中trackhead的选中状态
                                    trackHeaderRepeater.itemAt(i).selected = (i == currentTrack)
                                }
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
            onClicked: {
                console.assert(timeline);
                if(timeline)
                    timeline.position = (scrollView.flickableItem.contentX + mouse.x) / multitrack.scaleFactor
            }
            property bool scim: false
            onReleased: scim = false
            onExited: scim = false
            onPositionChanged: {
                console.assert(timeline);
                if ((mouse.modifiers === Qt.ShiftModifier || mouse.buttons === Qt.LeftButton) && timeline) {
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
                    console.assert(timeline);
                    if(!timeline)
                        return;
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
                        width: scrollView.width
                        timeScale: multitrack.scaleFactor
                        startX: scrollView.flickableItem.contentX
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
                                color: 'transparent'//'#989898'
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
                            color: 'transparent'//'#505050'//normalColor
                        }
                        decrementControl: Rectangle {
                            implicitWidth: 0
                            implicitHeight: 0
                        }
                        incrementControl: Rectangle {
                            implicitWidth: 0
                            implicitHeight: 0
                        }
                        corner: Rectangle {
                            implicitWidth: 14
                            implicitHeight: 14

                            border.width: 0
                            color: 'transparent'//normalColor
                        }
                    }

                    Item {
                        width: ((tracksContainer.width + headerWidth) < scrollView.width) ? scrollView.width : (tracksContainer.width + headerWidth)
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
                                    root.zoomIn(tracksArea.mouseX)
                                }
                                else if(wheel.angleDelta.y > 0)
                                {
                                    root.zoomOut(tracksArea.mouseX)
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
                                    width: (tracksContainer.width < scrollView.width) ? scrollView.width : tracksContainer.width
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
                y: tracksRepeater.count ? (tracksRepeater.itemAt(currentTrack) ? tracksRepeater.itemAt(currentTrack).y : 0) + ruler.height - scrollView.flickableItem.contentY : 0
                clip: (timeline && timeline.selection.length) ?
                        tracksRepeater.itemAt(currentTrack).clipAt(timeline.selection[0]) : null
                opacity: clip && clip.x + clip.width < scrollView.flickableItem.contentX ? 1 : 0
            }

            CornerSelectionShadow {
                y: tracksRepeater.count ? (tracksRepeater.itemAt(currentTrack) ? tracksRepeater.itemAt(currentTrack).y : 0) + ruler.height - scrollView.flickableItem.contentY : 0
                clip: (timeline && timeline.selection.length) ?
                        tracksRepeater.itemAt(currentTrack).clipAt(timeline.selection[timeline.selection.length - 1]) : null
                opacity: clip && clip.x > scrollView.flickableItem.contentX + scrollView.width ? 1 : 0
                anchors.right: parent.right
                mirrorGradient: true
            }

            Rectangle {
                id: cursor
                visible: x >= 0     // timeline.position > -1
                color: "#C0482C"    // activePalette.text
                width: 1
                height: root.height - scrollView.__horizontalScrollBar.height - toolbar.height
                x: timeline ? timeline.position * multitrack.scaleFactor - scrollView.flickableItem.contentX : 0
                y: 0
            }
            TimelinePlayhead {
                id: playhead
                visible: x >= -5        // timeline.position > -1
                x: timeline ? timeline.position * multitrack.scaleFactor - scrollView.flickableItem.contentX - 5 : 0
                y: 0
                width: 11
                height: 5
            }
        }
    }


    // 拖放时在轨道上显示 clip的占用区域
    // （会显示 append和 insert文字）
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

    // 显示（延迟或超前）时间的小气泡
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
            console.assert(scrollView);
            if(!scrollView) return;
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

    // 右键的菜单
    Menu {
        id: menu
        MenuItem {
            text: qsTr('Add Audio Track')
            shortcut: 'Ctrl+U'
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.addAudioTrack()
            }
        }
        MenuItem {
            text: qsTr('Add Video Track')
            shortcut: 'Ctrl+Y'
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.addVideoTrack()
            }
        }
        MenuItem {
            text: qsTr('Insert Track')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.insertTrack()
            }
        }
        MenuItem {
            enabled: !multitrack.defaultTrack
            text: qsTr('Remove Track')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.removeTrack()
            }
        }

    }

    // 轨道菜单
    Menu {
        id: trackmenu
        // 切割
        MenuItem {
            text: qsTr('Split')
            shortcut: 'S'
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.splitClip(currentTrack)
            }
        }
        // 追加资源
        MenuItem {
            text: qsTr('Append');
            shortcut: 'A'
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.append(currentTrack)
            }
        }
        MenuSeparator { }
        // 粘贴
        MenuItem {
            text: qsTr('Paste')
            shortcut: 'Ctrl+V'
            onTriggered: {
                console.assert(mainwindow);
                if(mainwindow)
                    mainwindow.on_actionPaste_triggered()
            }
        }
        MenuSeparator { }
        // 增加视频轨道
        MenuItem {
            text: qsTr('New Video Track')
            shortcut: 'Ctrl+Y'
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.addVideoTrack()
            }
        }
        // 增加音频轨道
        MenuItem {
            text: qsTr('New Audio Track')
            shortcut: 'Ctrl+U'
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.addAudioTrack()
            }
        }
        // 删除轨道
        MenuItem {
            text: qsTr('Delete Track')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.removeTrack()
            }
        }
        MenuSeparator { }
        // 轨道关闭视频
        MenuItem {
            text: qsTr('Hide')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.toggleTrackHidden(currentTrack)
            }
        }
        // 轨道关闭音频
        MenuItem {
            text: qsTr('Mute')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.toggleTrackMute(currentTrack)
            }
        }
    }


    // 轨道模型
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
            selection: timeline ? timeline.selection : 0
            onClipClicked: {
                currentTrack = track.DelegateModel.itemsIndex
                console.assert(timeline);
                if(timeline){
                    timeline.setSelection([clip.DelegateModel.itemsIndex], currentTrack, false)
                }
                root.clipClicked()
                toolbar.hasClipOrTrackSelected = true

            }

            onClipDragged: {
                // This provides continuous scrolling at the left/right edges.
                console.assert(scrollView);
                if(!scrollView) return;
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
                var s = timeline ? timeline.timecode(Math.abs(delta)) : ''
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
                console.assert(tracksRepeater);
                if (tracksRepeater && i >= 0  && i < tracksRepeater.count) {
                    var track = tracksRepeater.itemAt(i)
                    clip.reparent(track)
                    clip.trackIndex = track.DelegateModel.itemsIndex
                }
            }
            onCheckSnap: {
                console.assert(tracksRepeater);
                if(!tracksRepeater) return;
                for (var i = 0; i < tracksRepeater.count; i++){
                    console.assert(tracksRepeater.itemAt(i));
                    if(tracksRepeater)
                        tracksRepeater.itemAt(i).snapClip(clip)
                }
            }
            // 轨道被锁定时显示的图片
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
                        console.assert(trackHeaderRepeater.itemAt(index));
                        if(trackHeaderRepeater.itemAt(index))
                            trackHeaderRepeater.itemAt(index).pulseLockButton()
                    }
                }
            }
        }
    }

    // 连接 timelineDock.cpp的槽函数
    Connections {
        target: timeline
        onPositionChanged: if (!stopScrolling) Logic.scrollIfNeeded()
        onDragging: Logic.dragging(pos, duration)
        onDropped: Logic.dropped()
        onDropAccepted: Logic.acceptDrop(xml)
        onSelectionChanged: {
            console.assert(timeline);
            if(!timeline)   return;
            cornerstone.selected = timeline.isMultitrackSelected()
            // var selectedTrack = timeline.selectedTrackIndex()
            // for (var i = 0; i < trackHeaderRepeater.count; i++)
            //     trackHeaderRepeater.itemAt(i).selected = (i === selectedTrack)

            var isAClipSelected = timeline.isAClipSelected()
            if (isAClipSelected === true) {//如果选中clip则清除trackhead的选中状态
                for (var i = 0; i < trackHeaderRepeater.count; i++) {
                    trackHeaderRepeater.itemAt(i).selected = false
                }
            }
            if (isAClipSelected === false)
                toolbar.hasClipOrTrackSelected = false
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
            console.assert(headerScrollView);
            if(headerScrollView)
                headerScrollView.flickableItem.contentY = scrollView.flickableItem.contentY
        }
        onContentXChanged:
        {
//            rulerScrollView.flickableItem.contentX =  scrollView.flickableItem.contentX
        }
    }

    Connections {
        target: headerScrollView.flickableItem
        onContentYChanged:
        {
            console.assert(scrollView);
            if(scrollView)
                scrollView.flickableItem.contentY = headerScrollView.flickableItem.contentY
        }
    }


    // This provides continuous scrolling at the left/right edges.
    // 拖放 clip时滚动条的定时器
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
            console.assert(scrollView);
            if(!scrollView) return;
            scrollView.flickableItem.contentX += delta
            if (scrollView.flickableItem.contentX <= 0)
                stop()
        }
    }

}
