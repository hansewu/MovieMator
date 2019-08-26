/*
 * Copyright (c) 2015 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
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
import com.moviemator.qml 1.0
Metadata {
    type: Metadata.Filter
    name: qsTr("Reduce Noise")
    mlt_service: 'avfilter.smartblur'
    qml: 'ui.qml'
    filterType: qsTr('4 Denoise and Blur')
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['av.luma_radius','av.chroma_radius','av.luma_strength','av.chroma_strength','av.luma_threshold','av.chroma_threshold']
        parameters: [
            Parameter {
                name: qsTr('luma_radius')
                property: 'av.luma_radius'
                objectName: 'smartblur1'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '2.5'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('chroma_radius')
                property: 'av.chroma_radius'
                objectName: 'smartblur2'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '2.5'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('luma_strength')
                property: 'av.luma_strength'
                objectName: 'smartblur3'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0.5'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('chroma_strength')
                property: 'av.chroma_strength'
                objectName: 'smartblur4'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0.5'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('luma_threshold')
                property: 'av.luma_threshold'
                objectName: 'smartblur5'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '3.0'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('chroma_threshold')
                property: 'av.chroma_threshold'
                objectName: 'smartblur6'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '3.0'
                value: '0'
                factorFunc:  []
            }
        ]
    }
    objectName: 'reduceNoise'
}
