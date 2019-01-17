import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    isAudio: true
    name: qsTr("Pan")
    mlt_service: 'panner'
    objectName: 'audioPan'
    qml: 'ui.qml'
}
