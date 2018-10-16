import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Vignette")
    mlt_service: "vignette"
    qml: "ui_oldfilm.qml"
    gpuAlt: "movit.vignette"
    needsProVersion: true
}
