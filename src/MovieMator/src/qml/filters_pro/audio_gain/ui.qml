/*
 * Copyright (c) 2013-2015 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
 * Author: fuyunhuaxin <2446010587@qq.com>
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
    property string gainParameter: 'gain'
    Component.onCompleted: {
        if (filter.isNew) {
        }
    }

    function toDb(value) {
        return 20 * Math.log(value) / Math.LN10
    }

    function fromDb(value) {
        return Math.pow(10, value / 20);
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8

        RowLayout {
            Label {
                text: qsTr('Gain')
                color: '#ffffff'
            }
            SliderSpinner {
                id: slider
                minimumValue: -50
                maximumValue: 24
                suffix: 'dB'
                decimals: 1
                spinnerWidth: 80
                value: toDb(filter.getDouble(gainParameter))
                onValueChanged: filter.set(gainParameter, fromDb(value))
            }
            UndoButton {
                onClicked: slider.value = 0
            }
        }
        Item {
            Layout.fillHeight: true;
        }
    }
}
