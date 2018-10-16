import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    isAudio: true
    objectName: 'fadeInVolume'
    name: qsTr("Fade In Audio")
    mlt_service: "volume"
    qml: "ui.qml"
    isFavorite: true
    allowMultiple: false
}
