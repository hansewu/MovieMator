import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Mirror")
    mlt_service: "movit.mirror"
    needsGPU: true
    qml: "ui.qml"
}
