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
    name: qsTr("Wave")
    objectName: "wave"
    mlt_service: "wave"
    qml: "ui.qml"
    isFavorite: false
//    allowMultiple: false
    filterType: qsTr('7 Distort')
}
