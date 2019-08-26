/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: wyl <wyl@pylwyl.local>
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
import QtQuick.Controls 1.0
import MovieMator.Controls 1.0
import QtGraphicalEffects 1.0
import QtQml.Models 2.2
import QtQuick.Window 2.2
import 'Track.js' as Logic

Rectangle {
    id: clipRoot
    // clip名称
    property string clipName: ''
    // clip的资源
    property string clipResource: ''
    // mltService？？？
    property string mltService: ''
    // 入点
    property int inPoint: 0
    // 出点
    property int outPoint: 0
    // clip的持续时间（时长）
    property int clipDuration: 0
    // clip是否为空白
    property bool isBlank: false
    // clip是否为音频
    property bool isAudio: false
    // clip是否为转场
    property bool isTransition: false
    // clip是否为文本
    property bool isText: false
    // 文本缩略图？
    property string textThumbnail: ''
    // 音量高低？
    property var audioLevels
    // 淡入时长
    property int fadeIn: 0
    // 淡出时长
    property int fadeOut: 0
    // clip所在的轨道序号
    property int trackIndex
    // clip所在的原始轨道序号
    property int originalTrackIndex: trackIndex
    // clip的原始剪辑 clip序号
    property int originalClipIndex: index
    // clip的原始位置（x坐标）
    property int originalX: x
    // clip是否被选中
    property bool selected: false
    // clip的 hash码
    property string hash: ''
    // clip播放速度
    property double speed: 1.0
    // clip上是否有滤镜
    property bool hasFilter: false
    // clip是否为动画标签？？？？？
    property bool isAnimSticker: false


    // clip单击信号
    signal clicked(var clip)
    // clip移动信号
    signal moved(var clip)
    // clip被拖动信号
    signal dragged(var clip, var mouse)
    // clip被放置信号
    signal clipdropped(var clip)
    // clip被拖放到轨道信号
    signal draggedToTrack(var clip, int direction)
    // 修剪？？？
    signal trimmingIn(var clip, real delta, var mouse)
    // 修剪？？？
    signal trimmedIn(var clip)
    // 修剪？？？
    signal trimmingOut(var clip, real delta, var mouse)
    // 修剪？？？
    signal trimmedOut(var clip)

    // 调色板
    SystemPalette { id: activePalette }
    gradient: Gradient {
        GradientStop {
            id: gradientStop
            position: 0.00
//            color: Qt.lighter(getColor())
            color: getColor()
        }
        GradientStop {
            id: gradientStop2
            position: 1.0
            color: getColor()
        }
    }


    /*
     * clip背景框
     */
    border.color: selected? 'white' : backgroundColor // 'transparent'
    border.width: isBlank? 0 : selected ? 2 : 1
    clip: true
    Drag.active: mouseArea.drag.active
    Drag.proposedAction: Qt.MoveAction
    opacity: Drag.active? 0.5 : 1.0

    // clip的背景色
    function getColor() {
//        return isBlank? 'transparent' : isTransition? 'mediumpurple' : isAudio? 'darkseagreen' : root.moviematorBlue
        // return isBlank? 'transparent' : isTransition? '#8c953f' : isAudio? '#2a5733' : '#2f798f'
        return isBlank? 'transparent' : isTransition? '#8c953f' : '#0F7267'
    }

    // clip被选中时的背景色
    function getSelectedColor() {
        // return isBlank? 'transparent' : isTransition? '#8c953f': isAudio? '#419f51' : '#2eb9df'
        return isBlank? 'transparent' : isTransition? '#8c953f': '#C0482C'
    }

    // 重定位所在轨道为轨道 track
    function reparent(track) {
        console.assert(track);
        if(!track) return;
        Drag.active = false
        parent = track
        isAudio = track.isAudio
        height = track.height
     //   generateWaveform()
    }

    // 终止拖动
    function cancelDrag() {
        Drag.active = false
    }

    // 生成 clip的波形
    function generateWaveform() {
        // This is needed to make the model have the correct count.
        // Model as a property expression is not working in all cases.
        console.assert(waveform.maxWidth>0);
        if(waveform.maxWidth>0)
            waveformRepeater.model = Math.ceil(waveform.innerWidth / waveform.maxWidth)
//        for (var i = 0; i < waveformRepeater.count; i++)
        // console.assert(waveformRepeater.count>0);   // waveformRepeater.count可以为 0
        // console.assert(waveformRepeater.itemAt(0));  // waveformRepeater可以没有元素
        if(waveformRepeater.count>0 && waveformRepeater.itemAt(0))
            waveformRepeater.itemAt(0).update()
    }

    // clip的缩略图
    // 传递 time调用C++函数生成缩略图缓存
    function imagePath(time) {
        // 当 hash、mltService、clipResource都没有有效值时不应该有缩略图，消除警告
        if (isAudio || isBlank || isTransition || (hash=='' && mltService=='' && clipResource=='')) {
            return ''
        } else {
            return 'image://thumbnail/' + hash + '/' + mltService + '/' + clipResource + '#' + time
        }
    }

    // 响应音量调整信号的槽函数
    onAudioLevelsChanged: generateWaveform()

    // 出点位置的 clip缩略图
    Image {
        id: outThumbnail
        visible: settings.timelineShowThumbnails
        anchors.right: parent.right
        anchors.top: parent.top
        //anchors.topMargin: parent.border.width
        //anchors.rightMargin: parent.border.width
        anchors.bottom: parent.bottom
        //anchors.bottomMargin: parent.border.width
        anchors.margins: 6
        width: height * 16.0/9.0
        fillMode: Image.PreserveAspectFit
        source: isText ? textThumbnail : imagePath(outPoint)
    }

    // 入点位置的 clip缩略图
    Image {
        id: inThumbnail
        visible: settings.timelineShowThumbnails
        anchors.left: parent.left
        anchors.top: parent.top
        //anchors.topMargin: parent.border.width
        //anchors.leftMargin: parent.border.width
        anchors.bottom: parent.bottom
        //anchors.bottomMargin: parent.border.width
        anchors.margins: 6
        width: height * 16.0/9.0
        fillMode: Image.PreserveAspectFit
        source: isText ? textThumbnail : imagePath(inPoint)
    }

    // clip中间位置的缩略图
    Image {
        id: centerThumbnail
        visible: settings.timelineShowThumbnails
        anchors.left: parent.left
        anchors.top: parent.top
        //anchors.topMargin: parent.border.width
        //anchors.leftMargin: parent.width/2 - width / 2

        anchors.bottom: parent.bottom
        //anchors.bottomMargin: parent.border.width
        anchors.margins: 6
        width: height * 16.0/9.0
        fillMode: Image.PreserveAspectFit
        source: isText ? textThumbnail : imagePath((outPoint+inPoint)/2)
    }

    // 转场
    TimelineTransition {
        visible: isTransition
        anchors.fill: parent
        property string color: '#c3c998'//isAudio? 'darkseagreen' : root.moviematorBlue
        colorA: color
        colorB: color//clipRoot.selected ? Qt.darker(color) : Qt.lighter(color)
    }

    // clip被选中时的矩形边框
    Rectangle { // selected border
        z: 1
        visible: selected && !isBlank
        anchors.fill: parent
        color: 'transparent'
        border.width: 1     // 2
        border.color: '#DE9690'  // 'white'
    }

    // clip的矩形边框
    Rectangle { // border
        z: 1
        visible: !selected && !isBlank
        anchors.fill: parent
        color: 'transparent'
        border.width: 1
        border.color: '#1A4540'     // backgroundColor
    }

    // 波形
    Row {
        id: waveform
        visible: !isBlank && settings.timelineShowWaveforms
        height: isAudio? parent.height : parent.height / 2
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: parent.border.width
        opacity: 0.7
        property int maxWidth: 10000
        property int innerWidth: clipRoot.width - clipRoot.border.width * 2

        Repeater {
            id: waveformRepeater
            TimelineWaveform {
                width: Math.min(waveform.innerWidth, waveform.maxWidth)
                height: waveform.height
                fillColor: getColor()
                property int channels: 2
                inPoint: Math.round((clipRoot.inPoint + index * waveform.maxWidth / timeScale) * speed) * channels
                outPoint: inPoint + Math.round(width / timeScale * speed) * channels
                levels: audioLevels
            }
        }
    }



//    Rectangle {
//        // audio peak line
//        width: parent.width - parent.border.width * 2
//        visible: !isBlank && !isTransition
//        height: 1
//        anchors.left: parent.left
//        anchors.bottom: parent.bottom
//        anchors.leftMargin: parent.border.width
//        anchors.bottomMargin: waveform.height * 0.9
//        color: Qt.darker(parent.color)
//        opacity: 0.7
//    }

//    Rectangle {
//        // text background
//        color: 'lightgray'
//        visible: !isBlank && !isTransition
//        opacity: 0.7
//        anchors.top: parent.top
//        anchors.left: parent.left
//        anchors.topMargin: parent.border.width
//        anchors.leftMargin: parent.border.width +
//            ((isAudio || !settings.timelineShowThumbnails) ? 0 : inThumbnail.width)
//        width: label.width + 2
//        height: label.height
//    }


//    Image {
//        id: fxIndicator
//        visible: hasFilter
//        anchors.left: parent.left
//        anchors.top: parent.top
//        anchors.topMargin: parent.border.width + 2
//        anchors.leftMargin: parent.border.width +
//                            ((isAudio || !settings.timelineShowThumbnails) ? 0 : inThumbnail.width) + 2

//        fillMode: Image.PreserveAspectFit
//        source: 'qrc:///icons/light/32x32/show-filters.png'
//    }

    // 显示 clip名称的文本框
    Text {
        id: label
        text: clipName
        visible: !isBlank && !isTransition
        font.pointSize: 9       // 10
        anchors {
            top: parent.top
            left: parent.left
            topMargin: parent.border.width + 2
            leftMargin: parent.border.width +
                ((isAudio || !settings.timelineShowThumbnails || isAnimSticker) ? 0 : inThumbnail.width) + 1
        }
        color: '#C7F3ED'    // 'black'
    }



    states: [
        State {
            name: 'normal'
            when: !clipRoot.selected
            PropertyChanges {
                target: clipRoot
                z: 0
            }
        },
        State {
            name: 'selectedBlank'
            when: clipRoot.selected && clipRoot.isBlank

        },
        State {
            name: 'selected'
            when: clipRoot.selected
            PropertyChanges {
                target: clipRoot
                z: 1
            }
            PropertyChanges {
                target: gradientStop
                color: getSelectedColor()
            }
            PropertyChanges {
                target: gradientStop2
                color: getSelectedColor()
            }
        }
    ]


    // clip的左键选中用的 MouseArea
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: !isBlank
        acceptedButtons: Qt.LeftButton
        //propagateComposedEvents: true
        drag.target:isTransition ? null : parent
        drag.axis: Drag.XAxis
        property int startX
        onPressed: {
            //console.log("onPressed --- ")
            root.stopScrolling = true
            originalX = parent.x
            originalTrackIndex = trackIndex
            originalClipIndex = index
            startX = parent.x
            clipRoot.forceActiveFocus();
            clipRoot.clicked(clipRoot)

            //if (isTransition)
            //    menu.popup()
        }
        onPositionChanged: {
            if (!isTransition)
            {
                console.assert(timeline);
                if (originalTrackIndex == trackIndex && originalClipIndex == index && timeline)
                    originalClipIndex = timeline.removeTransitionOnClipWithUndo(trackIndex, index)
                //console.log("clip onPositionChanged")
                if (mouse.y < 0 && trackIndex > 0)
                    parent.draggedToTrack(clipRoot, -1)
                else if (mouse.y > height && (trackIndex + 1) < root.trackCount)
                    parent.draggedToTrack(clipRoot, 1)
                parent.dragged(clipRoot, mouse)
            }
        }
        onReleased: {
            clipRoot.cancelDrag()
            root.stopScrolling = false
            if (!isTransition /*&& !isText*/)
            {
                parent.y = 0
                var delta = parent.x - startX
                if (Math.abs(delta) >= 1.0 || trackIndex !== originalTrackIndex) {
                    parent.moved(clipRoot)
                    originalX = parent.x
                    originalTrackIndex = trackIndex
                } else {
                    parent.clipdropped(clipRoot)
                }
            }

        }
        onDoubleClicked: {
            console.assert(timeline);
            if(timeline)
                timeline.position = clipRoot.x / multitrack.scaleFactor
        }
        onClicked: {
            console.assert(mainwindow);
            if(mainwindow)
                mainwindow.setMultitrackAsCurrentProducer()
        }
    }

    // Clip的右键操作用的 MouseArea
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        propagateComposedEvents: true
        cursorShape: (trimInMouseArea.drag.active || trimOutMouseArea.drag.active)? Qt.SizeHorCursor :
//                (fadeInMouseArea.drag.active || fadeOutMouseArea.drag.active)? Qt.PointingHandCursor :
            drag.active? Qt.ClosedHandCursor :
            isBlank? Qt.ArrowCursor : Qt.OpenHandCursor
        onClicked: {
            if (!isBlank)
            {
                clipRoot.forceActiveFocus();
                clipRoot.clicked(clipRoot)
            }
            console.assert(mainwindow);
            if(mainwindow){
                mainwindow.setMultitrackAsCurrentProducer()
            }
            menu.popup()
        }
    }

    // 时间线可接收拖放内容的区域
    DropArea {
        id: dropArea
        anchors.fill: parent
        property int filterIndex: 0
        keys: isBlank ? [""] : ["filter/filterindex"]
        onDropped: {
            //当clip未选中时，先选中。因为添加滤镜的操作，是添加到选中的clip上的
            if (!selected)
            {
                //选中鼠标放下的位置的clip
                if (!isBlank)
                {
                    clipRoot.forceActiveFocus();
                    clipRoot.clicked(clipRoot)
                }

                console.assert(mainwindow);
                if (mainwindow)
                {
                    mainwindow.setMultitrackAsCurrentProducer()
                }

            }

            //添加滤镜到当前选中的clip上
            filterIndex = parseInt(drop.getDataAsString("filter/filterindex"))
            console.assert(metadatamodel);
            if (metadatamodel)
            {
                var model   = metadatamodel.get(filterIndex)
                if (model.isAudio)
                {
                    mainwindow.onShowPropertiesAudioFilterDock()
                }
                else
                {
                    mainwindow.onShowPropertiesVideoFilterDock()
                }
                attachedfiltersmodel.add(model)

                // 表示目标区域接受拖拽事件
                // Mac下拖放失败也就是目标区域禁止拖放会有个回弹动画
                drop.acceptProposedAction()
            }

            //当光标不在当先选中的clip上时，则跳转到当先选中的clip上，用于更好的看到添加滤镜后的变化
            console.assert(timeline);
            if (timeline)
            {
                var clipPosition = clipRoot.x / multitrack.scaleFactor
                if (timeline.position < clipPosition ||
                        timeline.position > clipPosition + clipDuration)
                {
                    timeline.position = clipRoot.x / multitrack.scaleFactor
                }
            }
        }
    }

    // 使用的地方都已经被注释了
    TimelineTriangle {
        id: fadeInTriangle
        visible: !isBlank && !isTransition
        width: parent.fadeIn * timeScale
        height: parent.height - parent.border.width * 2
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: parent.border.width
        opacity: 0.5
    }
//    Rectangle {
//        id: fadeInControl
//        enabled: !isBlank && !isTransition
//        anchors.left: fadeInTriangle.width > radius? undefined : fadeInTriangle.left
//        anchors.horizontalCenter: fadeInTriangle.width > radius? fadeInTriangle.right : undefined
//        anchors.top: fadeInTriangle.top
//        anchors.topMargin: -3
//        width: 15
//        height: 15
//        radius: 7.5
//        color: 'black'
//        border.width: 2
//        border.color: 'white'
//        opacity: 0
//        Drag.active: fadeInMouseArea.drag.active
//        MouseArea {
//            id: fadeInMouseArea
//            anchors.fill: parent
//            hoverEnabled: true
//            cursorShape: Qt.PointingHandCursor
//            drag.target: parent
//            drag.axis: Drag.XAxis
//            property int startX
//            property int startFadeIn
//            onEntered: parent.opacity = 0.7
//            onExited: parent.opacity = 0
//            onPressed: {
//                root.stopScrolling = true
//                startX = parent.x
//                startFadeIn = fadeIn
//                parent.anchors.left = undefined
//                parent.anchors.horizontalCenter = undefined
//                parent.opacity = 1
//                // trackRoot.clipSelected(clipRoot, trackRoot) TODO
//            }
//            onReleased: {
//                root.stopScrolling = false
//                if (fadeInTriangle.width > parent.radius)
//                    parent.anchors.horizontalCenter = fadeInTriangle.right
//                else
//                    parent.anchors.left = fadeInTriangle.left
//                bubbleHelp.hide()
//            }
//            onPositionChanged: {
//                if (mouse.buttons === Qt.LeftButton) {
//                    var delta = Math.round((parent.x - startX) / timeScale)
//                    var duration = startFadeIn + delta
//                    timeline.fadeIn(trackIndex, index, duration)

//                    // Show fade duration as time in a "bubble" help.
//                    var s = timeline.timecode(Math.max(duration, 0))
//                    bubbleHelp.show(clipRoot.x, trackRoot.y + clipRoot.height, s.substring(6))
//                }
//            }
//        }
//        SequentialAnimation on scale {
//            loops: Animation.Infinite
//            running: fadeInMouseArea.containsMouse
//            NumberAnimation {
//                from: 1.0
//                to: 0.5
//                duration: 250
//                easing.type: Easing.InOutQuad
//            }
//            NumberAnimation {
//                from: 0.5
//                to: 1.0
//                duration: 250
//                easing.type: Easing.InOutQuad
//            }
//        }
//    }

    // 使用的地方也被注释了
    TimelineTriangle {
        id: fadeOutCanvas
        visible: !isBlank && !isTransition
        width: parent.fadeOut * timeScale
        height: parent.height - parent.border.width * 2
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: parent.border.width
        opacity: 0.5
        transform: Scale { xScale: -1; origin.x: fadeOutCanvas.width / 2}
    }
//    Rectangle {
//        id: fadeOutControl
//        enabled: !isBlank && !isTransition
//        anchors.right: fadeOutCanvas.width > radius? undefined : fadeOutCanvas.right
//        anchors.horizontalCenter: fadeOutCanvas.width > radius? fadeOutCanvas.left : undefined
//        anchors.top: fadeOutCanvas.top
//        anchors.topMargin: -3
//        width: 15
//        height: 15
//        radius: 7.5
//        color: 'black'
//        border.width: 2
//        border.color: 'white'
//        opacity: 0
//        Drag.active: fadeOutMouseArea.drag.active
//        MouseArea {
//            id: fadeOutMouseArea
//            anchors.fill: parent
//            hoverEnabled: true
//            cursorShape: Qt.PointingHandCursor
//            drag.target: parent
//            drag.axis: Drag.XAxis
//            property int startX
//            property int startFadeOut
//            onEntered: parent.opacity = 0.7
//            onExited: parent.opacity = 0
//            onPressed: {
//                root.stopScrolling = true
//                startX = parent.x
//                startFadeOut = fadeOut
//                parent.anchors.right = undefined
//                parent.anchors.horizontalCenter = undefined
//                parent.opacity = 1
//            }
//            onReleased: {
//                root.stopScrolling = false
//                if (fadeOutCanvas.width > parent.radius)
//                    parent.anchors.horizontalCenter = fadeOutCanvas.left
//                else
//                    parent.anchors.right = fadeOutCanvas.right
//                bubbleHelp.hide()
//            }
//            onPositionChanged: {
//                if (mouse.buttons === Qt.LeftButton) {
//                    var delta = Math.round((startX - parent.x) / timeScale)
//                    var duration = startFadeOut + delta
//                    timeline.fadeOut(trackIndex, index, duration)

//                    // Show fade duration as time in a "bubble" help.
//                    var s = timeline.timecode(Math.max(duration, 0))
//                    bubbleHelp.show(clipRoot.x + clipRoot.width, trackRoot.y + clipRoot.height, s.substring(6))
//                }
//            }
//        }
//        SequentialAnimation on scale {
//            loops: Animation.Infinite
//            running: fadeOutMouseArea.containsMouse
//            NumberAnimation {
//                from: 1.0
//                to: 0.5
//                duration: 250
//                easing.type: Easing.InOutQuad
//            }
//            NumberAnimation {
//                from: 0.5
//                to: 1.0
//                duration: 250
//                easing.type: Easing.InOutQuad
//            }
//        }
//    }

    // Add -Start
    // 可以一直显示裁剪时候的红色边界
    // ①专门用来显示在左边裁剪的边界
    Rectangle {
        id: trimInBracket
        anchors.left: parent.left
        anchors.leftMargin: 0
        height: parent.height
        width: 5
        color: "red"
        visible: false
        
        Rectangle {
            id: trimInBracketTop
            anchors.top: parent.top
            anchors.left: parent.right
            height: parent.width
            width: parent.width
            color: parent.color
        }

        Rectangle {
            id: trimInBracketBottom
            anchors.bottom: parent.bottom
            anchors.left: parent.right
            height: parent.width
            width: parent.width
            color: parent.color
        }
    }

    // 显示左边裁剪的边界
    function showTrimInBracket(){
        trimInBracket.visible = true
        trimInBracket.z = 1
        trimInBracketTop.z = 1
        trimInBracketBottom.z = 1
    }
    // 隐藏左边裁剪的边界
    function hideTrimInBracket(){
        trimInBracket.visible = false
        trimInBracket.z = 0
        trimInBracketTop.z = 0
        trimInBracketBottom.z = 0
    }
    
    // ②专门用来显示在右边裁剪的边界
    Rectangle {
        id: trimOutBracket
        anchors.right: parent.right
        anchors.rightMargin: 0
        height: parent.height
        width: 5
        color: 'red'
        visible: false

        Rectangle {
            id: trimOutBracketTop
            anchors.top: parent.top
            anchors.right: parent.left
            height: parent.width
            width: parent.width
            color: parent.color
        }

        Rectangle {
            id: trimOutBracketBottom
            anchors.bottom: parent.bottom
            anchors.right: parent.left
            height: parent.width
            width: parent.width
            color: parent.color
        }
    }

    // 显示右边裁剪的边界
    function showTrimOutBracket(){
        trimOutBracket.visible = true
        trimOutBracket.z = 1
        trimOutBracketTop.z = 1
        trimOutBracketBottom.z = 1
    }
    // 隐藏右边裁剪的边界
    function hideTrimOutBracket(){
        trimOutBracket.visible = false
        trimOutBracket.z = 0
        trimOutBracketTop.z = 0
        trimOutBracketBottom.z = 0
    }
    // Add -End

    // 左侧入点的裁剪框框
    Rectangle {
        id: trimIn
        enabled: !isBlank && !isTransition
        anchors.left: parent.left
        anchors.leftMargin: 0
        height: parent.height
        width: 5
        // color: isAudio? 'green' : 'lawngreen'
        opacity: 0
        Drag.active: trimInMouseArea.drag.active
        Drag.proposedAction: Qt.MoveAction

        MouseArea {
            id: trimInMouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.SizeHorCursor
            drag.target: parent
            drag.axis: Drag.XAxis
            property double startX

            onPressed: {
                root.stopScrolling = true
                startX = mapToItem(null, x, y).x
                originalX = 0 // reusing originalX to accumulate delta for bubble help
                parent.anchors.left = undefined
                
                // Add 显示边界框
                showTrimInBracket()
            }
            onReleased: {
                root.stopScrolling = false
                parent.anchors.left = clipRoot.left
                clipRoot.trimmedIn(clipRoot)
                parent.opacity = 0

                // Add 隐藏边界框
                hideTrimInBracket()
            }
            onPositionChanged: {
                if (mouse.buttons === Qt.LeftButton) {
                    // Add 显示边界框
                    showTrimInBracket()

                    var newX = mapToItem(null, x, y).x
                    var delta = Math.round((newX - startX) / timeScale)

                    if (Math.abs(delta) > 0) {
                        if (clipDuration + originalX + delta > 0)
                            originalX += delta
                        clipRoot.trimmingIn(clipRoot, delta, mouse)
                        if (Logic.snapTrimIn(clipRoot, delta) !== 0)
                            startX = newX
                    }
                }
            }
            onEntered: showTrimInBracket()  // parent.opacity = 0.5
            onExited: hideTrimInBracket()   // parent.opacity = 0
        }
    }

    // 右侧出点的裁剪框框
    Rectangle {
        id: trimOut
        enabled: !isBlank && !isTransition
        anchors.right: parent.right
        anchors.rightMargin: 0
        height: parent.height
        width: 5
        // color: 'red'
        opacity: 0
        Drag.active: trimOutMouseArea.drag.active
        Drag.proposedAction: Qt.MoveAction

        MouseArea {
            id: trimOutMouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.SizeHorCursor
            drag.target: parent
            drag.axis: Drag.XAxis
            property int duration

            onPressed: {
                root.stopScrolling = true
                duration = clipDuration
                originalX = 0 // reusing originalX to accumulate delta for bubble help
                parent.anchors.right = undefined

                // Add 显示边界框
                showTrimOutBracket()
            }
            onReleased: {
                root.stopScrolling = false
                parent.anchors.right = clipRoot.right
                clipRoot.trimmedOut(clipRoot)

                // Add 隐藏边界框
                hideTrimOutBracket()
            }
            onPositionChanged: {
                if (mouse.buttons === Qt.LeftButton) {
                    // Add 显示边界框
                    showTrimOutBracket()

                    var newDuration = Math.round((parent.x + parent.width) / timeScale)
                    var delta = duration - newDuration
                    if (Math.abs(delta) > 0) {
                        if (clipDuration - originalX - delta > 0)
                            originalX += delta
                        clipRoot.trimmingOut(clipRoot, delta, mouse)
                        duration = newDuration
                    }
                }
            }
            onEntered: showTrimOutBracket() // parent.opacity = 0.5
            onExited: hideTrimOutBracket()  // parent.opacity = 0
        }
    }

    // clip如果有滤镜的话，在 clip左上角显示的 fx图标的黑色图片
    Rectangle {
        id :showFilter
        visible: !isBlank && hasFilter
        height: 20
        width: 20
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: parent.border.width
        opacity: 0.6
        color: 'black'
        radius: 3

        // fx图标
        Image {
            id: filterImage
            source: 'qrc:///icons/light/32x32/show-filters.png'
            height:1
            z:1
            anchors.fill: parent
        }

    }

    // 在 clip下方显示关键帧的灰色背景
    Rectangle {
        visible: !isBlank && selected && currentFilter && currentFilter.keyframeNumber > 0
        height: parent.height / 2
        width: parent.width
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: parent.border.width
        opacity: 0.7
        color: 'grey'


        //MouseArea {
        //    anchors.fill: parent
        //    acceptedButtons: Qt.LeftButton
        //}

        Repeater {
            id: keyFrameRepeater
            // 只有 Rectangle显示时才有 model，消除 currentFilter的 Reference Error警告
            model: (parent.visible && currentFilter) ? currentFilter.keyframeNumber : 0
            anchors.verticalCenter: parent.verticalCenter


            KeyframeIndicator{
                height: parent.height / 2
                width: height
                anchors.verticalCenter: parent.verticalCenter
                visible: !isBlank && selected &&currentFilter && (currentFilter.getKeyFrame(index) !== -1)
                x: (currentFilter?currentFilter.getKeyFrame(index):0) *multitrack.scaleFactor - width/2
                frameNumber: currentFilter ? currentFilter.getKeyFrame(index) : -1
                rotation: 45
            }
        }
    }

//    Connections{
//        target: currentFilter
//        onKeyFrameChanged:{
////            var  frameNumber = currentFilter.cache_getKeyFrameNumber()
////            firstKeyFrameRect.visible = !isBlank && frameNumber
////            if(frameNumber)
////            {
////                console.log("Clip.qml onKeyFrameChanged is called, keyframe=")
////              //  firstKeyFrameRect.visible = !isBlank && frameNumber// This is available in all editors.
////                var frame = currentFilter.getFristKeyFrame()

////                firstKeyFrameRect.anchors.leftMargin = clipRoot.border.width+frame
////                console.log(firstKeyFrameRect.anchors.leftMargin);
////            }

//        }
//        onAddKeyFrame:{
//            var keyNumber = currentFilter.cache_getKeyFrameNumber()
//            keyFrameRepeater.model = keyNumber
//        }

//        onRemoveKeyFrame:{
//            var keyNumber = currentFilter.cache_getKeyFrameNumber()
//            keyFrameRepeater.model = keyNumber
//        }

//    }

    // 右键 clip弹出的菜单
    /*
    Menu {
        id: menu
//        MenuItem {
//            visible: !isTransition
//            text: qsTr('Remove')
//            onTriggered: timeline.remove(trackIndex, index)
//        }
        // 裁剪菜单
        MenuItem {
            visible: !isBlank && !isTransition
            text: qsTr('Cut')
            onTriggered: {
                console.assert(timeline);
                if (!trackRoot.isLocked && timeline) {
                    timeline.copyClip(trackIndex, index)
//                    timeline.remove(trackIndex, index)
                    timeline.lift(trackIndex, index)
                } else {
                    root.pulseLockButtonOnTrack(currentTrack)
                }
            }
        }
        // 复制菜单
        MenuItem {
            visible: !isBlank && !isTransition
            text: qsTr('Copy')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.copyClip(trackIndex, index)
            }
        }
        // 菜单分割线
        MenuSeparator {
            visible: !isBlank && !isTransition
        }
        // 移除菜单
        MenuItem {
            visible: !isBlank && !isTransition
            text: qsTr('Remove')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.lift(trackIndex, index)
            }
        }
        // 波纹删除菜单
        // 删除非转场的 clip（包括空白的）
        MenuItem {
            visible: !isTransition
            text: qsTr('Ripple Delete')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.remove(trackIndex, index)
            }
        }

        // 菜单分割线
        MenuSeparator {
            visible: !isBlank && !isTransition
        }
        // 在播放游标处切分菜单
        MenuItem {
            visible: !isBlank && !isTransition
            text: qsTr('Split At Playhead (S)')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.splitClip(trackIndex, index)
            }
        }
        //MenuItem {
        //    visible: !isBlank && !isTransition
        //    text: qsTr('Rebuild Audio Waveform')
        //    onTriggered: timeline.remakeAudioLevels(trackIndex, index)
        //}
        //onPopupVisibleChanged: {
        //    if (visible && application.OS !== 'OS X' && __popupGeometry.height > 0) {
        //        // Try to fix menu running off screen. This only works intermittently.
        //        menu.__yOffset = Math.min(0, Screen.height - (__popupGeometry.y + __popupGeometry.height + 40))
        //        menu.__xOffset = Math.min(0, Screen.width - (__popupGeometry.x + __popupGeometry.width))
        //    }
        //}

        // 时间线工具转场属性改为转场设置
        MenuItem {
//            text: qsTr(!isTransition ? 'Properties' : 'Transition Settings')     // qsTr('Properties')
            text: !isTransition ? qsTr('Properties') : qsTr('Transition Settings')
            visible: !isText && !isBlank
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.onShowProperties(trackIndex, index)
            }
        }
        // 移除菜单（只对转场有效）
        MenuItem {
            visible: isTransition
            text:qsTr('Remove')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.removeTransition(trackIndex, index)
            }
        }


       // 文本设置菜单？？？
       // 文本文件？？？
       MenuItem {
           visible: isText
            text:qsTr('Text Settings')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.onTextSettings(trackIndex, index)
            }
       }

//       MenuItem {
//           visible: !isBlank && !isTransition
//            text:qsTr('Export as template')
//            onTriggered: timeline.exportAsTemplate(trackIndex, index)
//       }

    }
*/
    Menu {
        id: menu
        // 切割
        MenuItem {
            visible: !isTransition
            text: qsTr('Split')
            shortcut: 'S'
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.splitClip(currentTrack)
            }
        }
        // 插入资源
        MenuItem {
            visible: !isBlank && !isTransition
            text: qsTr("Insert");
            shortcut: 'V'
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.insert(currentTrack)
            }
        }
        // 追加资源
        MenuItem {
            visible: isBlank
            text: qsTr('Append');
            shortcut: 'A'
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.append(currentTrack)
            }
        }
        MenuSeparator {
            visible: !isTransition
        }
        // 删除
        MenuItem {
            visible: !isBlank && !isTransition
            text: qsTr('Delete')
            shortcut: 'delete'
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.lift(currentTrack, timeline.selection[0])//timeline.remove(currentTrack, selection[0])
            }
        }
        // 删除并关闭空隙
        MenuItem {
            visible: !isTransition
            text: qsTr('Ripple Delete')
            onTriggered: {
                console.assert(timeline);
                if(timeline) {
                    timeline.remove(trackIndex, index)
                }
            }
        }
        MenuSeparator {
            visible: !isTransition
        }
        // 剪切
        MenuItem {
            visible: !isBlank && !isTransition
            text: qsTr('Cut')
            shortcut: 'Ctrl+X'
            onTriggered: {
                console.assert(timeline);
                if (!trackRoot.isLocked && timeline) {
                    timeline.copyClip(trackIndex, index)
//                    timeline.remove(trackIndex, index)
                    timeline.lift(trackIndex, index)
                } else {
                    root.pulseLockButtonOnTrack(currentTrack)
                }
            }
        }
        // 复制
        MenuItem {
            visible: !isBlank && !isTransition
            text: qsTr('Copy')
            shortcut: 'Ctrl+C'
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.copyClip(trackIndex, index)
            }
        }
        // 粘贴
        MenuItem {
            visible: !isTransition
            text: qsTr('Paste')
            shortcut: 'Ctrl+V'
            onTriggered: {
                console.assert(mainwindow);
                if(mainwindow)
                    mainwindow.on_actionPaste_triggered()
            }
        }

        MenuSeparator {
            visible: isBlank
        }
        // 增加视频轨道
        MenuItem {
            visible: isBlank
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
            visible: isBlank
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
            visible: isBlank
            text: qsTr('Delete Track')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.removeTrack()
            }
        }

        MenuSeparator {
            visible: !isTransition
        }
        // 缩放到合适
        MenuItem {
            visible: !isBlank && !isTransition
            text: qsTr('Zoom to Fit')
            onTriggered: {
                toolbar.actionShowAllClips.trigger()
            }
        }
        // 添加滤镜
        MenuItem {
            visible: !isBlank && !isTransition
            text: qsTr('Add Filter')
            onTriggered: {
                toolbar.actionAddFilter.trigger()
            }
        }

        // 轨道关闭视频
        MenuItem {
            visible: isBlank
            text: qsTr('Hide')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.toggleTrackHidden(trackIndex)
            }
        }
        // 轨道关闭音频
        MenuItem {
            visible: isBlank
            text: qsTr('Mute')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.toggleTrackMute(trackIndex)
            }
        }


        //转场菜单
        MenuItem {
//            text: qsTr(!isTransition ? 'Properties' : 'Transition Settings')     // qsTr('Properties')
            text: !isTransition ? qsTr('Properties') : qsTr('Transition Settings')
            visible: isTransition
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.onShowProperties(trackIndex, index)
            }
        }
        // 移除菜单（只对转场有效）
        MenuItem {
            visible: isTransition
            text:qsTr('Remove')
            onTriggered: {
                console.assert(timeline);
                if(timeline)
                    timeline.removeTransition(trackIndex, index)
            }
        }
    }
}
