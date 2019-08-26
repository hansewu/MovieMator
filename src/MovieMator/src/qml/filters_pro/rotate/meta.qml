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
    objectName: "affineRotate"
    name: qsTr("Rotate")
    mlt_service: "affine"
    qml: "ui.qml"
    isFavorite: true
//    allowMultiple: false
    filterType: qsTr('1 Basic Processing')
    freeVersion: true
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['transition.fix_rotate_x', 'transition.scale_x','transition.ox','transition.oy']
        minimumVersion: '3'
        parameters: [
            Parameter {
                name: qsTr('Rotation')
                property: 'transition.fix_rotate_x'
                objectName: 'rotationSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('Scale')
                property: 'transition.scale_x'
                objectName: 'scaleSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '1'
                value: '0'
                factorFunc:  ['b:100.0']
            },
            Parameter {
                name: qsTr('Scale')
                property: 'transition.scale_y'
                objectName: 'scaleSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '1'
                value: '0'
                factorFunc:  ['b:100.0']
            },
            Parameter {
                name: qsTr('X offset')
                property: 'transition.ox'
                objectName: 'xOffsetSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0'
                value: '0'
                factorFunc:  ['x:-1.0']
            },
            Parameter {
                name: qsTr('Y offset')
                property: 'transition.oy'
                objectName: 'yOffsetSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0'
                value: '0'
                factorFunc:  ['x:-1.0']
            }
        ]
    }
}
