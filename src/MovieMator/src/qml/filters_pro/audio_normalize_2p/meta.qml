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
    name: qsTr("Normalize: Two Pass")
    mlt_service: "loudness"
    qml: "ui.qml"
    isClipOnly: true
   keyframes {
       allowAnimateIn: true
       allowAnimateOut: true
       simpleProperties: ['program']
       parameters: [
           Parameter {
               name: qsTr('program')
               property: 'program'
               objectName: 'programSlider'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '-23'
               value: '0'
               factorFunc:  []
           }
       ]
   }
    objectName: 'audioNormalizeTwoPass'
}
