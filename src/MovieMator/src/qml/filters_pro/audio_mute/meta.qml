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
    objectName: 'muteVolume'
    isAudio: true
    name: qsTr("Mute")
    mlt_service: "volume"
    qml: "ui.qml"
    isFavorite: true
    keyframes {
       allowAnimateIn: true
       allowAnimateOut: true
       simpleProperties: ['gain']
       parameters: [
           Parameter {
               name: qsTr('gain')
               property: 'gain'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '0'
               value: '0'
               factorFunc:  []
           }
       ]
   }
}
