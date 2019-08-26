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
    name: qsTr("Color Grading")
    mlt_service: "movit.lift_gamma_gain"
    needsGPU: true
    qml: "ui.qml"
    isFavorite: true
    filterType: qsTr('5 Advanced Coloring Tool')
    // keyframes {
    //     parameters: [
    //         Parameter {
    //             name: qsTr('Shadows (Lift)')
    //             property: 'lift_r'
    //         },
    //         Parameter {
    //             name: qsTr('Midtones (Gamma)')
    //             property: 'gamma_r'
    //         },
    //         Parameter {
    //             name: qsTr('Highlights (Gain)')
    //             property: 'gain_r'
    //         }
    //     ]
    // }
}
