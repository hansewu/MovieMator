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
    name: qsTr("Stabilize")
    mlt_service: "vidstab"
    qml: "ui.qml"
    isClipOnly: true
    isGpuCompatible: false
    isFavorite: false
//    allowMultiple: false
    filterType: qsTr('1 Basic Processing')
    keyframes {
        allowTrim: false
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['shakiness','accuracy','Zoom','refresh']
        parameters: [
            Parameter {
                name: qsTr('shakiness')
                property: 'shakiness'
                objectName: 'shakinessSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '1.0'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('accuracy')
                property: 'accuracy'
                objectName: 'accuracySlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '1.0'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('Zoom')
                property: 'zoom'
                objectName: 'zoomSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0'
                value: '0'
                factorFunc:  []
            }
            // ,
            // Parameter {
            //     name: qsTr('refresh')
            //     property: 'refresh'
            //     objectName: 'sliderBlue'
            //     controlType: 'SliderSpinner'
            //     paraType: 'int'
            //     defaultValue: '80'
            //     value: '0'
            //     factorFunc:  []
            // }
        ]
    }
    objectName: 'stabilize'
}
