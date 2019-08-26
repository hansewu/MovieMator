/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
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

import QtQuick 2.0
import QtQml.Models 2.1
import 'Track.js' as Logic

Rectangle {
    id: trackRoot
    // 轨道模型
    property alias model: trackModel.model
    // 轨道模型中轨道的序号
    property alias rootIndex: trackModel.rootIndex
    // 轨道是否是音频轨道
    property bool isAudio
    // 轨道是否是视频轨道
    property bool isVideo
    // 轨道是否是滤镜轨道
    property bool isFilter
    // 轨道类型？？？
    // 没有地方用到？？？
    property int trackType
    // 时间线缩放因子
    property real timeScale: 1.0
    // 添加的占位符？？？
    property bool placeHolderAdded: false
    // 是否是当前轨道
    property bool isCurrentTrack: false
    // 轨道是否被锁定
    property bool isLocked: false
    // 选中的各个 clip（选中的多个剪辑）
    property var selection

    // 轨道上的clip被点击时发出的信号
    signal clipClicked(var clip, var track)
    // clip从轨道被拖动时发出的信号
    signal clipDragged(var clip, int x, int y)
    // clip被放置到轨道上时发出的信号
    signal clipDropped(var clip)
    // 轨道上的 clip被放置到其他轨道上时发出的信号
    signal clipDraggedToTrack(var clip, int direction)
    // ？？？snap？
    signal checkSnap(var clip)

    // 重画轨道上的波形
    function redrawWaveforms() {
        console.assert(repeater);
        if(!repeater) return;
        for (var i = 0; i < repeater.count; i++){
            console.assert(repeater.imteAt(i));
            if(repeater.itemAt(i))
                repeater.itemAt(i).generateWaveform()
        }
    }

    // ？？？
    function snapClip(clip) {
        Logic.snapClip(clip, repeater)
    }

    // ？？？
    function snapDrop(clip) {
        Logic.snapDrop(clip, repeater)
    }

    // 轨道 index上的 clip
    function clipAt(index) {
        console.assert(repeater);
        if(!repeater) return;
        return repeater.itemAt(index)
    }

    color: 'transparent'
    width: clipRow.width

    // 轨道上的 Clip模型
    DelegateModel {
        id: trackModel
        Clip {
            // 当 model.xxx的值是 undefined时使用 ||''来消除警告
            clipName: model.name || ''
            clipResource: model.resource || ''
            clipDuration: model.duration || ''
            mltService: model.mlt_service || ''
            inPoint: model.in || 0
            outPoint: model.out || 0
            isBlank: model.blank || false
            isAudio: model.audio || false
            isText: model.text || false
            textThumbnail: model.thumbnail || ''
            isTransition: model.isTransition || false
            audioLevels: model.audioLevels || ''
            width: model.duration * timeScale || 0
            height: trackRoot.height
            trackIndex: trackRoot.DelegateModel.itemsIndex
            fadeIn: model.fadeIn || 0
            fadeOut: model.fadeOut || 0
            hash: model.hash || ''
            speed: model.speed || 1.0
            selected: trackRoot.isCurrentTrack && trackRoot.selection.indexOf(index) !== -1
            hasFilter: model.hasFilter || false
            isAnimSticker: model.isAnimSticker || false

            onClicked: trackRoot.clipClicked(clip, trackRoot);
            onMoved: {
                var fromTrack = clip.originalTrackIndex
                var toTrack = clip.trackIndex
                var clipIndex = clip.originalClipIndex
                var frame = Math.round(clip.x / timeScale)

                // Remove the placeholder inserted in onDraggedToTrack
                if (placeHolderAdded) {
                    placeHolderAdded = false
                    if (fromTrack === toTrack) {
                        multitrack.reload(true)
                    } else {
                        trackModel.items.remove(clipIndex, 1)
                    }
                }
                console.assert(timeline);
                if (timeline && !timeline.moveClip(fromTrack, toTrack, clipIndex, frame))
                    clip.x = clip.originalX
            }
            onDragged: {
                console.assert(timeline);
                if (toolbar.scrub && timeline) {
                    root.stopScrolling = false
                    timeline.position = Math.round(clip.x / timeScale)
                }
                // Snap if Alt key is not down.
                if (!(mouse.modifiers & Qt.AltModifier) && toolbar.snap)
                    trackRoot.checkSnap(clip)
                // Prevent dragging left of multitracks origin.
                clip.x = Math.max(0, clip.x)
                var mapped = trackRoot.mapFromItem(clip, mouse.x, mouse.y)
                trackRoot.clipDragged(clip, mapped.x, mapped.y)
            }
            onTrimmingIn: {
                var originalDelta = delta
                if (!(mouse.modifiers & Qt.AltModifier) && toolbar.snap && !toolbar.ripple)
                    delta = Logic.snapTrimIn(clip, delta)
                if (delta != 0) {
                    console.assert(timeline);
                    if (timeline && timeline.trimClipIn(trackRoot.DelegateModel.itemsIndex,
                                            clip.DelegateModel.itemsIndex, delta, toolbar.ripple)) {
                        // Show amount trimmed as a time in a "bubble" help.
                        var s = timeline.timecode(Math.abs(clip.originalX))
                        s = '%1%2 = %3'.arg((clip.originalX < 0)? '-' : (clip.originalX > 0)? '+' : '')
                                       .arg(s.substring(3))
                                       .arg(timeline.timecode(clipDuration))
                        bubbleHelp.show(clip.x, trackRoot.y + trackRoot.height, s)
                    } else {
                        clip.originalX -= originalDelta
                    }
                }
            }
            onTrimmedIn: {
                multitrack.notifyClipIn(trackRoot.DelegateModel.itemsIndex, clip.DelegateModel.itemsIndex)
                // Notify out point of clip A changed when trimming to add a transition.
                if (clip.DelegateModel.itemsIndex > 1 && repeater.itemAt(clip.DelegateModel.itemsIndex - 1).isTransition)
                    multitrack.notifyClipOut(trackRoot.DelegateModel.itemsIndex, clip.DelegateModel.itemsIndex - 2)
                bubbleHelp.hide()
            }
            onTrimmingOut: {
                var originalDelta = delta
                if (!(mouse.modifiers & Qt.AltModifier) && toolbar.snap && !toolbar.ripple)
                    delta = Logic.snapTrimOut(clip, delta)
                if (delta != 0) {
                    console.assert(timeline);
                    if (timeline && timeline.trimClipOut(trackRoot.DelegateModel.itemsIndex,
                                             clip.DelegateModel.itemsIndex, delta, toolbar.ripple)) {
                        // Show amount trimmed as a time in a "bubble" help.
                        var s = timeline.timecode(Math.abs(clip.originalX))
                        s = '%1%2 = %3'.arg((clip.originalX < 0)? '+' : (clip.originalX > 0)? '-' : '')
                                       .arg(s.substring(3))
                                       .arg(timeline.timecode(clipDuration))
                        bubbleHelp.show(clip.x + clip.width, trackRoot.y + trackRoot.height, s)
                    } else {
                        clip.originalX -= originalDelta
                    }
                }
            }
            onTrimmedOut: {
                multitrack.notifyClipOut(trackRoot.DelegateModel.itemsIndex, clip.DelegateModel.itemsIndex)
                // Notify in point of clip B changed when trimming to add a transition.
                if (clip.DelegateModel.itemsIndex + 2 < repeater.count && repeater.itemAt(clip.DelegateModel.itemsIndex + 1).isTransition)
                    multitrack.notifyClipIn(trackRoot.DelegateModel.itemsIndex, clip.DelegateModel.itemsIndex + 2)
                bubbleHelp.hide()
            }
            onDraggedToTrack: {
                if (!placeHolderAdded) {
                    placeHolderAdded = true
                    trackModel.items.insert(clip.DelegateModel.itemsIndex, {
                        'name': '',
                        'resource': '',
                        'duration': clip.clipDuration,
                        'mlt_service': '<producer',
                        'in': 0,
                        'out': clip.clipDuration - 1,
                        'blank': true,
                        'audio': false,
                        'isTransition': false,
                        'fadeIn': 0,
                        'fadeOut': 0
                    })
                }
            }
            onClipdropped: placeHolderAdded = false

            Component.onCompleted: {
                moved.connect(trackRoot.clipDropped)
                clipdropped.connect(trackRoot.clipDropped)
                draggedToTrack.connect(trackRoot.clipDraggedToTrack)
            }
        }
    }

    // 显示轨道上的 clip
    Row {
        id: clipRow
        Repeater { id: repeater; model: trackModel }
    }
}
