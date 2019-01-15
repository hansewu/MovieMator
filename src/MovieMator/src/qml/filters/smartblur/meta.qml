
import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Reduce Noise")
    mlt_service: 'avfilter.smartblur'
    qml: 'ui.qml'
    needsProVersion: true
}
