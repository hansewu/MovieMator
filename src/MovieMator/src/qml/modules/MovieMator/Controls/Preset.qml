
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1

RowLayout {
    property var parameters: []

    // Tell the parent QML page to update its controls.
    signal presetSelected()

    Component.onCompleted: {
        filter.loadPresets()
    }

    ComboBox {
        id: presetCombo
        Layout.fillWidth: true
        Layout.minimumWidth: 100
        Layout.maximumWidth: 200
        model: filter.presets
        onCurrentTextChanged: {
            filter.preset(currentText)
            presetSelected()
        }
    }
/*    Button {
        id: saveButton
        iconName: 'list-add'
        iconSource: 'qrc:///icons/light/32x32/list-add.png'
        tooltip: qsTr('Save')
        implicitWidth: 20
        implicitHeight: 20
        onClicked: nameDialog.show()
    }
    Button {
        id: deleteButton
        iconName: 'list-remove'
        iconSource: 'qrc:///icons/light/32x32/list-remove.png'
        tooltip: qsTr('Delete')
        implicitWidth: 20
        implicitHeight: 20
        onClicked: confirmDialog.show()
    }*/
    AddButton {

        tooltip: qsTr('Save')
        onClicked: nameDialog.show()
    }

    DeleteButton {

        tooltip: qsTr('Delete')
        onClicked: confirmDialog.show()
    }

    SystemPalette { id: dialogPalette; colorGroup: SystemPalette.Active }
    Window {
        id: nameDialog
        flags: Qt.Dialog
        color: dialogPalette.window
        modality: Qt.ApplicationModal
        title: qsTr('Save Preset')
        width: 200
        height: 90

        function acceptName() {
            presetCombo.currentIndex = filter.savePreset(parameters, nameField.text)
            nameDialog.close()
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10

            Label {
                text: qsTr('Name:')
            }
            TextField {
                id: nameField
                focus: true
                Layout.fillWidth: true
                onAccepted: nameDialog.acceptName()
                Keys.onPressed: {
                    if (event.key == Qt.Key_Escape) {
                        nameDialog.close()
                        event.accepted = true
                    }
                }
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                Layout.alignment: Qt.AlignRight
                Button {
                    text: qsTr('OK')
                    isDefault: true
                    onClicked: nameDialog.acceptName()
                }
                Button {
                    text: qsTr('Cancel')
                    onClicked: nameDialog.close()
                }
            }
        }
    }

    Window {
        id: confirmDialog
        flags: Qt.Dialog
        color: dialogPalette.window
        modality: Qt.ApplicationModal
        title: qsTr('Delete Preset')
        width: 300
        height: 90

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10

            Label {
                text: qsTr('Are you sure you want to delete %1?').arg(presetCombo.currentText)
                wrapMode: Text.Wrap
            }

            RowLayout {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                Layout.alignment: Qt.AlignRight
                Button {
                    text: qsTr('OK')
                    isDefault: true
                    onClicked: {
                        if (presetCombo.currentText !== ' ') {
                            filter.deletePreset(presetCombo.currentText)
                            presetCombo.currentIndex = 0
                        }
                        confirmDialog.close()
                    }
                }
                Button {
                    text: qsTr('Cancel')
                    onClicked: confirmDialog.close()
                }
            }
        }
    }
}
