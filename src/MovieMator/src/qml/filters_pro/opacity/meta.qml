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
    objectName: 'brightnessOpacity'
    name: qsTr("Opacity")
    mlt_service: "brightness"
    qml: "ui.qml"
    gpuAlt: "movit.opacity"
    filterType: qsTr('1 Basic Processing')

}
