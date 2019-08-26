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
    objectName: 'webvfxThreejsText'
    name: qsTr("Text3D(Eng.)")
    mlt_service: "webvfx"
    qml: "ui.qml"
    filterType: qsTr("2 Text")
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['resource','text','color','x_rotation','font','weight','bevel','depth','size','horizontal','vertical']
        parameters: [
            Parameter {
                name: qsTr('resource')
                property: 'resource'
                objectName: '*'
                controlType: ''
                paraType: 'string'
                defaultValue: 'threejs_text.html'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('text')
                property: 'text'
                objectName: '*'
                controlType: ''
                paraType: 'string'
                defaultValue: '3D Text'
                value: '*'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('color')
                property: 'color'
                objectName: '*'
                controlType: ''
                paraType: 'string'
                defaultValue: '#CCCCCC'
                value: ''
                factorFunc:  []
            }
            ,
            Parameter {
                name: qsTr('x_rotation')
                property: 'x_rotation'
                objectName: '*'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0.5'
                value: '*'
                factorFunc:  ['c:100.0']
            },
            Parameter {
                name: qsTr('font')
                property: 'font'
                objectName: '*'
                controlType: 'StringCtr'
                paraType: 'string'
                defaultValue: 'droid sans'
                value: '*'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('weight')
                property: 'weight'
                objectName: '*'
                controlType: 'StringCtr'
                paraType: 'string'
                defaultValue: 'bold'
                value: '*'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('bevel')
                property: 'bevel'
                objectName: '*'
                controlType: 'CheckBox'
                paraType: 'int'
                defaultValue: '0'
                value: '*'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('depth')
                property: 'depth'
                objectName: '*'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '20'
                value: '*'
                factorFunc:  ['c:2.0']
            },
            Parameter {
                name: qsTr('size')
                property: 'size'
                objectName: '*'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '70'
                value: '*'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('horizontal')
                property: 'horizontal'
                objectName: '*'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0.5'
                value: '*'
                factorFunc:  ['c:100.0']
            },
            Parameter {
                name: qsTr('vertical')
                property: 'vertical'
                objectName: '*'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0.5'
                value: '*'
                factorFunc:  ['c:100.0']
            }
        ]
    }
}
