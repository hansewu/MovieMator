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
    name: qsTr("Reverb")
    mlt_service: 'ladspa.1216'
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
               defaultValue: '50'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('Bandwidth')
               property: '1'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '1.5'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('Rolloff rate')
               property: '2'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '0.1'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('Dry rate')
               property: '3'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '0.75'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('Dry rate')
               property: '4'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '-1.5'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('Dry rate')
               property: '5'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '-10.0'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('Dry rate')
               property: '6'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '-20'
               value: '0'
               factorFunc:  []
           }
       ]
   }
    objectName: 'audioReverb'
}
