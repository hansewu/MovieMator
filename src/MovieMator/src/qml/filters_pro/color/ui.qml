/*
 * Copyright (c) 2014-2016 Meltytech, LLC
 * Author: Brian Matherly <code@brianmatherly.com>
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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property var defaultParameters: ['lift_r', 'lift_g', 'lift_b', 'gamma_r', 'gamma_g', 'gamma_b', 'gain_r', 'gain_g', 'gain_b']
    property double gammaFactor: 2.0
    property double gainFactor: 4.0
    width: 300
    height: 250

    function loadWheels() {


        var keyFrameCount = filter.getKeyFrameCountOnProject("lift_r");
        if(keyFrameCount>0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "lift_r");
                var liftRkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "lift_r");
                var liftGkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "lift_g");
                var liftBkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "lift_b");

                var gammaRkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "gamma_r");
                var gammaGkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "gamma_g");
                var gammaBkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "gamma_b");

                var gainRkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "gain_r");
                var gainGkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "gain_g");
                var gainBkeyValue = filter.getStringKeyValueOnProjectOnIndex(index, "gain_b");

                filter.cache_setKeyFrameParaValue(nFrame, "lift_r", liftRkeyValue.toString())
                filter.cache_setKeyFrameParaValue(nFrame, "lift_g", liftGkeyValue.toString())
                filter.cache_setKeyFrameParaValue(nFrame, "lift_b", liftBkeyValue.toString())

                filter.cache_setKeyFrameParaValue(nFrame, "gamma_r", gammaRkeyValue.toString())
                filter.cache_setKeyFrameParaValue(nFrame, "gamma_g", gammaGkeyValue.toString())
                filter.cache_setKeyFrameParaValue(nFrame, "gamma_b", gammaBkeyValue.toString())

                filter.cache_setKeyFrameParaValue(nFrame, "gain_r", gainRkeyValue.toString())
                filter.cache_setKeyFrameParaValue(nFrame, "gain_g", gainGkeyValue.toString())
                filter.cache_setKeyFrameParaValue(nFrame, "gain_b", gainBkeyValue.toString())

            }

            filter.syncCacheToProject();

            liftwheel.color = Qt.rgba( filter.getKeyFrameOnProjectOnIndex(0, "lift_r"),
                                   filter.getKeyFrameOnProjectOnIndex(0, "lift_g"),
                                   filter.getKeyFrameOnProjectOnIndex(0, "lift_b"),
                                   1.0 )
            gammawheel.color = Qt.rgba( filter.getKeyValueOnProjectOnIndex(0, "gamma_r") / gammaFactor,
                                    filter.getKeyValueOnProjectOnIndex(0, "gamma_g") / gammaFactor,
                                    filter.getKeyValueOnProjectOnIndex(0, "gamma_b") / gammaFactor,
                                    1.0 )
            gainwheel.color = Qt.rgba( filter.getKeyValueOnProjectOnIndex(0, "gain_r") / gainFactor,
                                   filter.getKeyValueOnProjectOnIndex(0, "gain_g") / gainFactor,
                                   filter.getKeyValueOnProjectOnIndex(0, "gain_b") / gainFactor,
                                   1.0 )
        }
        else
        {
            var liftColor = Qt.rgba( filter.getDouble("lift_r"),
                                       filter.getDouble("lift_g"),
                                       filter.getDouble("lift_b"),
                                       1.0 )
            var gammaColor = Qt.rgba( filter.getDouble("gamma_r") / gammaFactor,
                                        filter.getDouble("gamma_g") / gammaFactor,
                                        filter.getDouble("gamma_b") / gammaFactor,
                                        1.0 )
           var gainColor = Qt.rgba( filter.getDouble("gain_r") / gainFactor,
                                       filter.getDouble("gain_g") / gainFactor,
                                       filter.getDouble("gain_b") / gainFactor,
                                       1.0 )
           liftwheel.color = liftColor;
           gammawheel.color = gammaColor;
           gainwheel.color = gainColor;
        }
   }
    
    Component.onCompleted: {
        keyFrame.initFilter(layoutRoot)
    }

    GridLayout {
        id: layoutRoot
        columns: 6
        anchors.fill: parent
        anchors.margins: 20
        rowSpacing:15

        YFKeyFrame{
            id: keyFrame
            Layout.columnSpan:3
            onSyncUIDataToProject:{
                keyFrame.syncDataToProject(layoutRoot)
            }
            onRefreshUI:{
                keyFrame.updateParamsUI(layoutRoot)
            }
        }
        // Row 1
        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignLeft
            color: '#ffffff'
        }
        Preset {
            Layout.columnSpan: 5
            parameters: defaultParameters
            onPresetSelected: {
                loadWheels()
            }
        }

        SeparatorLine {
            Layout.fillWidth: true 
            Layout.columnSpan: 6
        }

        // Row 2
        Label {
            text: qsTr('Shadows')
            color: '#ffffff'

        }
        UndoButton {
            Layout.alignment: Qt.AlignCenter
            onClicked: liftwheel.color = Qt.rgba( 0.0, 0.0, 0.0, 1.0 )
        }
        Label {
            text: qsTr('Midtones')
            color: '#ffffff'

        }
        UndoButton {
            Layout.alignment: Qt.AlignCenter
            onClicked: gammawheel.color = Qt.rgba( 1.0 / gammaFactor, 1.0 / gammaFactor, 1.0 / gammaFactor, 1.0 )
        }
        Label {
            text: qsTr('Highlights')
            color: '#ffffff'
        }
        UndoButton {
            Layout.alignment: Qt.AlignCenter
            onClicked: gainwheel.color = Qt.rgba( 1.0 / gainFactor, 1.0 / gainFactor, 1.0 / gainFactor, 1.0 )
        }
        
        // Row 3
        ColorWheelItem {
            objectName: 'liftwheel'
            id: liftwheel
            Layout.columnSpan: 2
            implicitWidth: parent.width / 3 - parent.columnSpacing
            implicitHeight: implicitWidth / 1.1
            Layout.alignment : Qt.AlignLeft | Qt.AlignTop
            Layout.minimumHeight: 75
            Layout.maximumHeight: 300
            onColorChanged: {
                keyFrame.controlValueChanged(liftwheel)
            }
        }
        ColorWheelItem {
            objectName: 'gammawheel'
            id: gammawheel
            Layout.columnSpan: 2
            implicitWidth: parent.width / 3 - parent.columnSpacing
            implicitHeight: implicitWidth / 1.1
            Layout.alignment : Qt.AlignLeft | Qt.AlignTop
            Layout.minimumHeight: 75
            Layout.maximumHeight: 300
            onColorChanged: {
                keyFrame.controlValueChanged(gammawheel)
            }
        }
        ColorWheelItem {
            objectName: 'gainwheel'
            id: gainwheel
            Layout.columnSpan: 2
            implicitWidth: parent.width / 3 - parent.columnSpacing
            implicitHeight: implicitWidth / 1.1
            Layout.alignment : Qt.AlignLeft | Qt.AlignTop
            Layout.minimumHeight: 75
            Layout.maximumHeight: 300
            onColorChanged: {
                keyFrame.controlValueChanged(gainwheel)
            }
        }


        Item { Layout.fillHeight: true }
    }
}
