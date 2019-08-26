/*
 * Copyright (c) 2011-2016 Meltytech, LLC
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * 
 */

import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    isAudio: true
    name: qsTr("Normalize: One Pass")
    mlt_service: "dynamic_loudness"
    qml: "ui.qml"
    keyframes {
       allowAnimateIn: true
       allowAnimateOut: true
       simpleProperties: ['start']
       parameters: [
           Parameter {
               name: qsTr('*')
               property: 'target_loudness'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '-23.0'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('*')
               property: 'window'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '10.0'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('*')
               property: 'max_gain'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '15.0'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('*')
               property: 'min_gain'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '-15.0'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('*')
               property: 'max_rate'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '3.0'
               value: '0'
               factorFunc:  []
           }
       ]
   }
    objectName: 'audioNormalizeOnePass'
}
