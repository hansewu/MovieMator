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
    name: qsTr("Vignette")
    mlt_service: "movit.vignette"
    needsGPU: true
    qml: "ui_movit.qml"
    filterType: qsTr('6 Light')
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['radius', 'inner_radius']
        parameters: [
            Parameter {
                name: qsTr('Outer radius')
                property: 'radius'
                objectName: 'radiusSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0.30'
                value: '0'
                factorFunc:  ['c:100.0']
            },
            Parameter {
                name: qsTr('Inner radius')
                property: 'inner_radius'
                objectName: 'innerSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0.30'
                value: '0'
                factorFunc:  ['c:100.0']
            }
        ]
    }
}
