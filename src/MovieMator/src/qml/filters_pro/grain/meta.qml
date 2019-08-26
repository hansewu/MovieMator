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
    name: qsTr("Old Film: Grain")
    mlt_service: "grain"
    qml: "ui.qml"
    filterType: qsTr('9 Effect')
    keyframes {
        minimumVersion: '3'
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['noise', 'vert']
        parameters: [
            Parameter {
                name: qsTr('noise')
                property: 'noise'
                objectName: 'noiseSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '1.0'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('brightness')
                property: 'brightness'
                objectName: 'brightnessSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0.83'
                value: '0'
                factorFunc:  []
            }
        ]
    }
    objectName: 'oldFilmGrain'
}
