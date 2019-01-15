import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Sharpen")
    mlt_service: "frei0r.sharpness"
    qml: "ui_frei0r.qml"
    gpuAlt: "movit.sharpen"
    needsProVersion: true
}
