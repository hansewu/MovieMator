/*
 * Copyright (c) 2014-2015 Meltytech, LLC
 * Author: Brian Matherly <pez4brian@yahoo.com>
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
 
import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.1

Item {
    property var defaultParameters: []
    property var neutralParam
    property var tempParam
    property string defaultNeutral: "#7f7f7f"
    property double defaultTemp: 6500.0
    property double tempScale: 15000.0
    width: 300
    height: 250

    Component.onCompleted: {
        if (filter.get("mlt_service").indexOf("movit.") === 0 ) {
            // Movit filter
            neutralParam = "neutral_color"
            tempParam = "color_temperature"
            tempScale = 1;
        } else {
            // Frei0r filter
            neutralParam = "0"
            tempParam = "1"
            tempScale = 15000.0
        }
        defaultParameters = [neutralParam, tempParam]
        presetItem.parameters = defaultParameters

        if (filter.isNew) {
            // Set default parameter values
            filter.set(neutralParam, defaultNeutral)
            filter.set(tempParam, defaultTemp / tempScale)
            filter.savePreset(defaultParameters)
        }

        var keyFrameCount = filter.getKeyFrameCountOnProject(tempParam);
        if(keyFrameCount > 0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, tempParam);
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, tempParam);
                filter.cache_setKeyFrameParaValue(nFrame, tempParam, keyValue)

                keyValue = filter.getStringKeyValueOnProjectOnIndex(index, neutralParam);
                filter.cache_setKeyFrameParaValue(nFrame,neutralParam, keyValue)

            }
            filter.syncCacheToProject();

            var frame = filter.getKeyFrameOnProjectOnIndex(0, tempParam);

            tempslider.value = parseFloat(filter.getKeyFrameOnProjectOnIndex(0, tempParam) * tempScale)
            tempspinner.value = parseFloat(filter.getKeyValueOnProjectOnIndex(0, neutralParam) * tempScale)
            colorPicker.value = filter.cache_getKeyFrameParaValue(frame, neutralParam)//filter.getKeyValueOnProjectOnIndex(0, ""+neutralParam)
        }
        else
        {
            tempslider.value = filter.getDouble(tempParam) * tempScale
            tempspinner.value = filter.getDouble(tempParam) * tempScale


            colorPicker.value = filter.get(neutralParam)
        }
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 20
        rowSpacing: 15
        

        Label {
            text: qsTr('Preset') + "      "
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
        }
        Preset {
            id: presetItem
            Layout.columnSpan: 2
            onPresetSelected: {
                tempslider.value = filter.getDouble(tempParam) * tempScale
                colorPicker.value = filter.get(neutralParam)
            }
        }
        
        SeparatorLine {
            Layout.fillWidth: true 
            Layout.columnSpan: 3
        }

        // Row 1
        Label {
            text: qsTr('Neutral color')
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
        }
        ColorPicker {
            id: colorPicker
            property bool isReady: false
            Component.onCompleted: isReady = true
            onValueChanged: {
                if (isReady) {
                    
                    filter.set(neutralParam, ""+value)
                    
                    filter.set("disable", 0);
                }
            }
            onPickStarted: {
                filter.set("disable", 1);
            }
        }
        UndoButton {
            onClicked: colorPicker.value = defaultNeutral
        }

        // Row 2
        Label {
            text: qsTr('Color temperature')
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
        }
        RowLayout {
            Slider {
                id: tempslider
                Layout.fillWidth: true
                Layout.maximumHeight: tempspinner.height
                style: SliderStyle {
                    groove: Rectangle {
                        rotation: -90
                        height: parent.width
                        x: (parent.width - width) / 2
                        gradient: Gradient {
                            GradientStop { position: 0.000; color: "#FFC500" }
                            GradientStop { position: 0.392; color: "#FFFFFF" }
                            GradientStop { position: 1.000; color: "#DDFFFE" }
                        }
                        radius: 4
                        onWidthChanged: {
                            // Force width (which is really height due to rotation).
                            width = tempslider.height / 2
                        }
                    }
                    handle: Rectangle {
                        color: "lightgray"
                        border.color: "gray"
                        border.width: 2
                        width: height / 2
                        height: tempslider.height
                        radius: 4
                    }
                }
                minimumValue: 1000.0
                maximumValue: 15000.0
                property bool isReady: false
                Component.onCompleted: isReady = true
                onValueChanged: {
                    if (isReady) {
                        
                            tempspinner.value = value
                            filter.set(tempParam, value / tempScale)
                        
                    }
                }
            }
            SpinBox {
                id: tempspinner
                Layout.minimumWidth: 100
                minimumValue: 1000.0
                maximumValue: 15000.0
                decimals: 0
                stepSize: 10
                suffix: ' deg'
                onValueChanged: tempslider.value = value
            }
        }
        UndoButton {
            onClicked: tempslider.value = defaultTemp
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
