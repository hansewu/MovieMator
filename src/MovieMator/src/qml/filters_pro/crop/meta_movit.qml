import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Crop")
    mlt_service: "crop"
    needsGPU: true
    qml: "ui.qml"
    allowMultiple: false
}
