/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
 * Author: fuyunhuaxin <2446010587@qq.com>
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
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import MovieMator.Controls 1.0
import QtQuick.Dialogs 1.1

Rectangle {
    id:keyFrame

    color: 'transparent'
    width: parent.width
    height: 90

    signal enableKeyFrameChanged(bool bEnable)
    signal autoAddKeyFrameChanged(bool bEnable)

    signal addFrameChanged()
    signal frameChanged(double keyFrameNum)
    signal removeKeyFrame()
    signal removeAllKeyFrame()

    signal switchFoldStat(bool bChecked)
    

    function refreshFrameButtonsEnable()
    {
        var position = timeline.getPositionInCurrentClip()

        addKeyFrameButton.enabled   = enableKeyFrameCheckBox.checked && !autoAddKeyFrameCheckBox.checked && metadata && (metadata.keyframes.parameterCount > 0) 

        preKeyFrameButton.enabled   = enableKeyFrameCheckBox.checked && metadata && (metadata.keyframes.parameterCount > 0) && filter.bHasPreKeyFrame(position)

        nextKeyFrameButton.enabled  = enableKeyFrameCheckBox.checked && metadata && (metadata.keyframes.parameterCount > 0) && filter.bHasNextKeyFrame(position)

        removeKeyFrameButton.enabled= enableKeyFrameCheckBox.checked && metadata && (metadata.keyframes.parameterCount > 0) && filter.cache_bKeyFrame(position) && (position !== 0) && (position !== (timeline.getCurrentClipLength() - 1))

        autoAddKeyFrameCheckBox.enabled = enableKeyFrameCheckBox.checked
    }

    Connections {
        target: filterDock
        onPositionChanged: {
             refreshFrameButtonsEnable()

             var position = timeline.getPositionInCurrentClip()
             frameChanged(position)
        }
    }

    Rectangle{
        id:header
        color: '#555555'
        width: parent.width+10
        height: 30
        
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin:-10
        
        Button{
            id: foldBtn
            checkable:true
            checked:false
            width:15
            height:15
            anchors.left: parent.left
            anchors.leftMargin:11
            anchors.verticalCenter: parent.verticalCenter
            
            style: ButtonStyle {
                background: Rectangle {
                    color: 'transparent'
                }  
            }
            Image {
                id:keyIcon
                fillMode: Image.PreserveAspectCrop
                anchors.fill: parent
                source: foldBtn.checked?"qrc:///icons/light/32x32/arrow_2.png":"qrc:///icons/light/32x32/arrow_1.png"
            }
            onClicked: {
                switchFoldStat(checked)
            }
        }
        Text {
            id: catName
            text: qsTr("KeyFrame")
            color: '#ffffff'
            font.pixelSize: 14
            z:2
            anchors{
                left: foldBtn.right
                leftMargin:4
                verticalCenter: parent.verticalCenter
            }
        }
        
    }

    Rectangle{
        id:body
        color: 'transparent'
        width: parent.width
        height: 70
        anchors.top: header.bottom
        anchors.topMargin:0
        visible:foldBtn.checked?true:false
        GridLayout {
            columns: 4
            CheckBox {
                id: enableKeyFrameCheckBox
                Layout.columnSpan: 4
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.top: parent.top
                anchors.topMargin: 4
                text: qsTr('Enable Key Frames')
                checked: (filter && filter.cache_getKeyFrameNumber() > 0)
                style: CheckBoxStyle {
                    indicator: Rectangle {
                        color:'transparent'
                        radius: 3
                        z:1
                        implicitWidth: 18
                        implicitHeight: 18
                        Image {
                            z:2
                            anchors.fill: parent
                            source:enableKeyFrameCheckBox.checked ? 'qrc:///icons/light/32x32/key-a.png' : 'qrc:///icons/light/32x32/key.png'
                        }
                    }
                }
                onClicked: {
                    if(checked)
                    {   
                        if(metadata.keyframes.parameterCount > 0)
                        {   
                            addFrameChanged()
                            refreshFrameButtonsEnable()
                            autoAddKeyFrameCheckBox.checked = true
                        }  
                    }
                    else
                    {  
                        if(filter.cache_getKeyFrameNumber() > 0)
                            removeKeyFrameWarning.visible = true
                    }
                }
                onCheckedChanged: {
                    refreshFrameButtonsEnable()
                    enableKeyFrameChanged(checked)
                }
            }
            CheckBox {
                id: autoAddKeyFrameCheckBox
                Layout.columnSpan: 4
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.top: enableKeyFrameCheckBox.bottom
                anchors.topMargin: 4
                text: qsTr('Auto Add Key Frames')
                checked: true
                opacity: enableKeyFrameCheckBox.checked ? 1.0 : 0.5
                onClicked: {
                }
                height:25
                style: CheckBoxStyle {
                    indicator: Rectangle {
                        color:'transparent'
                        radius: 3
                        z:1
                        implicitWidth: 18
                        implicitHeight: 18
                        Image {
                            z:2
                            anchors.fill: parent
                            source:autoAddKeyFrameCheckBox.checked ? 'qrc:///icons/light/32x32/key-a.png' : 'qrc:///icons/light/32x32/key.png'
                        }
                    }
                }
                onCheckedChanged: 
                {
                    refreshFrameButtonsEnable() 
                    autoAddKeyFrameChanged(checked)
                }    
            }

            Button {
                id:addKeyFrameButton
                anchors {
                    left: parent.left
                    leftMargin: 24
                    top: autoAddKeyFrameCheckBox.bottom
                    topMargin: 8
                }
                implicitWidth: 18
                implicitHeight: 18

                enabled: refreshFrameButtonsEnable() 
                opacity: enabled ? 1.0 : 0.5
                tooltip: qsTr('Add key frame')
                //customIconSource: 'qrc:///icons/light/32x32/list-add.png'
                //customText: qsTr('Add')
                //buttonWidth : 85
                onClicked: {
                    addFrameChanged()
                    refreshFrameButtonsEnable() 
                }

                style: ButtonStyle {
                    background: Rectangle {
                        color: 'transparent'
                    }  
                }
                Image {
                    fillMode: Image.PreserveAspectCrop
                    anchors.fill: parent
                    source: addKeyFrameButton.pressed ? "qrc:///icons/light/32x32/add_keyframe-on.png" : (enabled?'qrc:///icons/light/32x32/add_keyframe.png':'qrc:///icons/light/32x32/add_keyframe_disable.png')
                }
            }

            Button {
                id:removeKeyFrameButton
                anchors {
                    left: addKeyFrameButton.right
                    leftMargin: 24
                    top: addKeyFrameButton.top
                    topMargin: 0
                }
                implicitWidth: 18
                implicitHeight: 18

                opacity: enabled ? 1.0 : 0.5
                tooltip: qsTr('Remove key frame')
                //customIconSource: 'qrc:///icons/light/32x32/list-remove.png'
                //customText: qsTr('Remove')
                //buttonWidth : 85

                style: ButtonStyle {
                    background: Rectangle {
                        color: 'transparent'
                    }  
                }
                Image {
                    fillMode: Image.PreserveAspectCrop
                    anchors.fill: parent
                    source: removeKeyFrameButton.pressed ? "qrc:///icons/light/32x32/remove_keyframe-on.png" : (enabled?'qrc:///icons/light/32x32/remove_keyframe.png':'qrc:///icons/light/32x32/remove_keyframe_disable.png')
                }

                onClicked: {
                    var position        = timeline.getPositionInCurrentClip()

                    if((position === 0) || (position === (timeline.getCurrentClipLength() - 1)))
                        return   //首尾帧无法删除

                    var bKeyFrame       = filter.cache_bKeyFrame(position)
                    if (bKeyFrame)
                        filter.removeKeyFrameParaValue(position)
                        removeKeyFrame()

                    refreshFrameButtonsEnable() 
                }
            }

            Button {
                id:preKeyFrameButton
                anchors {
                    top: addKeyFrameButton.top
                    topMargin: 0
                    left: removeKeyFrameButton.right
                    leftMargin: 24
                }
                implicitWidth: 18
                implicitHeight: 18

                opacity: enabled ? 1.0 : 0.5
                tooltip: qsTr('Prev key frame')
                //customIconSource: enabled?'qrc:///icons/light/32x32/previous_keyframe.png' :'qrc:///icons/light/32x32/previous_keyframe_disable.png'
                //customText: qsTr('<<')
                //buttonWidth : 85
                onClicked: {
                    var nFrame = filter.cache_getPreKeyFrameNum(timeline.getPositionInCurrentClip())
                    if(nFrame !== -1)
                    {
                        filterDock.position = nFrame
                    }
                }

                style: ButtonStyle {
                    background: Rectangle {
                        color: 'transparent'
                    }  
                }
                Image {
                    fillMode: Image.PreserveAspectCrop
                    anchors.fill: parent
                    source: preKeyFrameButton.pressed? "qrc:///icons/light/32x32/previous_keyframe-on.png" : (enabled?'qrc:///icons/light/32x32/previous_keyframe.png':'qrc:///icons/light/32x32/previous_keyframe_disable.png')
                }
            }

            Button {
                id:nextKeyFrameButton
                anchors {
                    top: addKeyFrameButton.top
                    topMargin: 0
                    left: preKeyFrameButton.right
                    leftMargin: 24
                }
                implicitWidth: 18
                implicitHeight: 18

                opacity: enabled ? 1.0 : 0.5
                tooltip: qsTr('Next key frame')
                //customIconSource: 'qrc:///icons/light/32x32/bg.png'
                //customIconSource: enabled?'qrc:///icons/light/32x32/next_keyframe.png':'qrc:///icons/light/32x32/next_keyframe_disable.png'
                //customText: qsTr('>>')
                //buttonWidth : 85
                onClicked: {
                    var nFrame = filter.cache_getNextKeyFrameNum(timeline.getPositionInCurrentClip())
                    if(nFrame !== -1)
                    {
                        filterDock.position = nFrame
                        //frameChanged(nFrame)
                    }
                }
                style: ButtonStyle {
                    background: Rectangle {
                        color: 'transparent'
                    }  
                }
                Image {
                    fillMode: Image.PreserveAspectCrop
                    anchors.fill: parent
                    source: nextKeyFrameButton.pressed? "qrc:///icons/light/32x32/next_keyframe-on.png" : (enabled?'qrc:///icons/light/32x32/next_keyframe.png':'qrc:///icons/light/32x32/next_keyframe_disable.png')
                }
            }
        }
    }
    MessageDialog {
        id: removeKeyFrameWarning
        visible: false
        title: qsTr("Confirm Removing Keyframes")
        text: qsTr('This will remove all keyframes.<p>Do you still want to do this?')
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            enableKeyFrameCheckBox.checked = false

            removeAllKeyFrame()
        }
        onNo: 
            enableKeyFrameCheckBox.checked = true  
        
    }
    Connections {
        target: filterDock
        onCurrentFilterChanged: {
            enableKeyFrameCheckBox.checked = ( filter && filter.cache_getKeyFrameNumber() > 0)
            autoAddKeyFrameChanged(autoAddKeyFrameCheckBox.checked)
        }
    }

    Connections {
        target: filter
        onFilterPropertyValueChanged: {
            enableKeyFrameCheckBox.checked = (filter && filter.cache_getKeyFrameNumber() > 0)
            autoAddKeyFrameChanged(autoAddKeyFrameCheckBox.checked)
        }
    }
}
