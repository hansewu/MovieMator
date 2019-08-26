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
    objectName: 'dynamicText'
    name: qsTr('Text')
    mlt_service: 'dynamictext'
    qml: "ui.qml"
    vui: 'vui.qml'
    isGpuCompatible: false
    filterType: qsTr('2 Text')
    freeVersion: true
     keyframes {
         allowAnimateIn: true
         allowAnimateOut: true
//         simpleProperties: ['geometry', 'fgcolour', 'olcolour', 'outline', 'bgcolour', 'pad', 'letter_spaceing', 'trans_fix_rotate_x', 'trans_scale_x', 'trans_ox', 'trans_oy']
         simpleProperties: ['fgcolour', 'olcolour', 'outline', 'bgcolour', 'pad', 'letter_spaceing', 'trans_fix_rotate_x', 'trans_scale_x', 'trans_ox', 'trans_oy']

         parameters: [
        /*     Parameter {
                 name: qsTr('Position / Size')
                 property: 'geometry'
                 isSimple: true
                 paraType: "rect"
             }, 
             Parameter {
                 name: qsTr('Text Color')
                 property: 'fgcolour'
                 isSimple: true
                 paraType: "rect"
             },
             Parameter {
                 name: qsTr('Outline Color')
                 property: 'olcolour'
                 isSimple: true
                 paraType: "rect"
             },
             Parameter {
                 name: qsTr('Outline')
                 property: 'outline'
                 isSimple: true
                 paraType: "int"
             },
             Parameter {
                 name: qsTr('Background Color')
                 property: 'bgcolour'
                 isSimple: true
                 paraType: "rect"
             },
             Parameter {
                 name: qsTr('Pad')
                 property: 'pad'
                 isSimple: true
                 paraType: "int"
             },
             Parameter {
                 name: qsTr('Letter Spaceing')
                 property: 'letter_spaceing'
                 isSimple: true
                 paraType: "int"
             },*/

             Parameter {
                 name: qsTr('transition fix_rotate_x')
                 property: 'trans_fix_rotate_x'
                 isSimple: true
                 paraType: "double"
             },
             Parameter {
                 name: qsTr('transition scale_x')
                 property: 'trans_scale_x'
                 isSimple: true
                 paraType: "double"
             },
             Parameter {
                 name: qsTr('transition scale_aspect_ratio')
                 property: 'trans_scale_aspect_ratio'
                 isSimple: true
                 paraType: "double"
             },

             Parameter {
                 name: qsTr('transition ox')
                 property: 'trans_ox'
                 isSimple: true
                 paraType: "double"
             },
             Parameter {
                 name: qsTr('transition oy')
                 property: 'trans_oy'
                 isSimple: true
                 paraType: "double"
             },
             Parameter {
                 name: qsTr('size')
                 property: 'size'
                paraType: '*'
                defaultValue: '140.976'
                value: ''
                factorFunc:  []
             }
         ]
     }
}
