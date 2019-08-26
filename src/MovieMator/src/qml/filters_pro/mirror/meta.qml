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
    name: qsTr("Mirror")
    mlt_service: "mirror"
    gpuAlt: "movit.mirror"
    qml: "ui.qml"
    filterType: qsTr('9 Effect')
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['disable']
        parameters: [
            Parameter {
                name: qsTr('mirror')
                property: 'mirror'
                objectName: '*'
                controlType: ''
                paraType: 'string'
                defaultValue: 'flip'
                value: ''
                factorFunc:  []
            }
        ]
    }
    objectName: 'mirror'
}
