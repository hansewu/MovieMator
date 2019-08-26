/*
 * Copyright (c) 2013-2014 Meltytech, LLC
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import 'Timeline.js' as Logic

import "../views/filter"

// 时间线工具栏菜单
ToolBar {
    // ？？？？
    property alias ripple: rippleButton.checked
    // ？？？？
    property alias scrub: scrubButton.checked
    // ？？？？
    property alias snap: snapButton.checked
    // checkBox被选中的颜色？？？
    property color checkedColor: Qt.rgba(activePalette.highlight.r, activePalette.highlight.g, activePalette.highlight.b, 0.3)
    // 时间线缩放的系数
    property alias scaleSliderValue: scaleSlider.value
    // 是否有轨道或者剪辑被选中
    property bool hasClipOrTrackSelected:false

    // Add -时间线以鼠标为中心缩放，参数传递给 scaleSlider
    // wheelx：鼠标（滚轮）位置
    // scaleValue：缩放前的缩放系数，起始是 1.01
    // maxWidth：用来保存曾经最长的时间线
    // flag：区分是点击缩放按钮还是鼠标滚轮，默认是鼠标滚轮
    property real wheelx: 0.0
    property real scaleValue: 1.01
    property real maxWidth: 1.0
    property bool flag: true
    // scaleCopy：用来过渡 multitrack.scaleFactor随 scaleSlider.value变化
    property real scaleCopy: scaleSliderValue
    // Add -End

    property alias actionAddFilter: addFilterAction
    property alias actionShowAllClips: showAllClipsAction

    id: root
    SystemPalette { id: activePalette }
    width: 200
    height: 51
    anchors.margins:0

    style: ToolBarStyle {
                panel:
                    Rectangle {
                    anchors.fill: parent
                    RowLayout{
                        spacing: 0
                        anchors.fill: parent
                        Rectangle {
                            width: 12;  height: root.height; color: normalColor;
                            z:1;
                            Image { source: 'qrc:///timeline/timeline-toolbar-bg-left.png';  fillMode: Image.Pad; anchors.fill: parent}
                        }
                        Rectangle {height: root.height;
                            Layout.fillWidth: true;
                            Image { anchors.fill: parent; source: 'qrc:///timeline/timeline-toolbar-bg-center.png'; fillMode: Image.TileHorizontally;}
                        }
                        Rectangle {
                            width: 12;  height: root.height; color: normalColor;
                            z:1;
                            Image { source: 'qrc:///timeline/timeline-toolbar-bg-right.png'; anchors.fill: parent}
                        }
                    }
                    }
    }


        RowLayout{
            x:0
            anchors.verticalCenter: parent.verticalCenter
            CustomToolbutton {
                customText: qsTr('Menu')
                action: menuAction
                implicitWidth: 44
                implicitHeight: 44
                customIconSource: 'qrc:///timeline/timeline-toolbar-menu-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-menu-p.png'
                disabledIconSource: 'qrc:///timeline/timeline-toolbar-menu-d.png'
            }

            Rectangle {
                implicitWidth: 44
                implicitHeight: 44
                color: 'transparent'
                Image {
                    anchors.centerIn: parent
                    source: 'qrc:///timeline/timeline-toolbar-separator.png'
                }
            }

            // 增加（添加 clip到轨道上）
            CustomToolbutton {
                customText: qsTr('Append')
                action: appendAction
                implicitWidth: 44
                implicitHeight: 44

                customIconSource: 'qrc:///timeline/timeline-toolbar-append-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-append-p.png'
                disabledIconSource: 'qrc:///timeline/timeline-toolbar-append-d.png'
            }
            // 插入 clip到轨道上
            CustomToolbutton {
                customText:qsTr('Insert')
                action: insertAction
                implicitWidth: 44
                implicitHeight: 44

                customIconSource: 'qrc:///timeline/timeline-toolbar-insert-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-insert-p.png'
                disabledIconSource: 'qrc:///timeline/timeline-toolbar-insert-d.png'
            }

            // 删除当前的 clip
            CustomToolbutton {
                customText: qsTr('Delete')
                action: deleteAction
                implicitWidth: 44
                implicitHeight: 44

                customIconSource: 'qrc:///timeline/timeline-toolbar-remove-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-remove-p.png'
                disabledIconSource: 'qrc:///timeline/timeline-toolbar-remove-d.png'
            }


            // 在播放游标处切分 clip
            CustomToolbutton {
                customText:qsTr('Split')
                action: splitAction
                implicitWidth: 44
                implicitHeight: 44

                customIconSource: 'qrc:///timeline/timeline-toolbar-split-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-split-p.png'
                disabledIconSource: 'qrc:///timeline/timeline-toolbar-split-d.png'
            }

            Rectangle {
                implicitWidth: 44
                implicitHeight: 44
                color: 'transparent'
                Image {
                    anchors.centerIn: parent
                    source: 'qrc:///timeline/timeline-toolbar-separator.png'
                }
            }

            // ？？？
            ToolButton {
                id: snapButton
                visible: false
                implicitWidth: 28
                implicitHeight: 35
                checkable: true
                checked: true
                iconName: 'snap'
                iconSource: 'qrc:///icons/light/32x32/actions/snap.png'
                tooltip: qsTr('Toggle snapping')
            }
            ToolButton {
                id: scrubButton
                visible: false //add to hide the button
                implicitWidth: 28
                implicitHeight: 35
                checkable: true
                iconName: 'scrub_drag'
                iconSource: 'qrc:///icons/light/32x32/actions/scrub_drag.png'
                tooltip: qsTr('Scrub while dragging')
            }
            ToolButton {
                id: rippleButton
                visible: false//add to hide the button
                implicitWidth: 28
                implicitHeight: 35
                checkable: true
                checked: false
                iconName: 'target'
                iconSource: 'qrc:///icons/light/32x32/actions/target.png'
                tooltip: qsTr('Ripple trim and drop')
                text: qsTr('Ripple')
            }
            // 添加改变大小滤镜
            CustomToolbutton {
                id: posAndSizeButton
                action: positionAndSizeAction
                visible: true//add to hide the button
                bEnabled: hasClipOrTrackSelected

                implicitWidth: 44
                implicitHeight: 44

                tooltip: qsTr('Change the position and size of the clip')
                customText: qsTr('Resize')

                customIconSource: 'qrc:///timeline/timeline-toolbar-size-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-size-p.png'
                disabledIconSource: 'qrc:///timeline/timeline-toolbar-size-d.png'
            }

            // 添加旋转滤镜
            CustomToolbutton {
                id: rotateButton
                action: rotateAction
                visible: true//add to hide the button
                bEnabled: hasClipOrTrackSelected


                implicitWidth: 44
                implicitHeight: 44

                tooltip: qsTr('Rotate clip')
                customText: qsTr('Rotate')

                customIconSource: 'qrc:///timeline/timeline-toolbar-rotate-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-rotate-p.png'
                disabledIconSource:'qrc:///timeline/timeline-toolbar-rotate-d.png'
            }

            // 添加裁剪滤镜
            CustomToolbutton {
                id: cropButton
                action: cropAction
                visible: true//add to hide the button
                bEnabled: hasClipOrTrackSelected


                implicitWidth: 44
                implicitHeight: 44

                tooltip: qsTr('Crop clip')
                customText: qsTr('Crop')

                customIconSource: 'qrc:///timeline/timeline-toolbar-crop-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-crop-p.png'
                disabledIconSource: 'qrc:///timeline/timeline-toolbar-crop-d.png'
            }

            // 时间线工具栏去掉淡入淡出按钮
            /*
            CustomToolbutton {
                id: fadeInButton
                action: fadeInAction
                visible: true//add to hide the button
                bEnabled: hasClipOrTrackSelected


                implicitWidth: 44
                implicitHeight: 44

                tooltip: qsTr('Fade in')
                customText: qsTr('Fade in')

                customIconSource: enabled?'qrc:///timeline/timeline-toolbar-fade-in-n.png':'qrc:///timeline/timeline-toolbar-fade-in-p.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-fade-in-p.png'
            }

            CustomToolbutton {
                id: fadeOutButton
                action: fadeOutAction
                visible: true//add to hide the button
                bEnabled: hasClipOrTrackSelected


                implicitWidth: 44
                implicitHeight: 44

                tooltip: qsTr('Fade Out')
                customText: qsTr('Fade Out')

                customIconSource: enabled?'qrc:///timeline/timeline-toolbar-fade-out-n.png':'qrc:///timeline/timeline-toolbar-fade-out-p.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-fade-out-p.png'
            }
            */
            // 添加音量滤镜
            CustomToolbutton {
                id: volumeButton
                action: volumeAction
                visible: true//add to hide the button
                bEnabled: hasClipOrTrackSelected

                implicitWidth: 44
                implicitHeight: 44

                tooltip: qsTr('Volume')
                customText: qsTr('Volume')

                customIconSource: 'qrc:///timeline/timeline-toolbar-volume-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-volume-p.png'
                disabledIconSource:'qrc:///timeline/timeline-toolbar-volume-d.png'
            }

            // 添加文字滤镜
            CustomToolbutton {
                id: textButton
                action: addTextAction
                visible: true
                bEnabled: hasClipOrTrackSelected

                implicitWidth: 44
                implicitHeight: 44

                customText:qsTr('Add Text')

                customIconSource: 'qrc:///timeline/timeline-toolbar-text-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-text-p.png'
                disabledIconSource: 'qrc:///timeline/timeline-toolbar-text-d.png'
            }

            // Add -添加滤镜按钮
            CustomToolbutton {
                id: filterButton
                action: addFilterAction
                visible: true
                bEnabled: hasClipOrTrackSelected

                implicitWidth: 44
                implicitHeight: 44

                customText:qsTr('Add Filter')

                customIconSource: 'qrc:///timeline/timeline-toolbar-filter-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-filter-p.png'
                disabledIconSource: 'qrc:///timeline/timeline-toolbar-filter-d.png'
            }
            // Add -End

            Rectangle {
                implicitWidth: 44
                implicitHeight: 44
                color: 'transparent'
                Image {
                    anchors.centerIn: parent
                    source: 'qrc:///timeline/timeline-toolbar-separator.png'
                }
            }

            // Add -转场设置按钮（原转场属性）
            CustomToolbutton {
                id: transitionButton
                action: setTransitionAction
                visible: true
                bEnabled: hasClipOrTrackSelected && tracksRepeater.itemAt(currentTrack).clipAt(timeline.selection[0]).isTransition

                implicitWidth: 44
                implicitHeight: 44

                customText:qsTr('Transition Settings')

                customIconSource: 'qrc:///timeline/timeline-toolbar-transition-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-transition-p.png'
                disabledIconSource: 'qrc:///timeline/timeline-toolbar-transition-d.png'
            }
            // Add -End

            Rectangle {
                implicitWidth: 44
                implicitHeight: 44
                color: 'transparent'
                Image {
                    anchors.centerIn: parent
                    source: 'qrc:///timeline/timeline-toolbar-separator.png'
                }
            }
            // Add -显示所有 Clips
            CustomToolbutton {
                id: allClipsButton
                action: showAllClipsAction
                visible: true
                //bEnabled: hasClipOrTrackSelected

                implicitWidth: 44
                implicitHeight: 44

                customText:qsTr('Zoom to Fit')

                customIconSource: 'qrc:///timeline/timeline-toolbar-fit-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-fit-p.png'
                disabledIconSource: 'qrc:///timeline/timeline-toolbar-fit-n.png'
            }
            // Add -End

            ColorOverlay {
                id: snapColorEffect
                visible: false//snapButton.checked
                anchors.fill: snapButton
                source: snapButton
                color: checkedColor
                cached: true
            }
            ColorOverlay {
                id: scrubColorEffect
                visible: scrubButton.checked
                anchors.fill: scrubButton
                source: scrubButton
                color: checkedColor
                cached: true
            }
            ColorOverlay {
                id: rippleColorEffect
                visible: rippleButton.checked
                anchors.fill: rippleButton
                source: rippleButton
                color: checkedColor
                cached: true
            }
        }

        // ColorOverlay {
        //     id: snapColorEffect
        //     visible: false//snapButton.checked
        //     anchors.fill: snapButton
        //     source: snapButton
        //     color: checkedColor
        //     cached: true
        // }
        // ColorOverlay {
        //     id: scrubColorEffect
        //     visible: scrubButton.checked
        //     anchors.fill: scrubButton
        //     source: scrubButton
        //     color: checkedColor
        //     cached: true
        // }
        // ColorOverlay {
        //     id: rippleColorEffect
        //     visible: rippleButton.checked
        //     anchors.fill: rippleButton
        //     source: rippleButton
        //     color: checkedColor
        //     cached: true
        // }




    RowLayout {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 1

        // 缩小时间线按钮
        Button {
            id: zoomOutButton
            style: ButtonStyle {
                background: Rectangle {
                    color: 'transparent'
                }
            }
            visible: true
            implicitWidth: 27
            implicitHeight: 27
            iconSource: pressed? 'qrc:///timeline/timeline-toolbar-zoomout-pressed.png' : 'qrc:///timeline/timeline-toolbar-zoomout.png'

            action: zoomOutAction
        }

        // 放大时间线按钮
        Button {
            id: zoomInButton
            style: ButtonStyle {
                background: Rectangle {
                    color: 'transparent'
                }
            }
            visible: true
            implicitWidth: 27
            implicitHeight: 27
            iconSource: pressed? 'qrc:///timeline/timeline-toolbar-zoomin-pressed.png' : 'qrc:///timeline/timeline-toolbar-zoomin.png'
            action: zoomInAction
        }
    }

    // 缩放时间线的滑动条
    // 被隐藏了
    ZoomSlider {
        id: scaleSlider
        visible: false
        width: 140
        height: 35
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        z: 2
        onValueChanged: flag ? Logic.scrollIfZoomNeeded(wheelx, scaleValue) : Logic.scrollIfNeeded()
    }

    // Add -滤镜菜单
    FilterMenu {
        id: filterMenu
        height: 400
        onFilterSelected: {
            var model = metadatamodel.get(index)
            if(model.isAudio)
                mainwindow.onShowPropertiesAudioFilterDock()
            else
                mainwindow.onShowPropertiesVideoFilterDock()
            attachedfiltersmodel.add(model)
        }
    }
    // Add -end

    // 各个按钮功能
    Action {
        id: menuAction
        tooltip: qsTr('Display a menu of additional actions')
        //iconName: 'format-justify-fill'
        //iconSource: 'qrc:///timeline/timeline-toolbar-menu-n.png'
        onTriggered: menu.popup()
    }

    Action {
        id: cutAction
        tooltip: qsTr('Cut - Copy the current clip to the Source\nplayer and ripple delete it')
        //iconName: 'edit-cut'
        //iconSource: 'qrc:///icons/oxygen/32x32/actions/edit-cut.png'
        enabled: timeline ? timeline.selection.length : false
        onTriggered: {
            console.assert(timeline);
            if(timeline)
                timeline.removeSelection(true)
        }
    }

    Action {
        id: copyAction
        tooltip: qsTr('Copy - Copy the current clip to the Source player (C)')
        //iconName: 'edit-copy'
        //iconSource: 'qrc:///icons/oxygen/32x32/actions/edit-copy.png'
        enabled: timeline ? timeline.selection.length : false
        onTriggered: {
            console.assert(timeline);
            if(timeline)
                timeline.copyClip(currentTrack, timeline.selection[0])
        }
    }

    Action {
        id: pasteAction
        tooltip: qsTr('Paste - Insert clip into the current track\nshifting following clips to the right (V)')
        //iconName: 'edit-paste'
        //iconSource: 'qrc:///icons/oxygen/32x32/actions/edit-paste.png'
        onTriggered: {
            console.assert(timeline);
            if(timeline)
//                timeline.insert(currentTrack)
                timeline.paste()        // 粘贴
        }
    }

    Action {
        id: appendAction
        tooltip: qsTr('Append to the current track (A)')
        //iconName: 'list-add'
        //iconSource: 'qrc:///timeline/timeline-toolbar-append-n.png'
        onTriggered:
        {
            console.assert(timeline);
            if(timeline){
                timeline.append(currentTrack)
            }
            hasClipOrTrackSelected = true;
        }
    }


    Action {
        id: deleteAction
        tooltip: qsTr('Remove current clip')
        //iconName: 'list-remove'
        //iconSource: 'qrc:///timeline/timeline-toolbar-remove-n.png'
        onTriggered: {
            console.assert(timeline);
            if(timeline)
                timeline.lift(currentTrack, timeline.selection[0])//timeline.remove(currentTrack, selection[0])
        }
    }

//    Action {
//        id: liftAction
//        tooltip: qsTr('Lift - Remove current clip without\naffecting position of other clips (Z)')
//        iconName: 'lift'
//        iconSource: 'qrc:///icons/light/32x32/actions/lift.png'
//        onTriggered: timeline.lift(currentTrack, selection[0])
//    }

    Action {
        id: insertAction
        tooltip: qsTr('Insert clip into the current track\nshifting following clips to the right (V)')
        //iconName: 'insert'
        //iconSource: 'qrc:///timeline/timeline-toolbar-insert-n.png'
        onTriggered: {
            console.assert(timeline);
            if(timeline)
                timeline.insert(currentTrack)
        }
    }

//    Action {
//        id: overwriteAction
//        tooltip: qsTr('Overwrite clip onto the current track (B)')
//        iconName: 'overwrite'
//        iconSource: 'qrc:///icons/light/32x32/actions/overwrite.png'
//        onTriggered: timeline.overwrite(currentTrack)
//    }

    Action {
        id: splitAction
        tooltip: qsTr('Split At Playhead (S)')
        //iconName: 'split'
        //iconSource: 'qrc:///timeline/timeline-toolbar-split-n.png'
        onTriggered: {
            console.assert(timeline);
            if(timeline)
                timeline.splitClip(currentTrack)
        }
    }

    Action {
        id: positionAndSizeAction
        tooltip: qsTr('Change clip\'s position and size')
        //iconName: 'posAndSize'
        enabled:hasClipOrTrackSelected
        //iconSource: 'qrc:///timeline/timeline-toolbar-size-n.png'
        onTriggered: {
            console.assert(timeline);
            if(timeline){
                timeline.addPositionAndSizeFilter()
                mainwindow.onShowPropertiesVideoFilterDock()
            }
        }
    }

    Action {
        id: rotateAction
    //    tooltip: qsTr('Change clip\'s position and size')
        //iconName: 'posAndSize'
        //iconSource: 'qrc:///timeline/timeline-toolbar-rotate-n.png'
        enabled: hasClipOrTrackSelected
        onTriggered: {
            console.assert(timeline);
            if(timeline){
                timeline.addRotateFilter()
                mainwindow.onShowPropertiesVideoFilterDock()
            }
        }
    }

    Action {
        id: cropAction
        enabled: hasClipOrTrackSelected
        onTriggered: {
            console.assert(timeline);
            if(timeline){
                timeline.addCropFilter()
                mainwindow.onShowPropertiesVideoFilterDock()
            }
        }
    }

    /*
    Action {
        id: fadeInAction
       // tooltip: qsTr('Change clip\'s position and size')
        //iconName: 'posAndSize'
        //iconSource: 'qrc:///timeline/timeline-toolbar-fade-in-n.png'
        enabled: hasClipOrTrackSelected
        onTriggered: {
            timeline.addFadeInVideoFilter()
        //    timeline.addFadeInAudioFilter()

        }
    }

    Action {
        id: fadeOutAction
      //  tooltip: qsTr('Change clip\'s position and size')
        //iconName: 'posAndSize'
        //iconSource: 'qrc:///timeline/timeline-toolbar-fade-out-n.png'
        enabled: hasClipOrTrackSelected
        onTriggered: {
            timeline.addFadeOutVideoFilter()
//          timeline.addFadeOutAudioFilter()

        }
    }
    */

    Action {
        id: volumeAction
        enabled: hasClipOrTrackSelected
        onTriggered: {
            console.assert(timeline);
            if(timeline){
                timeline.addVolumeFilter()
                mainwindow.onShowPropertiesAudioFilterDock()
            }
        }
    }

    Action {
        id: zoomOutAction
        tooltip: qsTr('Zoom out timeline')
        onTriggered: {
            flag = false
            // scaleSlider.value -= 0.1
            if(scaleCopy != 0) {
                scaleSlider.value = scaleCopy - 0.1
                scaleCopy = scaleSlider.value 
            }
        }
    }

    Action {
        id: zoomInAction
        tooltip: qsTr('Zoom in timeline')
        //iconName: 'posAndSize'
        //iconSource: 'qrc:///timeline/timeline-toolbar-zoomin.png'
        onTriggered: {
            flag = false
            // scaleSlider.value += 0.1
            scaleSlider.value = scaleCopy + 0.1
            scaleCopy = scaleSlider.value 
        }
    }

    Action {
        id: addTextAction
        tooltip:qsTr('Add text to video')
        enabled: hasClipOrTrackSelected
        onTriggered: {
            console.assert(timeline);
            if(timeline){
                timeline.addTextFilter()
                mainwindow.onShowPropertiesVideoFilterDock()
            }
        }
    }

    // Add -添加滤镜按钮
    Action {
        id: addFilterAction
        tooltip: qsTr("Add filter to video")
        enabled: hasClipOrTrackSelected
        onTriggered: {
            console.assert(timeline);
            if(timeline){
                filterMenu.popup(filterButton, timeline.dockPosition);
            }
        }
    }
    // Add -End

    // Add -转场设置按钮
    Action {
        id: setTransitionAction
        tooltip: qsTr("Set the transition property")
        enabled: hasClipOrTrackSelected
        onTriggered: {
            console.assert(timeline);
            // 只有点击转场才会弹出转场设置（属性）框，普通clip不弹属性框
            if(timeline && tracksRepeater.itemAt(currentTrack).clipAt(timeline.selection[0]).isTransition){
                timeline.onShowProperties(currentTrack, timeline.selection[0])
            }
        }
    }
    // Add -End

    // Add -显示所有 Clips
    // 长度超过 scrollView就直接铺满整个 scrollView
    // 名称是否要保持一致：zoomToFitAction
    Action {
        id: showAllClipsAction
        tooltip: qsTr("Show all clips")
        enabled: hasClipOrTrackSelected
        onTriggered: {
            var scaleRefer = scrollView.width / (tracksContainer.width / multitrack.scaleFactor * 1.01);
            if(multitrack.scaleFactor >= scaleRefer) {
                // scaleRefer<0.01说明比最小的缩放还要小
                scaleCopy = scaleRefer<0.01 ? 0 : Math.round(Math.pow(scaleRefer-0.01, 1/3) / 0.0625) * 0.0625;
                multitrack.scaleFactor = scaleRefer;
            }
            scrollView.flickableItem.contentX = 0;
        }
    }
    // Add -End
}
