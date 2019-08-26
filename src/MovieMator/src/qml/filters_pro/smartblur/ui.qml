/*
 * Copyright (c) 2016 Meltytech, LLC
 * Author: Brian Matherly <code@brianmatherly.com>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
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
import QtQuick.Layouts 1.1
import MovieMator.Controls 1.0

Item {
    property double radiusDefault: 2.5
    property double strengthDefault: 0.5
    property double thresholdDefault: 3.0

    property var defaultParameters: ["av.luma_radius", "av.chroma_radius", "av.luma_strength", "av.chroma_strength", "av.luma_threshold", "av.chroma_threshold"]

    width: 300
    height: 250
    Component.onCompleted: {
        if (filter.isNew) {
            filter.set("av.luma_radius", radiusDefault)
            filter.set("av.chroma_radius", radiusDefault)
            filter.set("av.luma_strength", strengthDefault)
            filter.set("av.chroma_strength", strengthDefault)
            filter.set("av.luma_threshold", thresholdDefault)
            filter.set("av.chroma_threshold", thresholdDefault)
            filter.savePreset(defaultParameters)
        }

        var keyFrameCount = filter.getKeyFrameCountOnProject("av.luma_radius");
        console.log("1......")
        console.log(keyFrameCount)
        if(keyFrameCount > 0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "av.luma_radius");
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, "av.luma_radius");
                filter.cache_setKeyFrameParaValue(nFrame, "av.luma_radius", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "av.chroma_radius");
                filter.cache_setKeyFrameParaValue(nFrame, "av.chroma_radius", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "av.luma_strength");
                filter.cache_setKeyFrameParaValue(nFrame, "av.luma_strength", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "av.chroma_strength");
                filter.cache_setKeyFrameParaValue(nFrame, "av.chroma_strength", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "av.luma_threshold");
                filter.cache_setKeyFrameParaValue(nFrame, "av.luma_threshold",  keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "av.chroma_threshold");
                filter.cache_setKeyFrameParaValue(nFrame, "av.chroma_threshold", keyValue)
            }

            filter.syncCacheToProject();

            radiusSlider.value = filter.getKeyValueOnProjectOnIndex(0, "av.luma_radius")
            strengthSlider.value = filter.getKeyValueOnProjectOnIndex(0, "av.luma_strength")
            thresholdSlider.value = filter.getKeyValueOnProjectOnIndex(0, "av.luma_threshold")
        }
        else
        {
            setControls()
        }
    }

    function setControls() {
        radiusSlider.value = filter.getDouble("av.luma_radius")
        strengthSlider.value = filter.getDouble("av.luma_strength")
        thresholdSlider.value = filter.getDouble("av.luma_threshold")
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        KeyFrame{
             id: keyFrame
             Layout.columnSpan:3
             onRefreshUI:
             {
                 var blurValue = filter.cache_getKeyFrameParaDoubleValue(keyFrameNum, "av.luma_radius");
                 if(blurValue !== -1.0)
                 {
                     radiusSlider.value = blurValue
                 }

                 blurValue = filter.cache_getKeyFrameParaDoubleValue(keyFrameNum, "av.luma_threshold");
                 if(blurValue !== -1.0)
                 {
                     thresholdSlider.value = blurValue

                 }

                 blurValue = filter.cache_getKeyFrameParaDoubleValue(keyFrameNum, "av.luma_strength");
                 if(blurValue !== -1.0)
                 {
                     strengthSlider.value = blurValue

                 }


             }
         }
        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        Preset {
            id: presetItem
            Layout.columnSpan: 2
            onPresetSelected: setControls()
        }

        Label {
            text: qsTr('Blur Radius')
            Layout.alignment: Qt.AlignRight
            ToolTip {text: qsTr('The radius of the gaussian blur.')}
            color: '#ffffff'
        }
        SliderSpinner {
            id: radiusSlider
            minimumValue: 0.1
            maximumValue: 5.0
            decimals: 1
            spinnerWidth: 80
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.cache_setKeyFrameParaValue(nFrame, "av.luma_radius", value)
                    filter.cache_setKeyFrameParaValue(nFrame, "av.chroma_radius", value)
                    filter.syncCacheToProject()
                }
                else{
                    filter.set("av.luma_radius", value)
                    filter.set("av.chroma_radius", value)
                }
            }
        }
        UndoButton {
            onClicked: radiusSlider.value = radiusDefault
        }

        Label {
            text: qsTr('Blur Strength')
            Layout.alignment: Qt.AlignRight
            ToolTip {text: qsTr('The strength of the gaussian blur.')}
            color: '#ffffff'
        }
        SliderSpinner {
            id: strengthSlider
            minimumValue: 0.0
            maximumValue: 1.0
            decimals: 1
            spinnerWidth: 80
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.cache_setKeyFrameParaValue(nFrame, "av.luma_radius", value)
                    filter.cache_setKeyFrameParaValue(nFrame, "av.chroma_radius", value)
                    filter.syncCacheToProject()
                }
                else{
                    filter.set("av.luma_strength", value)
                    filter.set("av.chroma_strength", value)
                }
                
            }
        }
        UndoButton {
            onClicked: strengthSlider.value = strengthDefault
        }
        
        Label {
            text: qsTr('Threshold')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
            ToolTip {text: qsTr('If the difference between the original pixel and the blurred pixel is less than threshold, the pixel will be replaced with the blurred pixel.')}
        }
        SliderSpinner {
            id: thresholdSlider
            minimumValue: 0.0
            maximumValue: 30
            decimals: 0
            spinnerWidth: 80
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.cache_setKeyFrameParaValue(nFrame, "av.luma_threshold", value)
                    filter.cache_setKeyFrameParaValue(nFrame, "av.chroma_threshold", value)
                    filter.syncCacheToProject()
                }
                else{
                    filter.set("av.luma_threshold", value)
                    filter.set("av.chroma_threshold", value)
                }
                
            }
        }
        UndoButton {
            onClicked: thresholdSlider.value = thresholdDefault
        }

        Item { Layout.fillHeight: true }
    }
}
