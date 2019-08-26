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
    name: qsTr("Overlay HTML")
    mlt_service: "webvfx"
    qml: "ui.qml"
    isFavorite: true
    filterType: qsTr('9 Effect')
    objectName: 'overlayHTML'
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['disable']
        parameters: [
            Parameter {
                name: qsTr('disable')
                property: 'disable'
                objectName: '*'
                controlType: ''
                paraType: 'double'
                defaultValue: '1'
                value: ''
                factorFunc:  []
            }
        ]
    }
}
