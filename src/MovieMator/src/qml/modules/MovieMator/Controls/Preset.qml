/*
 * Copyright (c) 2013-2015 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
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

RowLayout {
    property var parameters: []
    property ListModel listMode: ListModel {}
    property string m_strType: ""

    // Tell the parent QML page to update its controls.
    signal beforePresetLoaded()
    signal presetSelected()

    Component.onCompleted: {
        filter.loadPresets()
        var count = filter.presetCount()
        for (var i = 0; i < count; i++) {
            var presetStr = filter.getPresetWithIndex(i)
                var presetLowCaseName = (presetStr.toString()).toLowerCase()
                console.log("Preset presetStr: ", presetStr)
                var strType = "." + m_strType
                strType.toLowerCase()

                if(presetLowCaseName === " " ||  (m_strType === "") || presetLowCaseName.indexOf(strType) > 0)
                    listMode.append({name: presetStr})
        }
    }

//    ComboBox {
//        id: presetCombo
//        Layout.fillWidth: true
//        Layout.minimumWidth: 100
//        Layout.maximumWidth: 264
//        model: filter.presets
//        onCurrentTextChanged: {
//            if (currentText.length > 1) {//一个空格
//                beforePresetLoaded()
//                filter.preset(currentText)
//                presetSelected()
//            }
//        }
//    }

    MyComboBox {
        id: presetCombo
        Layout.fillWidth: true
        Layout.minimumWidth: 100
        Layout.maximumWidth: 264
        listModel: listMode
        onCurrentTextChanged: {
            if (currentText.length > 1) {//一个空格
                beforePresetLoaded()
                filter.preset(currentText)
                presetSelected()
            }
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
        width: 300
        height: 90

        function acceptName() {
            var fileName = nameField.text
            if(m_strType != "")
                fileName = fileName + "." + m_strType
           // presetCombo.currentIndex = 
	        filter.savePreset(parameters, fileName)
            nameDialog.close()

            var nameUsed = false
            for(var index =0; index < listMode.count; index++)
            {
                if(listMode.get(index).name  === fileName)
                {
                    nameUsed = true
                }
            }

            if(nameUsed == false)
                listMode.append({name: fileName})

            presetCombo.currentText = fileName
            
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
                            
                            listMode.remove(presetCombo.currentIndex)
                           //presetCombo.currentIndex = 0
                            presetCombo.currentText  = ' '
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
