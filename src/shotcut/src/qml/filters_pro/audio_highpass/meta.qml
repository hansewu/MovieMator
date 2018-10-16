import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    isAudio: true
    name: qsTr("High Pass")
    mlt_service: 'ladspa.1890'
    qml: 'ui.qml'
}
