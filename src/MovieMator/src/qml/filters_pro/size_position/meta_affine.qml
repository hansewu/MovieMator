/*
 * Copyright (c) 2011-2016 Meltytech, LLC
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
 * Author: fuyunhuaxin <2446010587@qq.com>
 *
 */

import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    objectName: 'affineSizePosition'
    name: qsTr('Size and Position')
    mlt_service: 'affine'
    qml: 'ui_affine.qml'
    vui: 'vui_affine.qml'
    gpuAlt: 'movit.rect'
    isFavorite: true
    allowMultiple: false
    filterType: qsTr('1 Basic Processing')
    freeVersion: true
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['transition.rect_anim_relative']
        parameters: [
            Parameter {
                name: qsTr('Position / Size')
                property: 'transition.rect_anim_relative'
                paraType:'rect'
                value: 'X0.0Y0.0W1.0H1.0'
                defaultValue:'0.0x0.0+1.0+1.0'
            }
            ,
            Parameter {
                name: qsTr('transition.fill')
                property: 'transition.fill'
                objectName: '*'
                controlType: ''
                paraType: 'double'
                defaultValue: '1.0'
                value: ''
                factorFunc:  []
            }
            ,
            Parameter {
                name: qsTr('transition.distort')
                property: 'transition.distort'
                objectName: '*'
                controlType: ''
                paraType: 'double'
                defaultValue: '0.0'
                value: ''
                factorFunc:  []
            }
        ]
    }
}
