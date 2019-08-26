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
    name: qsTr("Wave")
    objectName: "wave"
    mlt_service: "wave"
    qml: "ui.qml"
    isFavorite: false
//    allowMultiple: false
    filterType: qsTr('7 Distort')
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['wave','speed','deformX','deformY']
        parameters: [
            Parameter {
                name: qsTr('wave')
                property: 'wave'
                objectName: 'waveSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '1.0'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('speed')
                property: 'speed'
                objectName: 'speedSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0.05'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('deformX')
                property: 'deformX'
                objectName: 'deformXCheckBox'
                controlType: 'CheckBox'
                paraType: 'int'
                defaultValue: '0'
                value: ''
                factorFunc:  []
            },
            Parameter {
                name: qsTr('deformY')
                property: 'deformY'
                objectName: 'deformYCheckBox'
                controlType: 'CheckBox'
                paraType: 'int'
                defaultValue: '0'
                value: ''
                factorFunc:  []
            }
        ]
    }
}
