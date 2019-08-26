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
    objectName: 'fadeOutVolume'
    name: qsTr("Fade Out Audio")
    mlt_service: "volume"
    qml: "ui.qml"
    isFavorite: true
    allowMultiple: false
    freeVersion: true
    keyframes {
        allowTrim: false
        allowAnimateIn: true
        simpleProperties: ['start']
        parameters: [
           Parameter {
               name: qsTr('*')
               property: 'gain'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '1.0'
               value: '0'
               factorFunc:  ['c:100.0']
           },
           Parameter {
               name: qsTr('*')
               property: 'end'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '0'
               value: '0'
               factorFunc:  []
           }
           ,
           Parameter {
               name: qsTr('*')
               property: 'in'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '-23'
               value: '0'
               factorFunc:  ['c:100.0']
           },
           Parameter {
               name: qsTr('*')
               property: 'out'
               objectName: '*'
               controlType: 'SliderSpinner'
               paraType: 'double'
               defaultValue: '772'
               value: '0'
               factorFunc:  ['c:100.0']
           }
        ]
    }
}
