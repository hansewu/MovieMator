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
    name: qsTr("Balance")
    mlt_service: 'panner'
    objectName: 'audioBalance'
    qml: 'ui.qml'
   keyframes {
       allowAnimateIn: true
       allowAnimateOut: true
       simpleProperties: ['start']
       parameters: [
           Parameter {
               name: qsTr('start')
               property: 'start'
               objectName: 'slider'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '0.5'
               value: '0'
               factorFunc:  ['c:1000.0']
           }
       ]
   }
}
