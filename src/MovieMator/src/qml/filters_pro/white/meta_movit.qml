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
    name: qsTr("White Balance")
    objectName: "movit_white_balance"
    mlt_service: "movit.white_balance"
    needsGPU: true
    qml: "ui.qml"
    isFavorite: true
    filterType: qsTr('3 Basic Coloring Tool')
    // keyframes {
    //     allowAnimateIn: true
    //     allowAnimateOut: true
    //     simpleProperties: ['neutral_color','color_temperature']
    //     parameters: [
    //         Parameter {
    //             name: qsTr('neutral_color')
    //             property: 'neutral_color'
    //             isSimple: true
    //             isCurve: true
    //         },
    //         Parameter {
    //             name: qsTr('color_temperature')
    //             property: 'color_temperature'
    //             isSimple: true
    //             isCurve: true
    //         }
    //     ]
    // }
}
