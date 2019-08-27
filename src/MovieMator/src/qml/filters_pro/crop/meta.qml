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
    name: qsTr("Crop")
    mlt_service: "crop"
    qml: "ui.qml"
    gpuAlt: "crop"
    objectName: 'crop'
    isClipOnly: true
    freeVersion: true
    filterType: qsTr('1 Basic Processing')
}
