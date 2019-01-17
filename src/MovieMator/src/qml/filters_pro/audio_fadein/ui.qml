

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    objectName: 'fadeIn'
    property alias duration: timeSpinner.value

    Component.onCompleted: {
        if (filter.isNew) {
            duration = Math.ceil(settings.audioInDuration * profile.fps)
            filter.set('gain', 0)
            filter.set('end', 1)
            filter.set('in', filter.producerIn)
            filter.set('out', filter.getDouble('in') + duration - 1)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8

        RowLayout {
            Label {
                text: qsTr('Duration')
                color: '#ffffff'
            }
            TimeSpinner {
                id: timeSpinner
                minimumValue: 1
                maximumValue: 5000
                value: filter.getDouble('out') - filter.getDouble('in') + 1
                onValueChanged: {
                    filter.set('out', filter.getDouble('in') + value - 1)
                }
                onSetDefaultClicked: {
                    duration = Math.ceil(settings.audioInDuration * profile.fps)
                }
                onSaveDefaultClicked: {
                    settings.audioInDuration = duration / profile.fps
                }
            }
        }
        Item {
            Layout.fillHeight: true;
        }
    }
}
