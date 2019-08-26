/*
 * Copyright (c) 2014-2016 Meltytech, LLC
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
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.0

Item {
    property string fillProperty
    property string distortProperty
    property string rectProperty
    property string valignProperty
    property string halignProperty
    property var _locale: Qt.locale(application.numericLocale)
    property rect filterRect : filter.getRect(rectProperty)
    property rect rectTmp
    property rect rectOld  //保存了当前的rect值，这样remove all keyframes之后（底层数据清空），可以将此值用于更新底层。不然也可以直接将界面的控件值设置到底层，不过此时这个滤镜需要使用rectangle.rect 
    property var clickedButton
    property bool bTile: false
    property bool bFit: false
    property bool bFitCrop: false
    property bool bFitNoScale: false

    width: 350
    height: 250

    Component.onCompleted: {
        
        filter.setInAndOut(0, timeline.getCurrentClipParentLength() - 1)
        //导入上次工程保存的关键帧
        var metaParamList = metadata.keyframes.parameters
        /*
        var keyFrameCount = filter.getKeyFrameCountOnProject(metaParamList[0].property);
        if(keyFrameCount < 0){
                keyFrameCount = filter.getKeyFrameCountOnProject(metaParamList[0].property);
        }
        for(var keyIndex=0; keyIndex<keyFrameCount;keyIndex++)
        {
            var nFrame = filter.getKeyFrameOnProjectOnIndex(keyIndex, metaParamList[0].property)
            for(var paramIndex=0;paramIndex<metaParamList.length;paramIndex++){
                var prop = metaParamList[paramIndex].property
                var keyValue = filter.getAnimRectValue(nFrame, prop)
                filter.cache_setKeyFrameParaRectValue(nFrame, prop, keyValue)
            }
        }
        filter.syncCacheToProject();
        */
        if (filter.isNew) {
            filter.set(fillProperty, 1)
            filter.set(distortProperty, 0)

            // loadPresets()

            rectTmp.x = 0.0
            rectTmp.y = 0.0
            rectTmp.width = 1.0
            rectTmp.height = 1.0
            
            filter.set(rectProperty, rectTmp)
            filter.set(valignProperty, 'top')
            filter.set(halignProperty, 'left')
            filter.savePreset(preset.parameters)
        }
        setControls()

        // metadata.keyframes.parameters[0].value = rectTmp

        keyFrame.initFilter()
    }
           
        
    function loadPresets()
    {
        rectTmp.x       = 0.0
        rectTmp.y       = 0.5
        rectTmp.width   = 0.5
        rectTmp.height  = 0.5

        filter.set(rectProperty,   rectTmp)
        filter.savePreset(preset.parameters, qsTr('Bottom Left'))

        rectTmp.x       = 0.5
        rectTmp.y       = 0.5
        rectTmp.width   = 0.5
        rectTmp.height  = 0.5
        filter.set(rectProperty,   rectTmp)
        filter.savePreset(preset.parameters, qsTr('Bottom Right'))

        rectTmp.x       = 0.0
        rectTmp.y       = 0.0
        rectTmp.width   = 0.5
        rectTmp.height  = 0.5
        filter.set(rectProperty,   rectTmp)
        filter.savePreset(preset.parameters, qsTr('Top Left'))

        rectTmp.x       = 0.5
        rectTmp.y       = 0.0
        rectTmp.width   = 0.5
        rectTmp.height  = 0.5
        filter.set(rectProperty,   rectTmp)
        filter.savePreset(preset.parameters, qsTr('Top Right'))
    } 

    function setFilter() {
        var x = parseFloat(rectX.text)
        var y = parseFloat(rectY.text)
        var w = parseFloat(rectW.text)
        var h = parseFloat(rectH.text)

        filterRect.x = x / profile.width
        filterRect.y = y / profile.height
        filterRect.width = w / profile.width
        filterRect.height = h / profile.height

        if(!keyFrame.bEnableKeyFrame)  //没有关键帧
        {   
            filter.resetProperty(rectProperty)
            filter.set(rectProperty, filterRect)
        }
        else if ((keyFrame.bEnableKeyFrame && keyFrame.bAutoSetAsKeyFrame) || filter.cache_bKeyFrame(keyFrame.getCurrentFrame()))
        {
            var nFrame = keyFrame.getCurrentFrame()
            
            if (!filter.cache_bKeyFrame(nFrame)) keyFrame.showAddFrameInfo(nFrame)

            //var rectValue = filter.getRect(rectProperty)
            filter.cache_setKeyFrameParaRectValue(nFrame, rectProperty, filterRect,1.0)
            filter.syncCacheToProject();
        } 
               
    }

    function saveValues() {
        
        var x = parseFloat(rectX.text)
        var y = parseFloat(rectY.text)
        var w = parseFloat(rectW.text)
        var h = parseFloat(rectH.text)
        filterRect.x        = x / profile.width
        filterRect.y        = y / profile.height
        filterRect.width    = w / profile.width
        filterRect.height   = h / profile.height

        filter.set(rectProperty, filterRect)
        
    }

    function setControls() {
        if (filter.get(distortProperty) === '1')
            distortRadioButton.checked = true
        else if (filter.get(fillProperty) === '1')
            fillRadioButton.checked = true
        else
            fitRadioButton.checked = true
    }

    function changeMode(){
        if(fitRadioButton.checked == true){
            filter.set(fillProperty, 1)
            filter.set(distortProperty, 1)
            
            filter.set(fillProperty, 0)
            filter.set(distortProperty, 0)
        }else if(fillRadioButton.checked == true){
            filter.resetProperty(fillProperty)
            filter.resetProperty(distortProperty)

            filter.set(fillProperty, 1)
            filter.set(distortProperty, 0)
        }else if(distortRadioButton.checked === true){
            filter.resetProperty(fillProperty)
            filter.resetProperty(distortProperty)

            filter.set(fillProperty, 1)
            filter.set(distortProperty, 1)
        }
    }

    function setPerset(){
        var position = timeline.getPositionInCurrentClip()
        filter.get(rectProperty)
        var rect = filter.getAnimRectValue(position, rectProperty)
        var rect3 = filter.get(rectProperty)

        filterRect.x = rect.x
        filterRect.y = rect.y
        filterRect.width = rect.width
        filterRect.height = rect.height

        setFilter()
        setControls()
    }

    function tile()
    {
        bTile = false

        filter.resetProperty(fillProperty)
        filter.resetProperty(distortProperty)

        filter.set(fillProperty, 1)
        filter.set(distortProperty, 1)

        filterRect.x = 0
        filterRect.y = 0
        filterRect.width = 1
        filterRect.height = 1

        setFilter()
        setControls()
    }

    function fitNoScale()
    {
        bFitNoScale = false

        filter.resetProperty(fillProperty)
        filter.resetProperty(distortProperty)

        filter.set(fillProperty, 1)
        filter.set(distortProperty, 0)

        var scale = 1
        if(filter.producerAspect() > profile.width/profile.height) 
        {
            if ((filter.mediaHeight !== -1) && (filter.mediaWidth !== -1))
                scale = filter.mediaWidth/profile.width

            filterRect.height   = 1.0/filter.producerAspect() * profile.width/profile.height * scale
            filterRect.width    = 1 * scale
        }
        else
        {   
            if ((filter.mediaHeight !== -1) && (filter.mediaWidth !== -1))
                scale = filter.mediaHeight/profile.height

            filterRect.width    = filter.producerAspect() * profile.height/profile.width * scale
            filterRect.height   = 1 * scale
        }
        filterRect.x        = (1.0 - filterRect.width)/2.0
        filterRect.y        = (1.0 - filterRect.height)/2.0

        setFilter()
        setControls()
    }

    function fit()
    {
        bFit = false

        filter.resetProperty(fillProperty)
        filter.resetProperty(distortProperty)

        filter.set(fillProperty, 1)
        filter.set(distortProperty, 0)

        if(filter.producerAspect() > profile.width/profile.height) 
        {
            filterRect.height   = 1.0/filter.producerAspect() * profile.width/profile.height
            filterRect.width    = 1
        }
        else
        {
            filterRect.width    = filter.producerAspect() * profile.height/profile.width
            filterRect.height   = 1
        }
        filterRect.x        = (1.0 - filterRect.width)/2.0
        filterRect.y        = (1.0 - filterRect.height)/2.0


        setFilter()
        setControls()
    }

    function fitCrop()
    {
        bFitCrop = false

        filter.resetProperty(fillProperty)
        filter.resetProperty(distortProperty)

        filter.set(fillProperty, 1)
        filter.set(distortProperty, 0)


        if(filter.producerAspect() > profile.width/profile.height) 
        {
            filterRect.height   = 1.0 
            filterRect.width    = 1.0/(1.0/filter.producerAspect() * profile.width/profile.height)
        }
        else
        {
            filterRect.width    = 1.0
            filterRect.height   = 1/(filter.producerAspect() * profile.height/profile.width)
        }
        filterRect.x        = (1.0 - filterRect.width)/2.0
        filterRect.y        = (1.0 - filterRect.height)/2.0

        setFilter()
        setControls()
    }
    

    ExclusiveGroup { id: sizeGroup }
    ExclusiveGroup { id: halignGroup }
    ExclusiveGroup { id: valignGroup }

    GridLayout {
        columns: 5
        rowSpacing: 13
        columnSpacing: 20
        anchors.fill: parent
        anchors.margins: 18

        KeyFrame{
            id: keyFrame
            Layout.columnSpan:5
            onSyncUIDataToProject:{
                if(filter.cache_getKeyFrameNumber() <= 0){
                    filter.resetProperty(rectProperty)
                    filter.set(rectProperty, rectOld)
                }
                else
                {   
                    
                }
            }
            onSetAsKeyFrame:{
                var nFrame = keyFrame.getCurrentFrame()
                //saveValues()
            
                //var rectValue = filter.getRect(rectProperty)
                var rectValue = filter.getAnimRectValue(nFrame, rectProperty)
                if (filter.cache_getKeyFrameNumber() <= 0)
                {
                    var position2 = (timeline.getCurrentClipLength() - 1) //filter.producerOut - filter.producerIn + 1
                    filter.cache_setKeyFrameParaRectValue(0, rectProperty, rectValue)
                    filter.cache_setKeyFrameParaRectValue(position2, rectProperty, rectValue,1.0)
                    //filter.syncCacheToProject();
                }
                filter.cache_setKeyFrameParaRectValue(nFrame, rectProperty, rectValue,1.0)
                //filter.syncCacheToProject();

                setControls()
            }
            onRefreshUI:
            {   
                var nFrame = keyFrame.getCurrentFrame()
                var rect = filter.getAnimRectValue(keyFrameNum, rectProperty)
                rectOld  = filter.getAnimRectValue(keyFrameNum, rectProperty)
                filterRect.x = rect.x
                filterRect.y = rect.y
                filterRect.width = rect.width
                filterRect.height = rect.height   

                metadata.keyframes.parameters[0].value = 'X'+ rect.x +'Y'+rect.y+'W'+rect.width+'H'+rect.height 

                setControls()
    
            }
        }

        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
        }
        Preset {
            id: preset
            parameters: [fillProperty, distortProperty, rectProperty, halignProperty, valignProperty]
            Layout.columnSpan: 4
            onPresetSelected: setPerset()
        }

        SeparatorLine {
            Layout.columnSpan: 5
            Layout.minimumWidth: parent.width
            Layout.maximumWidth: parent.width
        }

        Label {
            text: qsTr('Size mode')
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
        }
       
        RadioButton {
            id: fitRadioButton
            text: qsTr('Fit')
            visible:false
            exclusiveGroup: sizeGroup
            onClicked: {
                if(filter.cache_getKeyFrameNumber() > 0){
                    sizeKeyFrameWarning.visible = true
                }
                else{
                    filter.set(fillProperty, 1)
                    filter.set(distortProperty, 1)

                    filter.set(fillProperty, 0)
                    filter.set(distortProperty, 0)
                }
                
            }
        }
        RadioButton {
            id: fillRadioButton
            text: qsTr('Fill')
            exclusiveGroup: sizeGroup
            onClicked: {
                if(filter.cache_getKeyFrameNumber() > 0){
                    sizeKeyFrameWarning.visible = true
                }
                else{
                    filter.set(fillProperty, 1)
                    filter.set(distortProperty, 0)
                }
            }
        }
        RadioButton {
            id: distortRadioButton
            text: qsTr('Distort')
            exclusiveGroup: sizeGroup
            onClicked: {
                if(filter.cache_getKeyFrameNumber() > 0){
                    sizeKeyFrameWarning.visible = true
                }
                else{
                    filter.set(fillProperty, 1)
                    filter.set(distortProperty, 1)
                }
                
            }
        }

        Item { Layout.fillWidth: true }

        Label {
            text: qsTr('Position')
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
            visible: false
        }
        RowLayout {
            Layout.columnSpan: 4
            visible: false
            TextField {
                id: rectX
                text: (filterRect.x * profile.width).toFixed()
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
            }
            Label { 
                text: ',' 
                color: '#ffffff'
            }
            TextField {
                id: rectY
                text: (filterRect.y * profile.height).toFixed()
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
            }
        }
        Label {
            text: qsTr('Size')
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
            visible: false
        }
        RowLayout {
            visible: false
            Layout.columnSpan: 4
            //minimumHeight:4
            TextField {
                id: rectW
                text: (filterRect.width * profile.width).toFixed()
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
            }
            Label { 
                text: 'x' 
                color: '#ffffff'
            }
            TextField {
                id: rectH
                text: (filterRect.height * profile.height).toFixed()
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
            }
            
        }
    

        SeparatorLine {
            Layout.columnSpan: 5
            Layout.minimumWidth: parent.width
            Layout.maximumWidth: parent.width
        }

        Button {
            id: fit1Button
            tooltip: qsTr('1:1')
            Layout.alignment: Qt.AlignRight
            onClicked: {
                bFitNoScale = true

                if(filter.cache_getKeyFrameNumber() > 0)   // 关键帧
                {   
                    if(fillRadioButton.checked)   //填配模式
                    {
                        fitNoScale()
                    }
                    else
                        sizeKeyFrameWarning.visible = true
                }
                else          
                {
                    fitNoScale()
                }
            }

            implicitWidth: 56
            implicitHeight: 41
            style: ButtonStyle {
                background: Rectangle {
                    color: 'transparent'
                }  
            }
            Image {
                fillMode: Image.PreserveAspectCrop
                anchors.fill: parent
                source: fit1Button.pressed? "qrc:///icons/light/32x32/size-fit-1-a.png" : "qrc:///icons/light/32x32/size-fit-1.png"
            }
        }

        Button {
            id: fitButton
            //text: qsTr('Fit')
            tooltip: qsTr('Fit')
            //iconSource: "qrc:///icons/light/32x32/size-fit.png"
            Layout.alignment: Qt.AlignRight
            onClicked: {
                bFit = true

                if(filter.cache_getKeyFrameNumber() > 0)   // 关键帧
                {   
                    if(fillRadioButton.checked)   //填配模式
                    {
                        fit()
                    }
                    else
                        sizeKeyFrameWarning.visible = true
                }
                else          
                {
                    fit()
                }
            }

            implicitWidth: 56
            implicitHeight: 41
            style: ButtonStyle {
                background: Rectangle {
                    color: 'transparent'
                }  
            }
            Image {
                fillMode: Image.PreserveAspectCrop
                anchors.fill: parent
                source: fitButton.pressed? "qrc:///icons/light/32x32/size-fit-a.png" : "qrc:///icons/light/32x32/size-fit.png"
            }
        }

        Button {
            id: fitCropButton
            //text: qsTr('FitCrop')
            tooltip: qsTr('FitCrop')
            //iconSource: "qrc:///icons/light/32x32/size-fit-crop.png"
            Layout.alignment: Qt.AlignRight
            onClicked: {
                bFitCrop = true

                if(filter.cache_getKeyFrameNumber() > 0)   // 关键帧
                {   
                    if(fillRadioButton.checked)   //填配模式
                    {
                        fitCrop()
                    }
                    else
                        sizeKeyFrameWarning.visible = true
                }
                else          
                {
                    fitCrop()
                }
            }
            implicitWidth: 56
            implicitHeight: 41
            style: ButtonStyle {
                background: Rectangle {
                    color: 'transparent'
                }  
            }
            Image {
                fillMode: Image.PreserveAspectCrop
                anchors.fill: parent
                source: fitCropButton.pressed? "qrc:///icons/light/32x32/size-fit-crop-a.png" : "qrc:///icons/light/32x32/size-fit-crop.png"
            }
        }

        Button {
            id: tileButton
            //text: qsTr('Tile')
            tooltip: qsTr('Tile')
            //iconSource: "qrc:///icons/light/32x32/size-tile.png"
            //width:100
            Layout.alignment: Qt.AlignLeft
            onClicked: {
                bTile = true

                if(filter.cache_getKeyFrameNumber() > 0)   // 关键帧
                {   
                    if(distortRadioButton.checked)   //变形模式
                    {
                        tile()
                    }
                    else
                        sizeKeyFrameWarning.visible = true
                }
                else          
                {
                    tile()
                }
            }
            implicitWidth: 56
            implicitHeight: 41
            style: ButtonStyle {
                background: Rectangle {
                    color: 'transparent'
                }  
            }
            Image {
                fillMode: Image.PreserveAspectCrop
                anchors.fill: parent
                source: tileButton.pressed? "qrc:///icons/light/32x32/size-tile-a.png" : "qrc:///icons/light/32x32/size-tile.png"
            }
        }

        
    }

    Connections {
        target: filter
        onFilterPropertyValueChanged: {
            var keyFrameNum = timeline.getPositionInCurrentClip()
            var newValue = filter.getRect(rectProperty)
            if (filterRect !== newValue)
                filterRect = newValue
        }
    }

    MessageDialog {
        id: sizeKeyFrameWarning
        title: qsTr("May I have your attention please")
        text: qsTr("Change mode will remove all of the key frames, are you sure to do?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            var keyFrameNum = timeline.getPositionInCurrentClip()
            var rect = filter.getAnimRectValue(keyFrameNum, rectProperty)
            filter.removeAllKeyFrame()
            
            filter.resetProperty(rectProperty)
            filter.set(rectProperty,rect)

            changeMode()
            
            if (bTile) tile()
            if (bFit) fit()
            if (bFitCrop) fitCrop()
            if (bFitNoScale) fitNoScale()
        }
        onNo: {
            if (bTile) bTile = false
            if (bFit) bFit = false
            if (bFitCrop) bFitCrop = false
            if (bFitNoScale) bFitNoScale = false

            setControls()  
        }
        Component.onCompleted: visible = false
    }
}
