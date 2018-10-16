import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Stabilize")
    mlt_service: "vidstab"
    qml: "ui.qml"
    isClipOnly: true
    needsProVersion: true
}
