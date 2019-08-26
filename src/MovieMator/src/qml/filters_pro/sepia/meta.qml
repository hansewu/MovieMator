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
    name: qsTr("Sepia Tone")
    mlt_service: "sepia"
    qml: 'ui.qml'
    isFavorite: true
//    allowMultiple: false
    filterType: qsTr('5 Advanced Coloring Tool')
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['u','v']
        parameters: [
            Parameter {
                name: qsTr('u')
                property: 'u'
                objectName: 'sliderBlue'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '80'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('v')
                property: 'v'
                objectName: 'sliderRed'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '160'
                value: '0'
                factorFunc:  []
            }
        ]
    }
    objectName: 'sepiaTone'
}
