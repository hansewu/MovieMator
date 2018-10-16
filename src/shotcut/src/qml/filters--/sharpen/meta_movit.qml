import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Sharpen")
    mlt_service: "movit.sharpen"
    needsGPU: true
    qml: "ui_movit.qml"
}
