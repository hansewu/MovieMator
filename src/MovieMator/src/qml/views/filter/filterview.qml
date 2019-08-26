/*
 * Copyright (c) 2014-2016 Meltytech, LLC
 * Author: Brian Matherly <code@brianmatherly.com>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
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
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4
//import Qt.labs.controls 1.0

//主界面右侧属性界面中的滤镜dock界面
Rectangle {
    id: root
    
    //当选中应用已添加列表中的第attachedIndex个滤镜时发出此信号
    signal currentFilterRequested(int attachedIndex)
    
    //清除当前滤镜的参数显示界面
    function clearCurrentFilter(index) {
        if((attachedfiltersmodel.isVisible(index) === attachedFilters.isvideo)||(index<0)){
            if (filterConfig.item) {
                filterConfig.item.width = 1
                filterConfig.item.height = 1
            }
            filterConfig.source = ""
        }
    }
    
    //设置当前选中滤镜为列表中的第index个滤镜，并显示对应参数界面
    function setCurrentFilter(index) {
        if(attachedfiltersmodel.isVisible(index) === attachedFilters.isvideo){
            attachedFilters.setCurrentFilter(index)
            filterConfig.source = metadata ? metadata.qmlFilePath : ""
        }
    }

    //重新加载已选择滤镜
    function reChooseFilter(){
        attachedFilters.reLoadFilter()
    }

    color: '#353535'//activePalette.window
    width: 400

    onWidthChanged: _setLayout()
    onHeightChanged: _setLayout()
    
    function _setLayout() {
        // if (height > width - 200) {
        //     root.state = "portrait"
        // } else {
        //     root.state = "landscape"
        // }
    }
    
    SystemPalette { id: activePalette }
    
    //添加滤镜菜单
    FilterMenu {
        id: filterMenu

        height: 400
        onFilterSelected: {
            if((typeof attachedfiltersmodel == 'undefined')||(attachedfiltersmodel == null)){
                throw new Error("attachedfiltersmodel is undefined")
            }
            if((typeof metadatamodel == 'undefined')||(metadatamodel == null)){
                throw new Error("metadatamodel is undefined")
            }
            attachedfiltersmodel.add(metadatamodel.get(index))
        }
    }

    Component.onCompleted: {
        if(isVideo)
            attachedFilters.isvideo = true
        else
            attachedFilters.isvideo = false
    }

    //已添加到producer上的滤镜列表界面
    GridLayout {
        id: attachedContainer
        columns: 3
        // width: titleBackground.width
        height: 72
        anchors {
            top: parent.top
            topMargin: 2
            left: parent.left
            leftMargin: 2
            right:parent.right
            rightMargin: 2
        }
        Rectangle{
            color: 'transparent'
            anchors.fill: parent
            AttachedFilters {
                id: attachedFilters
                color:'#2e2e2e'
                Layout.columnSpan: 3
                height:parent.height
                isvideo:isVideo
                anchors {
                    top: parent.top
                    topMargin: 4
                    left: parent.left
                    leftMargin: 3
                    right:parent.right
                    rightMargin: 3
                }
                onFilterClicked: {
                    root.currentFilterRequested(index)
                }
                onRemoveLastFilter:{
                    clearCurrentFilter(-1)
                }
                ScrollView{
                    visible: !attachedfiltersmodel.isProducerSelected
                    anchors.fill: parent
                    horizontalScrollBarPolicy:Qt.ScrollBarAlwaysOn
                    style: ScrollViewStyle {
                        transientScrollBars: false
                        minimumHandleLength:90
                        handle: Item {
                            implicitWidth: 14
                            implicitHeight: 14
                            Rectangle {
                                color: "#787878"
                                anchors.fill: parent
                                anchors.margins: 3
                                radius: 4
                            }
                        }
                        scrollBarBackground: Item {
                            implicitWidth: 14
                            implicitHeight: 14
                        }
                        decrementControl: Rectangle {
                                    implicitWidth: 0
                                    implicitHeight: 0
                        }
                        incrementControl: Rectangle {
                                    implicitWidth: 0
                                    implicitHeight: 0
                        }
                        corner: Item {
                            implicitWidth: 14
                            implicitHeight: 14
                        }
                    }
                    Rectangle{
                        id:icon
                        width:57
                        height:parent.height
                        anchors {
                            left: parent.left
                            leftMargin: 4
                        }
                        Image{
                            id:emptyImg
                            anchors.horizontalCenter : parent.horizontalCenter
                            width: 57
                            height: 30
                            source: 'qrc:///icons/light/32x32/icon.jpg'
                            anchors {
                                top: parent.top
                                topMargin: 4
                            }
                        }
                        Text {
                            height: 20
                            anchors {
                                top: emptyImg.bottom
                                topMargin: 4
                                left: emptyImg.left
                                leftMargin: 0
                            }
                            text: qsTr("Nothing selected")
                            color: '#ffffff'
                            font.pixelSize: 12
                        }
                    }
                }
            }
        }
    }

    //动态滤镜参数界面
    ScrollView {
        id: filterConfigScrollView
        anchors {
            top: attachedContainer.bottom
            bottom: keyFrameControlContainer.top
            bottomMargin:10
            left: root.left
            right: root.right
        }
        style: ScrollViewStyle {
                transientScrollBars: false
              //  scrollToClickedPosition:true
                handle: Item {
                    implicitWidth: 14
                    implicitHeight: 14
                    Rectangle {
                        color: "#787878"
                        anchors.fill: parent
                        anchors.margins: 3
                        radius: 4
                    }
                }
                scrollBarBackground: Item {
                    implicitWidth: 14
                    implicitHeight: 14
                }
                decrementControl: Rectangle {
                            implicitWidth: 0
                            implicitHeight: 0
                }
                incrementControl: Rectangle {
                            implicitWidth: 0
                            implicitHeight: 0
                }
                corner: Item {
                    implicitWidth: 14
                    implicitHeight: 14
                }
            }

        //根据动态滤镜参数调节参数界面宽
        function expandWidth() {

            if (filterConfig.item) {
                filterConfig.item.width =
                    Math.max(filterConfig.minimumWidth,
                             filterConfigScrollView.width /* scroll bar */)
            }
        }
        onWidthChanged: expandWidth()
        //动态加载滤镜参数界面qml的加载器
        Loader {
            id: filterConfig
            property int minimumWidth: 0
            onLoaded: {
                minimumWidth = item.width
                filterConfigScrollView.expandWidth()
            }
        }
    }

    //关键帧界面
    Rectangle{
        id:keyFrameControlContainer
        color: '#353535'
        border.color: "black"
        border.width: 0
        height: 15
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            leftMargin:0
            rightMargin:10
            bottomMargin:5
        }
        radius: 2//圆角
        visible: metadata && (metadata.keyframes.parameterCount > 0)
        KeyFrameControl {
            id: keyFrameControl
            width:parent.width
            height:parent.height
            anchors {
                horizontalCenter: parent.horizontalCenter;
                bottom: parent.bottom
            }
            onSwitchFoldStat: {
                if(bChecked){
                    keyFrameControlContainer.height = 90
                }else{
                    keyFrameControlContainer.height = 15
                }
            }
        }
    }
}
