import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Blur")
    mlt_service: "boxblur"
    qml: "ui_boxblur.qml"
    gpuAlt: "movit.blur"
}
