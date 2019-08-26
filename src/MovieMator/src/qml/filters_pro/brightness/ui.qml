/*
 * Copyright (c) 2013-2016 Meltytech, LLC
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
    width: 300
    height: 250

    property double levelValue: 100

    function testSetAnimationValueAndSetValue()
    {
        //⾮动画数据设置时，会清空这个 property 的之前所有动画和非动画数据。
        //获取非动画数据，直接获取之前设置的非动画数据，不论中间有没有设置过动画和非动画 都可以获取到。
        //获取动画数据，如果当前有动画数据存储，就获取动画数据，如果没有就获取非动画数据 。
        filter.set("level", 0.55)

        var double_level = filter.getDouble("level")    //0.55
        var anim_double_level = filter.getAnimDoubleValue(10, "level") //0.55
        console.log("test level get ", double_level)
        console.log("test level get anim", anim_double_level)

        //动画数据设置时，不会清空property之前设置的非动画数据，只会在当前动画数据基础上 插入以及新增加设置动画数据。
        //获取非动画数据，直接获取之前设置的非动画数据，不论中间有没有设置过动画和非动画 都可以获取到。
        //获取动画数据，如果当前有动画数据存储，就获取动画数据，如果没有就获取非动画数据 。
        //set 之后，amin_set 动画，不会改变非动画的数据，获取动画和非动画的数据信息都存在。
        filter.cache_setKeyFrameParaValue(10, "level", 0.88)
        filter.syncCacheToProject()

        double_level = filter.getDouble("level")  //0.55
        anim_double_level = filter.getAnimDoubleValue(10, "level") //0.88
        console.log("test2 level get ", double_level)
        console.log("test2 level get anim", anim_double_level)

        //⾮动画数据设置时，会清空这个 property 的之前所有动画和⾮动画数据。
        //anim_set 动画，set 时，动画信息丢失。获取动画和非动画的数据最后得到的结果都是set 的结果(非动画的数据)。
        //获取⾮动画数据，直接获取之前设置的⾮动画数据，不论中间有没有设置过动画和非动画 都可以获取到。
        //获取动画数据，如果当前有动画数据存储，就获取动画数据，如果没有就获取非动画数据 。
        filter.set("level", 0.22)

        double_level = filter.getDouble("level")   //0.22
        anim_double_level = filter.getAnimDoubleValue(10, "level")  //0.22
        console.log("test3 level get ", double_level)
        console.log("test3 level get anim", anim_double_level)

    }

    Component.onCompleted: {
        keyFrame.initFilter(layoutRoot)
        //testSetAnimationValueAndSetValue()
    }
    GridLayout {
        id: layoutRoot
        columns: 3
        anchors.fill: parent
        anchors.margins: 20

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

        Label {
            text: qsTr('Brightness') + "    "
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            objectName: 'brightnessSlider'
            id: brightnessSlider
            minimumValue: 0.0
            maximumValue: 200.0
            decimals: 1
            spinnerWidth: 80
            suffix: ' %'
            onValueChanged: {
                keyFrame.controlValueChanged(brightnessSlider)
            }
        }
        UndoButton {
            onClicked: {
                brightnessSlider.value = 100
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
