import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    objectName: 'brightnessOpacity'
    name: qsTr("Opacity")
    mlt_service: "brightness"
    qml: "ui.qml"
    gpuAlt: "movit.opacity"
}
