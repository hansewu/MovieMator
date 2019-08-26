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
    name: qsTr("Band Pass")
    mlt_service: 'ladspa.1892'
    qml: 'ui.qml'
    keyframes {
       allowAnimateIn: true
       allowAnimateOut: true
       simpleProperties: ['start']
       parameters: [
           Parameter {
               name: qsTr('Center frequency')
               property: '0'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '322'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('Bandwidth')
               property: '1'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '322'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('Rolloff rate')
               property: '2'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '1'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('Dry rate')
               property: 'wetness'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '1.0'
               value: '0'
               factorFunc:  ['c:100.0']
           }
       ]
   }
    objectName: 'audioBandPass'
}
