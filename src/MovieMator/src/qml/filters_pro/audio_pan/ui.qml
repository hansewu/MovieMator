/*
 * Copyright (c) 2013-2015 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
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
import MovieMator.Controls 1.0

Item {
    width: 300
    height: 250
    Component.onCompleted: {
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        Label {
            text: qsTr('Channel')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
//        ComboBox {
//            id: combo
//            Layout.columnSpan: 2
//            model: [qsTr('Left'), qsTr('Right')]
//            onCurrentIndexChanged: filter.set('channel', currentIndex)
//        }

        MyComboBox {
            id: combo
            Layout.columnSpan: 2
            listModel: ListModel {
                ListElement {name: qsTr('Left')}
                ListElement {name: qsTr('Right')}
            }
            onCurrentIndexChanged: filter.set('channel', currentIndex)
        }

        Label {
            text: qsTr('Left')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: slider
            minimumValue: 0
            maximumValue: 1000
            label: qsTr('Right')
            ratio: maximumValue
            decimals: 2
            value: filter.getDouble('start') * maximumValue
            onValueChanged: filter.set('start', value / maximumValue)
        }
        UndoButton {
            onClicked: slider.value = 0
        }

        Item {
            Layout.fillHeight: true;
        }
    }
}
