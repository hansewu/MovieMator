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
    name: qsTr("Swap Channels")
    mlt_service: "channelcopy"
    objectName: 'audioSwapChannels'
    qml: "ui.qml"
    keyframes {
       allowAnimateIn: true
       allowAnimateOut: true
       simpleProperties: ['start']
       parameters: [
           Parameter {
               name: qsTr('*')
               property: 'swap'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '1'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('*')
               property: 'from'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '0'
               value: '0'
               factorFunc:  []
           },
           Parameter {
               name: qsTr('*')
               property: 'top'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '1'
               value: '0'
               factorFunc:  []
           }
       ]
   }
}
