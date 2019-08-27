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
    name: qsTr("Low Pass")
    mlt_service: 'ladspa.1891'
    qml: 'ui.qml'
    objectName: 'audioLowPass'
}
