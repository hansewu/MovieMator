import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    isAudio: true
    name: qsTr("Low Pass")
    mlt_service: 'ladspa.1891'
    qml: 'ui.qml'
}
