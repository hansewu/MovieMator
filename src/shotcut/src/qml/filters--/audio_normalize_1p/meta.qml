import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    isAudio: true
    name: qsTr("Normalize: One Pass")
    mlt_service: "dynamic_loudness"
    qml: "ui.qml"
}
