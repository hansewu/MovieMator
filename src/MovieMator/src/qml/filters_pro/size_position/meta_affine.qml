/*
 * Copyright (c) 2011-2016 Meltytech, LLC
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
 * Author: fuyunhuaxin <2446010587@qq.com>
 *
 */

import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    objectName: 'affineSizePosition'
    name: qsTr('Size and Position')
    mlt_service: 'size_position'
    qml: 'ui_affine.qml'
    vui: 'vui_affine.qml'
    gpuAlt: 'movit.rect'
    isFavorite: true
    allowMultiple: false
    filterType: qsTr('1 Basic Processing')
    freeVersion: true
}
