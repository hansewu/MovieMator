/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
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
import QtQuick.Window 2.1
import QtQml 2.2
import QtQuick.Layouts 1.1
import com.moviemator.qml 1.0 as MovieMator

Item {
    property alias text: toolTipText.text
    property bool show: false
    onShowChanged: if (show) toolTipWindow.beginDisplay()
    anchors.fill: parent


    Window {
        id: toolTipWindow
        width: 350
        height: 100
        visible: false
        color: '#353535'

        flags: Qt.FramelessWindowHint
        
        function beginDisplay() {
            var mainWinRect = mainwindow.geometry
            toolTipWindow.x = mainWinRect.x + mainWinRect.width/2.0 - toolTipWindow.width/2.0
            toolTipWindow.y = mainWinRect.y + mainWinRect.height/2.0 - toolTipWindow.height/2.0

            toolTipWindow.opacity = 0.5 

            toolTipWindow.show()
            
            tipTimer.restart() 
            showAnim.running = true
        }

 
        SequentialAnimation {
            id: showAnim
            loops: 1
            
            NumberAnimation {
                target: toolTipWindow
                property: "opacity"
                from: 0.5
                to: 2.0
                duration: 1500
            }

            

            NumberAnimation {
                target: toolTipWindow
                property: "opacity"
                from: 1.0
                to: 0.5
                duration: 500
            }
        }
        Timer {
            id: tipTimer
            interval: 2000
            onTriggered: toolTipWindow.close()
        }

        Rectangle {
            id: toolTipRect
            anchors.centerIn: parent
            color: '#353535'
            Text {
                id: toolTipText
                anchors.centerIn: parent
                color: '#ffffff'
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }


        
    }
}
