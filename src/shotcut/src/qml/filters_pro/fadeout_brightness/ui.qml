
import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Item {
   width: 300
    height: 250
    objectName: 'fadeOut'
    property alias duration: timeSpinner.value

    Component.onCompleted: {
        if (filter.isNew) {
            duration = Math.ceil(settings.videoOutDuration * profile.fps)
            var out = filter.producerOut
            var inFrame = out - duration + 1
            filter.set('level', '0=1; %1=0'.arg(duration - 1))
            filter.set('alpha', 1)
            filter.set('in', inFrame)
            filter.set('out', out)
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
                onValueChanged: {
                    var inFrame = filter.getDouble('out') - duration + 1
                    filter.set('in', inFrame)
                    if (filter.get('alpha') != 1)
                        filter.set('alpha', '0=1; %1=0'.arg(duration - 1))
                    else
                        filter.set('level', '0=1; %1=0'.arg(duration - 1))
                }
                onSetDefaultClicked: {
                    duration = Math.ceil(settings.videoOutDuration * profile.fps)
                }
                onSaveDefaultClicked: {
                    settings.videoOutDuration = duration / profile.fps
                }
            }
        }
        CheckBox {
            id: alphaCheckbox
//            text: qsTr('Adjust opacity instead of fade with black')
            onClicked: {
                if (checked) {
                    filter.set('level', 1)
                    filter.set('alpha', '0=1; %1=0'.arg(duration - 1))
                } else {
                    filter.set('level', '0=1; %1=0'.arg(duration - 1))
                    filter.set('alpha', 1)
                }
            }

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
