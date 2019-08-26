import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("*")
    mlt_service: "frei0r.*"
    qml: "ui.qml"
    filterType: qsTr('C Other')
    keyframes {
            allowAnimateIn: true
            allowAnimateOut: true
            simpleProperties: []
            parameters: [
                Parameter {
                    name: qsTr('*')
                    property: '*'
                    objectName: '*'
                    controlType: '*'
                    paraType: '*'
                    defaultValue: '*'
                    value: '*'
                    factorFunc:  []
                }
            ]
        }

}
