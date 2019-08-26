/*
 * Copyright (c) 2014-2016 Meltytech, LLC
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
 * Author: Dragon-S <15919917852@163.com>
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

import QtQuick 2.1
import MovieMator.Controls 1.0

Flickable {
    property string fillProperty: 'fill'
    property string rectProperty: 'geometry'
    property string halignProperty: 'valign'
    property string valignProperty: 'halign'
    property var _locale: Qt.locale(application.numericLocale)

    width: 300
    height: 250
    interactive: false
    clip: true
    property real zoom: (video.zoom > 0)? video.zoom : 1.0
    property rect filterRect
    property bool blockUpdate: false
    contentWidth: video.rect.width  * zoom
    contentHeight: video.rect.height * zoom
    contentX: video.offset.x
    contentY: video.offset.y

    function getAspectRatio() {
        return (filter.get(fillProperty) === '1')? filter.producerAspect() : 0.0
    }

    function getAbsoluteRect(position) {
        var rect = filter.getRectOfTextFilter(rectProperty)
        if (position >= 0) {
            rect = filter.getAnimRectValue(position, rectProperty)
        }
        return Qt.rect(rect.x * profile.width, rect.y * profile.height, rect.width * profile.width, rect.height * profile.height)
    }

    function getRelativeRect(absoluteRect) {
        return Qt.rect(absoluteRect.x / profile.width, absoluteRect.y / profile.height, absoluteRect.width / profile.width, absoluteRect.height / profile.height)
    }

    function setKeyFrameParaValue (nFrame, currentPropert, value) {
        var paramCount = metadata.keyframes.parameterCount
        for(var i = 0; i < paramCount; i++) {
            var property = metadata.keyframes.parameters[i].property
            var paraType = metadata.keyframes.parameters[i].paraType
            if (property === currentPropert) {
                if (paraType === "rect") {
                    filter.cache_setKeyFrameParaRectValue(nFrame, property, value, 1.0)
                } else {
                    filter.cache_setKeyFrameParaValue(nFrame, property, value);
                }
            } else {
                if (paraType === "rect") {
                    var rectValue = filter.getAnimRectValue(nFrame, property)
                    filter.cache_setKeyFrameParaRectValue(nFrame, property, rectValue, 1.0)
                } else {
                    var valueStr = filter.get(property)
                    filter.cache_setKeyFrameParaValue(nFrame, property, valueStr);
                }

            }
        }
        filter.syncCacheToProject();
    }

    function setRectangleControl() {
        if (blockUpdate) return

        var newValue = getAbsoluteRect(-1)
        if (filter.cache_getKeyFrameNumber() > 0) {
            var position = timeline.getPositionInCurrentClip()
            newValue = getAbsoluteRect(position)
        }

        if (filterRect !== newValue) {
            filterRect = newValue
            rectangle.setHandles(filterRect)
        }
    }


    function isKeyFramePropterty(property)
    {
        var metaParamList = metadata.keyframes.parameters
        for(var paramIndex = 0; paramIndex < metaParamList.length; paramIndex++)
         {
                var prop = metaParamList[paramIndex].property
                if(prop === property) 
                {
                    return true
                }
         }

         return false
    }


    function updateFilter(currentProperty, value) {
        if (filter.enableAnimation() && isKeyFramePropterty(currentProperty === true)) 
        {
            var nFrame = timeline.getPositionInCurrentClip()
            if (filter.autoAddKeyFrame()) {
                if (!filter.cache_bKeyFrame(nFrame)) {
                    showAddFrameInfo(nFrame)
                }
                setKeyFrameParaValue(nFrame, currentProperty, value)
            } else {
                if (filter.cache_bKeyFrame(nFrame)) {
                    setKeyFrameParaValue(nFrame, currentProperty, value)
                } else {
                    filter.set(currentProperty, value)
                }
            }
        } else {
            filter.set(currentProperty, value)
        }
    }

    InfoDialog {
        id: addFrameInfoDialog
        text: qsTr('Auto set as key frame at postion')+ ": " + position + "."
        property int position: 0
    }

    function showAddFrameInfo(position)
    {
        if (filter.autoAddKeyFrame() === false) return

        addFrameInfoDialog.show     = false
        addFrameInfoDialog.show     = true
        addFrameInfoDialog.position = position
    }

    Component.onCompleted: {
        if (filter.cache_getKeyFrameNumber() > 0) {
            var position = timeline.getPositionInCurrentClip()
            filterRect = getAbsoluteRect(position)
        } else {
            filterRect = getAbsoluteRect(-1)
        }
        rectangle.setHandles(filterRect)
        setRectangleControl()
    }

    MouseArea {
        anchors.fill: parent
        onClicked: textEdit.focus = false
    }
    DropArea { anchors.fill: parent }

    Item {
        id: videoItem
        x: video.rect.x
        y: video.rect.y
        width: video.rect.width
        height: video.rect.height
        scale: zoom

        Rectangle {
            visible: false // DISABLED FOR NOW
            anchors.fill: textEdit
            color: 'white'
            opacity: textEdit.opacity * 0.5
        }
        TextEdit {
            visible: false // DISABLED FOR NOW
            id: textEdit
            x: Math.round(filterRect.x * rectangle.widthScale) + rectangle.handleSize
            y: Math.round(filterRect.y * rectangle.heightScale) + rectangle.handleSize
            width: Math.round(filterRect.width * rectangle.widthScale) - 2 * rectangle.handleSize
            height: Math.round(filterRect.height * rectangle.heightScale) - 2 * rectangle.handleSize
            horizontalAlignment: (filter.get('halign') === 'left')? TextEdit.AlignLeft
                               : (filter.get('halign') === 'right')? TextEdit.AlignRight
                               : TextEdit.AlignHCenter
            verticalAlignment: (filter.get('valign') === 'top')? TextEdit.AlignTop
                             : (filter.get('valign') === 'bottom')? TextEdit.AlignBottom
                             : TextEdit.AlignVCenter
            text: filter.get('argument')
            font.family: filter.get('family')
            font.pixelSize: 24 //0.85 * height / text.split("\n").length
            textMargin: filter.getDouble('pad')
            opacity: activeFocus
            onActiveFocusChanged: filter.set('disable', activeFocus)
            onTextChanged: filter.set('argument', text)
        }

        RectangleControl {
            id: rectangle
            widthScale: video.rect.width / profile.width
            heightScale: video.rect.height / profile.height
            aspectRatio: getAspectRatio()
            handleSize: Math.max(Math.round(8 / zoom), 4)
            borderSize: Math.max(Math.round(1.33 / zoom), 1)
            onWidthScaleChanged: {
                setHandles(filterRect)
            }
            onHeightScaleChanged: {
                setHandles(filterRect)
            }
            onRectChanged:  {
                blockUpdate = true
                filterRect.x = Math.round(rect.x / rectangle.widthScale)
                filterRect.y = Math.round(rect.y / rectangle.heightScale)
                filterRect.width = Math.round(rect.width / rectangle.widthScale)
                filterRect.height = Math.round(rect.height / rectangle.heightScale)

                updateFilter(rectProperty, getRelativeRect(filterRect))
//                if (filter.cache_getKeyFrameNumber() > 0) {
//                    var position = timeline.getPositionInCurrentClip()
//                    setKeyFrameParaValue(position, rectProperty, getRelativeRect(filterRect))
//                } else {
//                    filter.set(rectProperty, getRelativeRect(filterRect))
//                }
                blockUpdate = false
            }
        }
    }

    Connections {
        target: filter
        onFilterPropertyValueChanged: {
            setRectangleControl()
        }
    }

    Connections {
        target: filterDock
        onPositionChanged: {
            setRectangleControl()

            rectangle.visible = (timeline.getPositionInCurrentClip() >= 0) ? true : false
        }
    }
}
