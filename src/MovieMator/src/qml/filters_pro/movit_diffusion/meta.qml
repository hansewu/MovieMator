/*
 * Copyright (c) 2011-2016 Meltytech, LLC
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
 */

import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Diffusion")
    mlt_service: "movit.diffusion"
    needsGPU: true
    qml: "ui.qml"
    filterType: qsTr('9 Effect')
    objectName: 'diffusion'
}
