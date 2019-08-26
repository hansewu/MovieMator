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
    mlt_service: "lift_gamma_gain"
    qml: "ui.qml"
    isFavorite: true
    gpuAlt: "movit.lift_gamma_gain"
    filterType: qsTr('5 Advanced Coloring Tool')
    keyframes {
        parameters: [
            Parameter {
                name: qsTr('Shadows (Lift)')
                property: 'lift_r'
                objectName: 'liftwheel'
                controlType: 'ColorWheelItem'
                paraType: 'double'
                defaultValue: '0.6'
                value: '0'
                factorFunc:  ['c:255.0']
            },
            Parameter {
                name: qsTr('Shadows (Lift)')
                property: 'lift_g'
                objectName: 'liftwheel'
                controlType: 'ColorWheelItem'
                paraType: 'double'
                defaultValue: '0.2'
                value: '0'
                factorFunc:  ['c:255.0']
            },
            Parameter {
                name: qsTr('Shadows (Lift)')
                property: 'lift_b'
                objectName: 'liftwheel'
                controlType: 'ColorWheelItem'
                paraType: 'double'
                defaultValue: '0.2'
                value: '0'
                factorFunc:  ['c:255.0']
            },
            Parameter {
                name: qsTr('Midtones (Gamma)')
                property: 'gamma_r'
                objectName: 'gammawheel'
                controlType: 'ColorWheelItem'
                paraType: 'double'
                defaultValue: '0.6'
                value: '0'
                factorFunc:  ['c:127.5']
            },
            Parameter {
                name: qsTr('Midtones (Gamma)')
                property: 'gamma_g'
                objectName: 'gammawheel'
                controlType: 'ColorWheelItem'
                paraType: 'double'
                defaultValue: '0.2'
                value: '0'
                factorFunc:  ['c:127.5']
            },
            Parameter {
                name: qsTr('Midtones (Gamma)')
                property: 'gamma_b'
                objectName: 'gammawheel'
                controlType: 'ColorWheelItem'
                paraType: 'double'
                defaultValue: '0.2'
                value: '0'
                factorFunc:  ['c:127.5']
            },
            Parameter {
                name: qsTr('Highlights (Gain)')
                property: 'gain_r'
                objectName: 'gainwheel'
                controlType: 'ColorWheelItem'
                paraType: 'double'
                defaultValue: '0.6'
                value: '0'
                factorFunc:  ['c:63.75']
            },
            Parameter {
                name: qsTr('Highlights (Gain)')
                property: 'gain_g'
                objectName: 'gainwheel'
                controlType: 'ColorWheelItem'
                paraType: 'double'
                defaultValue: '0.6'
                value: '0'
                factorFunc:  ['c:63.75']
            },
            Parameter {
                name: qsTr('Highlights (Gain)')
                property: 'gain_b'
                objectName: 'gainwheel'
                controlType: 'ColorWheelItem'
                paraType: 'double'
                defaultValue: '0.2'
                value: '0'
                factorFunc:  ['c:63.75']
            }
        ]
    }
    objectName: 'colorGrading'
}
