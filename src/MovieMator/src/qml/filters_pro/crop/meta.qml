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
    name: qsTr("Crop")
    mlt_service: "crop"
    qml: "ui.qml"
    gpuAlt: "crop"
    objectName: 'crop'
//    allowMultiple: false
    isClipOnly: true
    freeVersion: true
    filterType: qsTr('1 Basic Processing')
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['level']
        parameters: [
            Parameter {
                name: qsTr('Center')
                property: 'center'
                objectName: 'centerCheckBox'
                controlType: 'CheckBox'
                paraType: 'int'
                defaultValue: '0'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('Center bias')
                property: 'center_bias'
                objectName: 'brightnessSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('Top')
                property: 'top'
                objectName: 'topslider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('Bottom')
                property: 'bottom'
                objectName: 'bottomslider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('Left')
                property: 'left'
                objectName: 'leftslider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('Right')
                property: 'right'
                objectName: 'rightslider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0'
                value: '0'
                factorFunc:  []
            }
        ]
    }
}
