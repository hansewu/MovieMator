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
    height: 150

    signal enableKeyFrameChanged(bool bEnable)
//    signal autoAddKeyFrameChanged(bool bEnable)

    signal addFrameChanged()
    signal addKeyframe(string strIdentifierOfParameter, int nAnimationType, double dAnimationDuration)
    signal removeKeyFrame(string strIdentifierOfParameter)
    signal removeAllKeyFrame(string strIdentifierOfParameter)
    signal frameChanged(string strIdentifierOfParameter, double keyFrameNum)

    signal switchFoldStat(bool bChecked)
    
    property ListModel  m_listModelParameters: ListModel{}
    property string     m_strIdentifierOfParameter: ""

    function refreshFrameButtonsEnable(strIdentifierOfParameter)
    {
        if (strIdentifierOfParameter === "")
            return;

        var position = timeline.getPositionInCurrentClip()

        enableKeyFrameCheckBox.checked  = filter.isKeyframeActivate(strIdentifierOfParameter)
        addKeyFrameButton.enabled       = enableKeyFrameCheckBox.checked //&& !autoAddKeyFrameCheckBox.checked
        preKeyFrameButton.enabled       = enableKeyFrameCheckBox.checked && filter.hasPreKeyframeAtPositon(strIdentifierOfParameter, position)
        nextKeyFrameButton.enabled      = enableKeyFrameCheckBox.checked && filter.hasNextKeyframeAtPositon(strIdentifierOfParameter, position)
        removeKeyFrameButton.enabled    = enableKeyFrameCheckBox.checked && filter.isKeyframeAtPosition(strIdentifierOfParameter, position)
//        autoAddKeyFrameCheckBox.enabled = enableKeyFrameCheckBox.checked

//        addKeyFrameButton.visible       = filter.isKeyframeActivate(strIdentifierOfParameter)
//        preKeyFrameButton.visible       = filter.isKeyframeActivate(strIdentifierOfParameter)
//        nextKeyFrameButton.visible      = filter.isKeyframeActivate(strIdentifierOfParameter)
//        removeKeyFrameButton.visible    = filter.isKeyframeActivate(strIdentifierOfParameter)

        labelKeyframe.visible           = filter.isKeyframeActivate(strIdentifierOfParameter)
        labelKeyframeInfo.visible       = filter.isKeyframeActivate(strIdentifierOfParameter)
        labelInterpolation.visible      = filter.isKeyframeActivate(strIdentifierOfParameter)
        comboboxInterpolation.visible   = filter.isKeyframeActivate(strIdentifierOfParameter)


        labelAnimationType.visible      = !filter.isKeyframeActivate(strIdentifierOfParameter)
        comboboxAnimationType.visible   = !filter.isKeyframeActivate(strIdentifierOfParameter)
        labelAnimationDuration.visible  = !filter.isKeyframeActivate(strIdentifierOfParameter)
        sliderAnimationDuration.visible = !filter.isKeyframeActivate(strIdentifierOfParameter)

        rowLayoutButtons.visible        = filter.isKeyframeActivate(strIdentifierOfParameter)
    }


    function initUIWithParameter(strIdentifierOfParameter)
    {
        console.log("initUIWithParameter: ", strIdentifierOfParameter)

        if (strIdentifierOfParameter === "")
            return;

        m_strIdentifierOfParameter = strIdentifierOfParameter
        refreshFrameButtonsEnable(strIdentifierOfParameter)
    }

    Connections {
        target: filterDock
        onPositionChanged: {
             refreshFrameButtonsEnable(m_strIdentifierOfParameter)

             var position = timeline.getPositionInCurrentClip()
             frameChanged(m_strIdentifierOfParameter, position)
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
        height: 120
        anchors.top: header.bottom
        anchors.topMargin:0
        visible:foldBtn.checked?true:false
        ColumnLayout
        {
            //columns: 5
            GridLayout
            {
                columns: 2
                Layout.topMargin: 4
                Layout.leftMargin: 24
                Label
                {
                    id: parameterLabel
                    text: qsTr('Parameter:')
                    color: '#ffffff'
                }

                MyComboBox
                {
                    id: comboboxParametersList
                    listModel: m_listModelParameters
                    height: 26

                    onCurrentIndexChanged:
                    {
                        if (currentIndex === -1)
                            return;
                        console.log("select parameter: ", m_listModelParameters.get(currentIndex).parameterIdentifier)
                        m_strIdentifierOfParameter = m_listModelParameters.get(currentIndex).parameterIdentifier
                        initUIWithParameter(m_listModelParameters.get(currentIndex).parameterIdentifier)
                    }
                }

                Label {id:spacer}

                CheckBox {
                    id: enableKeyFrameCheckBox

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
                                addKeyframe(m_strIdentifierOfParameter, comboboxAnimationType.currentIndex, sliderAnimationDuration.value)
                                refreshFrameButtonsEnable(m_strIdentifierOfParameter)
//                              autoAddKeyFrameCheckBox.checked = true
                            }
                        }
                        else
                        {
                            if(filter.isKeyframeActivate(m_strIdentifierOfParameter))
                                removeKeyFrameWarning.visible = true
                        }
                    }
                    onCheckedChanged: {
                        enableKeyFrameChanged(checked)
                    }
                }
                //            CheckBox {
                //                id: autoAddKeyFrameCheckBox
                //                visible: false
                //                Layout.columnSpan: 4
                //                anchors.left: parent.left
                //                anchors.leftMargin: 24
                //                anchors.top: enableKeyFrameCheckBox.bottom
                //                anchors.topMargin: 4
                //                text: qsTr('Auto Add Key Frames')
                //                checked: true
                //                opacity: enableKeyFrameCheckBox.checked ? 1.0 : 0.5
                //                onClicked: {
                //                }
                //                height:25
                //                style: CheckBoxStyle {
                //                    indicator: Rectangle {
                //                        color:'transparent'
                //                        radius: 3
                //                        z:1
                //                        implicitWidth: 18
                //                        implicitHeight: 18
                //                        Image {
                //                            z:2
                //                            anchors.fill: parent
                //                            source:autoAddKeyFrameCheckBox.checked ? 'qrc:///icons/light/32x32/key-a.png' : 'qrc:///icons/light/32x32/key.png'
                //                        }
                //                    }
                //                }
                //                onCheckedChanged:
                //                {
                //                    refreshFrameButtonsEnable(m_strIdentifierOfParameter)
                //                    autoAddKeyFrameChanged(checked)
                //                }
                //            }

                Label
                {
                    id: labelAnimationType
                    text: qsTr('Animation Type:')
                    color: '#ffffff'
                }

                MyComboBox
                {
                    id: comboboxAnimationType
                    listModel: ListModel{
                        ListElement {name: qsTr('In&Out Animation')}
                        ListElement {name: qsTr('In Animaiton')}
                        ListElement {name: qsTr('Out Animaiton')}
                    }

                    height: 26

                    onCurrentIndexChanged:
                    {
                    }
                }

                Label
                {
                    id: labelAnimationDuration
                    text: qsTr('Animation Duration:')
                    color: '#ffffff'
                }

                CustomSlider
                {
                    id: sliderAnimationDuration

                    minimumValue: 0.5
                    maximumValue: 5.0
                    stepSize: 0.5
                    tickmarksEnabled: true
                }

                Label
                {
                    id: labelKeyframe
                    visible: false
                    text: qsTr('Keyframe:')
                    color: '#ffffff'
                }
                Label
                {
                    id: labelKeyframeInfo
                    visible: false
                    text: qsTr('(Position: 00:00:00:00, Value:100)')
                    color: '#ffffff'
                }

                Label
                {
                    id: labelInterpolation
                    visible: false
                    text: qsTr('Interpolation:')
                    color: '#ffffff'
                }

                MyComboBox
                {
                    id: comboboxInterpolation
                    visible: false
                    listModel: ListModel{}
                    height: 26

                    onCurrentIndexChanged:
                    {
                    }
                }

            }

            RowLayout
            {
                id: rowLayoutButtons
                Layout.leftMargin: 24
                spacing: 12

                Button {
                    id:addKeyFrameButton
                    implicitWidth: 18
                    implicitHeight: 18

                    opacity: enabled ? 1.0 : 0.5
                    tooltip: qsTr('Add key frame')

                    onClicked: {
                        addKeyframe(m_strIdentifierOfParameter, comboboxAnimationType.currentIndex, sliderAnimationDuration.value)
                        refreshFrameButtonsEnable(m_strIdentifierOfParameter)
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

                implicitWidth: 18
                implicitHeight: 18

                opacity: enabled ? 1.0 : 0.5
                tooltip: qsTr('Remove key frame')

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
                    var position = timeline.getPositionInCurrentClip()

                    if((position === 0) || (position === (timeline.getCurrentClipLength() - 1)))//首尾帧无法删除
                        return

                    var bKeyFrame = filter.isKeyframeAtPosition(m_strIdentifierOfParameter, position)
                    if (bKeyFrame)
                        removeKeyFrame(m_strIdentifierOfParameter)

                    refreshFrameButtonsEnable(m_strIdentifierOfParameter)
                }
            }

            Button {
                id:preKeyFrameButton

                implicitWidth: 18
                implicitHeight: 18

                opacity: enabled ? 1.0 : 0.5
                tooltip: qsTr('Prev key frame')

                onClicked: {
                    var nFrame = filter.cache_getPreKeyFrameNum(timeline.getPositionInCurrentClip(), m_strIdentifierOfParameter)
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

                implicitWidth: 18
                implicitHeight: 18

                opacity: enabled ? 1.0 : 0.5
                tooltip: qsTr('Next key frame')

                onClicked: {
                    var nFrame = filter.cache_getNextKeyFrameNum(timeline.getPositionInCurrentClip(), m_strIdentifierOfParameter)
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
                    source: nextKeyFrameButton.pressed? "qrc:///icons/light/32x32/next_keyframe-on.png" : (enabled?'qrc:///icons/light/32x32/next_keyframe.png':'qrc:///icons/light/32x32/next_keyframe_disable.png')
                }
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

            removeAllKeyFrame(m_strIdentifierOfParameter)
        }
        onNo: 
            enableKeyFrameCheckBox.checked = true  
        
    }

    Connections
    {
        target: filterDock
        onCurrentFilterChanged:
        {
            enableKeyFrameCheckBox.checked = ( filter && filter.cache_getKeyFrameNumber() > 0)
//            autoAddKeyFrameChanged(autoAddKeyFrameCheckBox.checked)

            m_listModelParameters.clear()
            comboboxParametersList.currentIndex = -1
            if(metadata)
            {
                for (var i = 0; i < metadata.keyframes.parameterCount; i++)
                {
                    m_listModelParameters.append({name: metadata.keyframes.parameters[i].name, parameterIdentifier: metadata.keyframes.parameters[i].property})
                }
                comboboxParametersList.selectItemOfIndex(0);
            }
        }
    }

    Connections {
        target: filter
        onFilterPropertyValueChanged: {
            enableKeyFrameCheckBox.checked = (filter && filter.cache_getKeyFrameNumber() > 0)
//            autoAddKeyFrameChanged(autoAddKeyFrameCheckBox.checked)
        }
    }

    Connections
    {
        target: filter
        onEditKeyframeOfParameter:
        {
            keyFrame.visible = true
            foldBtn.checked = true
            switchFoldStat(true)
            initUIWithParameter(strIdentifierOfParameter)
        }
    }
}
