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
    name: qsTr("Gain / Volume")
    mlt_service: "volume"
    qml: "ui.qml"
    isFavorite: true
    freeVersion: true
   keyframes {
       allowAnimateIn: true
       allowAnimateOut: true
       simpleProperties: ['gain']
       parameters: [
           Parameter {
               name: qsTr('gain')
               property: 'gain'
               objectName: 'slider'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '1.0'
               value: '0'
               factorFunc:  ['c:20','pow:10']
           }
       ]
   }
    objectName: 'audioGainVolume'
}
