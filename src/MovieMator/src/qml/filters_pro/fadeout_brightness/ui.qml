/*
 * Copyright (c) 2014-2016 Meltytech, LLC
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
    property int blockUpdate: 0

    Component.onCompleted: {
        if (filter.isNew) {
            duration = Math.ceil(settings.videoOutDuration * profile.fps)
            var out = filter.producerOut
            var inFrame = out - duration + 1
            filter.set('level', '0=1; %1=0'.arg(duration - 1))
            filter.set('alpha', 1)
            filter.setInAndOut(inFrame, out)
        }
    }

    function isInited(){
        if(blockUpdate < 2){
            blockUpdate += 1
            return false
        }
        return true
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
                    if(isInited()){
                        var inFrame = filter.getDouble('out') - duration + 1
                        filter.set('in', inFrame)
                        if (filter.get('alpha') != 1)
                            filter.set('alpha', '0=1; %1=0'.arg(duration - 1))
                        else
                            filter.set('level', '0=1; %1=0'.arg(duration - 1))
                    }
                    
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
            checked:false
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
