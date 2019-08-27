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
    name: qsTr("Brightness")
    mlt_service: "brightness"
    qml: "ui.qml"
    isFavorite: true
    gpuAlt: "movit.opacity"
    filterType: qsTr('5 Advanced Coloring Tool')
    objectName: 'brightness'
}
