/*
 * Copyright (c) 2014-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
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

import QtQuick 2.1
import MovieMator.Controls 1.0

Flickable {
    id: spVui
    property string rectProperty
    property string fillProperty
    property string distortProperty
    property string halignProperty
    property string valignProperty
    property var _locale: Qt.locale(application.numericLocale)
    property rect rectCtr2
    property string metaValue: ''

    property var fillStat: filter ? filter.get(fillProperty) : ''
    property var distortStat: filter ? filter.get(distortProperty) : ''
    property var valignStat: filter ? filter.get(valignProperty) : ''
    property var halignStat: filter ? filter.get(halignProperty) : ''

    width: 400
    height: 200
    interactive: false
    clip: true
    property real zoom: (video.zoom > 0)? video.zoom : 1.0
    property rect filterRect: filter.getRect(rectProperty)
    contentWidth: video.rect.width * zoom
    contentHeight: video.rect.height * zoom
    contentX: video.offset.x
    contentY: video.offset.y

    function getAspectRatio() {
        if (!filter) return 0.0

        return (filter.get(fillProperty) === '1' && filter.get(distortProperty) === '0')? filter.producerAspect() : 0.0
    }

    function updateRectangleUI()
    {
        var rect = filter.getRect(rectProperty)

        if (filter.cache_getKeyFrameNumber(rectProperty) > 0)
        {   
            var position = timeline.getPositionInCurrentClip()
            rect = filter.getAnimRectValue(position, rectProperty)
        } 
        
        var rectHandles = Qt.rect(rect.x * profile.width, rect.y * profile.height, rect.width * profile.width, rect.height * profile.height)
        rectangle.setHandles(rectHandles)
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

    function setFilterRect()
    {
        var position = timeline.getPositionInCurrentClip()
        if (!filter.isKeyframeActivate(rectProperty))
        {
             filter.set(rectProperty, filterRect)
        }
        else if ((filter.isKeyframeActivate(rectProperty) && filter.autoAddKeyFrame())
                 || filter.isKeyframeAtPosition(rectProperty, position))
        {
            if (!filter.isKeyframeAtPosition(rectProperty, position))
                showAddFrameInfo(position)
            filter.cache_setKeyFrameParaRectValue(position, rectProperty, filterRect, 1.0)
        }
    }

    Component.onCompleted: {

        var rectT = filter.getRect(rectProperty)
        if (filter.cache_getKeyFrameNumber(rectProperty) > 0)
        {
            var position = timeline.getPositionInCurrentClip()
            filterRect = filter.getAnimRectValue(position, rectProperty)
        }
        else
        {
            filterRect = filter.getRect(rectProperty)
        }

        rectT.x = filterRect.x * profile.width
        rectT.y = filterRect.y * profile.height
        rectT.width = filterRect.width * profile.width
        rectT.height = filterRect.height * profile.height

        rectangle.setHandles(rectT)
    }

    DropArea { anchors.fill: parent }

    Item {
        id: videoItem
        x: video.rect.x
        y: video.rect.y
        width: video.rect.width
        height: video.rect.height
        scale: zoom

        RectangleControl {
            id: rectangle
            widthScale: video.rect.width / profile.width
            heightScale: video.rect.height / profile.height
            aspectRatio: getAspectRatio()
            handleSize: Math.max(Math.round(8 / zoom), 4)
            borderSize: Math.max(Math.round(1.33 / zoom), 1)
            onWidthScaleChanged: {
                updateRectangleUI()
            }
            onHeightScaleChanged: {
                updateRectangleUI()
            }
            onRectChanged:  {
                filterRect.x        = rect.x / rectangle.widthScale / profile.width
                filterRect.y        = rect.y / rectangle.heightScale / profile.height
                filterRect.width    = rect.width / rectangle.widthScale / profile.width
                filterRect.height   = rect.height / rectangle.heightScale / profile.height
                
                setFilterRect()
            }
        }
    }

    Connections {
        target: filterDock
        onPositionChanged: {
            
            if(metaValue !== metadata.keyframes.parameters[0].value){
                metaValue = metadata.keyframes.parameters[0].value
                var x = metaValue.substring(1,metaValue.indexOf('Y'))
                var y = metaValue.substring(metaValue.indexOf('Y')+1,metaValue.indexOf('W'))
                var w = metaValue.substring(metaValue.indexOf('W')+1,metaValue.indexOf('H'))
                var h = metaValue.substring(metaValue.indexOf('H')+1)

                rectCtr2.x      = parseFloat(x) * profile.width
                rectCtr2.y      = parseFloat(y) * profile.height
                rectCtr2.width  = parseFloat(w) * profile.width
                rectCtr2.height = parseFloat(h) * profile.height
                rectangle.setHandles(rectCtr2)
                
            }

            rectangle.visible = (timeline.getPositionInCurrentClip() >= 0) ? true : false 
        }
    }
}
