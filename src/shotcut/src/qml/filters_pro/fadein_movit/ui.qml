

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Item {
    width: 300
    height: 250
    objectName: 'fadeIn'
    property alias duration: timeSpinner.value

    Component.onCompleted: {
        if (filter.isNew) {
            duration = Math.ceil(settings.videoInDuration * profile.fps)
            filter.set('opacity', '0~=0; %1=1'.arg(duration - 1))
            filter.set('alpha', 1)
            filter.set('in', filter.producerIn)
            filter.set('out', filter.getDouble('in') + duration - 1)
        }
        alphaCheckbox.checked = filter.get('alpha') != 1
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
                minimumValue: 2
                maximumValue: 5000
                value: filter.getDouble('out') - filter.getDouble('in') + 1
                onValueChanged: filter.set('out', filter.getDouble('in') + value - 1)
                onSetDefaultClicked: {
                    duration = Math.ceil(settings.videoInDuration * profile.fps)
                }
                onSaveDefaultClicked: {
                    settings.videoInDuration = duration / profile.fps
                }
            }
        }
        CheckBox {
            id: alphaCheckbox
        //    text:

            // When =-1, alpha follows opacity value.
            onClicked: filter.set('alpha', checked? -1 : 1)

            style: CheckBoxStyle {
                        label: Text {
                            color: "white"
                            text: qsTr('Adjust opacity instead of fade with black')
                        }
            }


        }
        Item {
            Layout.fillHeight: true;
        }
    }
}
