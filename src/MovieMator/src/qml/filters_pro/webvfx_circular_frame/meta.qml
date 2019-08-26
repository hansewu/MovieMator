/*
 * Copyright (c) 2011-2016 Meltytech, LLC
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
 */

import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    objectName: 'webvfxCircularFrame'
    name: qsTr("Circular Frame (HTML)")
    mlt_service: "webvfx"
    qml: "ui.qml"
    filterType: qsTr('6 Light')
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['radius']
        parameters: [
            Parameter {
                name: qsTr('resource')
                property: 'resource'
                objectName: '*'
                controlType: ''
                paraType: 'string'
                defaultValue: 'filter-demo.html'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('Radius')
                property: 'radius'
                objectName: '*'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0.5'
                value: '0'
                factorFunc:  ['c:100']
            }
            ,
            Parameter {
                name: qsTr('color')
                property: 'color'
                objectName: '*'
                controlType: 'ColorPicker'
                paraType: 'string'
                defaultValue: '#CCCCCC'
                value: '#000000'
                factorFunc:  []
            }
            ,
            Parameter {
                name: qsTr('disable')
                property: 'disable'
                objectName: '*'
                controlType: ''
                paraType: 'double'
                defaultValue: '0'
                value: ''
                factorFunc:  []
            }
        ]
    }
}
