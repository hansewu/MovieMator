
import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import 'Timeline.js' as Logic

ToolBar {
    property alias ripple: rippleButton.checked
    property alias scrub: scrubButton.checked
    property alias snap: snapButton.checked
    property color checkedColor: Qt.rgba(activePalette.highlight.r, activePalette.highlight.g, activePalette.highlight.b, 0.3)
    property alias scaleSliderValue: scaleSlider.value

    property bool hasClipOrTrackSelected:false



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

            CustomToolbutton {
                customText: qsTr('Append')
                action: appendAction
                implicitWidth: 44
                implicitHeight: 44

                customIconSource: 'qrc:///timeline/timeline-toolbar-append-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-append-p.png'
            }

            CustomToolbutton {
                customText:qsTr('Insert')
                action: insertAction
                implicitWidth: 44
                implicitHeight: 44

                customIconSource: 'qrc:///timeline/timeline-toolbar-insert-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-insert-p.png'
            }

            CustomToolbutton {
                customText: qsTr('Delete')
                action: deleteAction
                implicitWidth: 44
                implicitHeight: 44

                customIconSource: 'qrc:///timeline/timeline-toolbar-remove-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-remove-p.png'
            }


            CustomToolbutton {
                customText:qsTr('Split')
                action: splitAction
                implicitWidth: 44
                implicitHeight: 44

                customIconSource: 'qrc:///timeline/timeline-toolbar-split-n.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-split-p.png'
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
            CustomToolbutton {
                id: posAndSizeButton
                action: positionAndSizeAction
                visible: true//add to hide the button
                bEnabled: hasClipOrTrackSelected

                implicitWidth: 44
                implicitHeight: 44

                tooltip: qsTr('Change the position and size of the clip')
                customText: qsTr('Resize')

                customIconSource: enabled?'qrc:///timeline/timeline-toolbar-size-n.png':'qrc:///timeline/timeline-toolbar-size-p.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-size-p.png'
            }

            CustomToolbutton {
                id: rotateButton
                action: rotateAction
                visible: true//add to hide the button
                bEnabled: hasClipOrTrackSelected


                implicitWidth: 44
                implicitHeight: 44

                tooltip: qsTr('Rotate clip')
                customText: qsTr('Rotate')

                customIconSource: enabled?'qrc:///timeline/timeline-toolbar-rotate-n.png':'qrc:///timeline/timeline-toolbar-rotate-p.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-rotate-p.png'
            }

            CustomToolbutton {
                id: cropButton
                action: cropAction
                visible: true//add to hide the button
                bEnabled: hasClipOrTrackSelected


                implicitWidth: 44
                implicitHeight: 44

                tooltip: qsTr('Crop clip')
                customText: qsTr('Crop')

                customIconSource: enabled?'qrc:///timeline/timeline-toolbar-crop-n.png':'qrc:///timeline/timeline-toolbar-crop-p.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-crop-p.png'
            }

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

            CustomToolbutton {
                id: volumeButton
                action: volumeAction
                visible: true//add to hide the button
                bEnabled: hasClipOrTrackSelected

                implicitWidth: 44
                implicitHeight: 44

                tooltip: qsTr('Volume')
                customText: qsTr('Volume')

                customIconSource: enabled?'qrc:///timeline/timeline-toolbar-volume-n.png':'qrc:///timeline/timeline-toolbar-volume-p.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-volume-p.png'
            }

            CustomToolbutton {
                id: textButton
                action: addTextAction
                visible: true
                bEnabled: hasClipOrTrackSelected

                implicitWidth: 44
                implicitHeight: 44

                customText:qsTr('Add Text')

                customIconSource: enabled?'qrc:///timeline/timeline-toolbar-text-n.png':'qrc:///timeline/timeline-toolbar-text-p.png'
                pressedIconSource: 'qrc:///timeline/timeline-toolbar-text-p.png'
            }
        }

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




    RowLayout {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 1

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

    ZoomSlider {
        id: scaleSlider
        visible: false
        width: 140
        height: 35
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        z: 2
        onValueChanged: Logic.scrollIfNeeded()
    }

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
        enabled: timeline.selection.length
        onTriggered: timeline.removeSelection(true)
    }

    Action {
        id: copyAction
        tooltip: qsTr('Copy - Copy the current clip to the Source player (C)')
        //iconName: 'edit-copy'
        //iconSource: 'qrc:///icons/oxygen/32x32/actions/edit-copy.png'
        enabled: timeline.selection.length
        onTriggered: timeline.copyClip(currentTrack, timeline.selection[0])
    }

    Action {
        id: pasteAction
        tooltip: qsTr('Paste - Insert clip into the current track\nshifting following clips to the right (V)')
        //iconName: 'edit-paste'
        //iconSource: 'qrc:///icons/oxygen/32x32/actions/edit-paste.png'
        onTriggered: timeline.insert(currentTrack)
    }

    Action {
        id: appendAction
        tooltip: qsTr('Append to the current track (A)')
        //iconName: 'list-add'
        //iconSource: 'qrc:///timeline/timeline-toolbar-append-n.png'
        onTriggered:
        {
            timeline.append(currentTrack)
            hasClipOrTrackSelected = true;
        }
    }


    Action {
        id: deleteAction
        tooltip: qsTr('Remove current clip')
        //iconName: 'list-remove'
        //iconSource: 'qrc:///timeline/timeline-toolbar-remove-n.png'
        onTriggered: timeline.lift(currentTrack, timeline.selection[0])//timeline.remove(currentTrack, selection[0])
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
        onTriggered: timeline.insert(currentTrack)
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
        onTriggered: timeline.splitClip(currentTrack)
    }

    Action {
        id: positionAndSizeAction
        tooltip: qsTr('Change clip\'s position and size')
        //iconName: 'posAndSize'
        enabled:hasClipOrTrackSelected
        //iconSource: 'qrc:///timeline/timeline-toolbar-size-n.png'
        onTriggered: timeline.addPositionAndSizeFilter()
    }

    Action {
        id: rotateAction
    //    tooltip: qsTr('Change clip\'s position and size')
        //iconName: 'posAndSize'
        //iconSource: 'qrc:///timeline/timeline-toolbar-rotate-n.png'
        enabled: hasClipOrTrackSelected
        onTriggered: timeline.addRotateFilter()
    }

    Action {
        id: cropAction
   //     tooltip: qsTr('Change clip\'s position and size')
        //iconName: 'posAndSize'
        //iconSource: 'qrc:///timeline/timeline-toolbar-crop-n.png'
        enabled: hasClipOrTrackSelected
        onTriggered: timeline.addCropFilter()
    }

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

    Action {
        id: volumeAction
   //     tooltip: qsTr('Change clip\'s position and size')
        //iconName: 'posAndSize'
        //iconSource: 'qrc:///timeline/timeline-toolbar-volume-n.png'
        enabled: hasClipOrTrackSelected
        onTriggered: timeline.addVolumeFilter()
    }

    Action {
        id: zoomOutAction
        tooltip: qsTr('Zoom out timeline')
        //iconName: 'posAndSize'
//        iconSource: 'qrc:///timeline/timeline-toolbar-zoomout.png'
        onTriggered: scaleSlider.value -= 0.1
    }

    Action {
        id: zoomInAction
        tooltip: qsTr('Zoom in timeline')
        //iconName: 'posAndSize'
        //iconSource: 'qrc:///timeline/timeline-toolbar-zoomin.png'
        onTriggered: scaleSlider.value += 0.1
    }

    Action {
        id: addTextAction
        tooltip:qsTr('Add text to video')
        enabled: hasClipOrTrackSelected
        onTriggered:timeline.addTextFilter()
    }
}
