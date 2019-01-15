import QtQuick 2.0
import com.moviemator.qml 1.0

Metadata {
    type: Metadata.Filter
    isAudio: true
    name: qsTr("Swap Channels")
    mlt_service: "channelcopy"
    objectName: 'audioSwapChannels'
    qml: "ui.qml"
}
