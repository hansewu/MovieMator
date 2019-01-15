import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Brightness")
    mlt_service: "brightness"
    qml: "ui.qml"
    isFavorite: true
    gpuAlt: "movit.opacity"
    allowMultiple: false
    keyframes {
            allowAnimateIn: true
            allowAnimateOut: true
            simpleProperties: ['level']
            parameters: [
                Parameter {
                    name: qsTr('Level')
                    property: 'level'
                    isSimple: true
                    isCurve: true
                    minimum: 0
                    maximum: 2
                }
            ]
        }

}
