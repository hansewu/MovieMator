/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1

GridLayout {
    property var parameters: []
    property ListModel presets: ListModel {}
    property int selectedIndex: 0

    // Tell the parent QML page to update its controls.
    signal beforePresetLoaded()
    signal presetSelected()

    columns: 2
    anchors.margins: 8

    Component.onCompleted: {
        filter.loadPresets()
    }

    GridMenu {
        id: gridMenu
        width: 320
        height: 320
        Layout.columnSpan: 2
        listModel: presets
        onSelected: {
            beforePresetLoaded()
            filter.preset(presets.get(index).name)
            selectedIndex = index
            presetSelected()
        }
    }

    AddButton {
        id: addButton
        tooltip: qsTr('Save')
        onClicked: nameDialog.show()
    }

    DeleteButton {
        id: deleteButton
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
            filter.savePreset(parameters, nameField.text)
            deleteButton.enabled = true
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
                    if (event.key === Qt.Key_Escape) {
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
                text: qsTr('Are you sure you want to delete %1?').arg(presets.get(selectedIndex).name)
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
                        filter.deletePreset(presets.get(selectedIndex).name)
                        if (presets.count > 0) {
                            gridMenu.selectedOfIndex(0)
                        } else {
                            deleteButton.enabled = false
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

    Connections {
        target: filter
        onPresetsChanged: {
            presets.clear()

            var presetsCount = filter.getPresetsCount()
            for (var i = 0; i < presetsCount; i++) {
                var preset = filter.getPresetOfIndex(i)
                presets.append({"name": preset, "portrait": "qrc:///icons/filters/text/slide-in-from-left.gif"})
            }
        }
    }
}
