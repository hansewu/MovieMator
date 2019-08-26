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
    objectName: "frei0r_colgate"
    mlt_service: "frei0r.colgate"
    qml: "ui.qml"
    isFavorite: true
    gpuAlt: "movit.white_balance"
    filterType: qsTr('3 Basic Coloring Tool')
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['0','1']
        parameters: [
            Parameter {
                name: qsTr('0')
                property: '0'
                isSimple: true
                isCurve: true
                objectName: 'whiteColorPicker'
                controlType: 'ColorPicker'
                paraType: 'string'
                defaultValue: '#7f7f7f'
                value: '0'
                factorFunc:  []
            },
            Parameter {
                name: qsTr('1')
                property: '1'
                isSimple: true
                isCurve: true
                objectName: 'whiteSlider'
                controlType: 'SliderSpinner'
                paraType: 'double'
                defaultValue: '0.433333333333'
                value: '0'
                factorFunc:  []
            }
        ]
    }
}
