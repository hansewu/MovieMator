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
    objectName: "Frei0rColoradjWidget"
    name: qsTr("Color Grading")
    mlt_service: "frei0r.coloradj_RGB"
    qml: "ui_frei0r_coloradj.qml"
    isHidden: true
    filterType: qsTr('5 Advanced Coloring Tool')
}
