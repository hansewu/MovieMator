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
    objectName: 'fadeInBrightness'
    name: qsTr("Fade In Video")
    mlt_service: "brightness"
    qml: "ui.qml"
    isFavorite: true
    gpuAlt: "movit.opacity"
    allowMultiple: false
    filterType: qsTr('1 Basic Processing')
    freeVersion: true
    keyframes {
        allowTrim: false
        allowAnimateIn: true
        parameters: [
            Parameter {
                name: qsTr('out')
                property: 'out'
                objectName: '*'
                controlType: ''
                paraType: 'double'
                defaultValue: '23'
                value: ''
                factorFunc:  []
            },
            Parameter {
                name: qsTr('alpha')
                property: 'alpha'
                objectName: '*'
                controlType: ''
                paraType: 'double'
                defaultValue: '1.0'
                value: ''
                factorFunc:  []
            }
        ]
    }
}
