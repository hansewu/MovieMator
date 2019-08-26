/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl1987527 <wyl1987527@163.com>
 * Author: Author: fuyunhuaxin <2446010587@qq.com>
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
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import MovieMator.Controls 1.0
import QtQml 2.2
import 'translateTool.js' as Trans

Rectangle {
    id: root
    color: "transparent"
    property int repeaterItemWidth: 114
    property int repeaterItemHeight: 96
    property int currentChoosed : 0
    property bool translate2CH: ("zh" === settings.language || "zh_CN"===settings.language)/*("zh_CN" === Qt.locale().name)*/
    // property bool translate2CH:false
    
    function findFilterModel(name){
        for(var i=0;i<filtersInfoList.count;i++){
            if(name === filtersInfoList.get(i).name){
                return filtersInfoList.get(i)
            }
        }
        return null
    }

    function findFilterType(type){
        for(var i=0;i<catListAll.count;i++){
            if(type === catListAll.get(i).typename){
                return catListAll.get(i)
            }
        }
        return null
    }

    function addFilter(index){
        if(filtersResDock == null){
            throw new Error("filtersResDock is undefined")
        }

        filtersResDock.addFilterItem(index)
    }

    function previewFilter(perviewSettingFilePath){
        if(filtersResDock == null){
            throw new Error("filtersResDock is undefined")
        }
        filtersResDock.previewFilter(perviewSettingFilePath)
    }

    function updateData()
    {
        if(filtersInfo == null){
            return;
        }
        var num = filtersInfo.rowCount();
        for(var i=0;i< filtersInfo.rowCount();i++){
            var filterInfo = {
                id: filtersInfo.get(i).name + '' + i,
                index : i,
                visible:filtersInfo.get(i).visible,
                name : filtersInfo.get(i).name,
                filterType : filtersInfo.get(i).filterType,
                imageSourcePath : filtersInfo.get(i).imageSourcePath,
                perviewSettingFilePath:filtersInfo.get(i).perviewSettingFilePath
            }
            filtersInfoList.append(filterInfo)
        }

        catList.clear()
        catListAll.clear()
        catListAll.append({"typename":qsTr('All')})
        for(var j=0;j<filtersInfoList.count;j++){
            if('' === filtersInfoList.get(j).filterType) continue;
            if(null === findFilterType(filtersInfoList.get(j).filterType)){
                catList.append({"typename":filtersInfoList.get(j).filterType})
                catListAll.append({"typename":filtersInfoList.get(j).filterType})
            }
        }
    }

    Component.onCompleted: {
        updateData()
    }

    ListModel{
        id:catList
        ListElement{
            typename:'A'
        }
    }
    ListModel{
        id:catListAll
        ListElement{
            typename:qsTr('All')
        }
    }

     ListModel{
        id:filtersInfoList
    }

    Component{
        id:delegate
        Rectangle{
            id:delegateRoot
            width: scrollView.width-20
            height: Math.ceil(filters.count / parseInt(width / repeaterItemWidth)) * repeaterItemHeight + filterhead.height + 10
            anchors{
                left:parent.left
                leftMargin:20
            }
            color: 'transparent'
            function refreshFilters(){
                filters.clear()
                for(var i=0;i<filtersInfoList.count;i++){
                    if(typename === filtersInfoList.get(i).filterType){
                        if(filtersInfoList.get(i).visible === false){
                            continue;
                        }
                        filters.append(filtersInfoList.get(i))
                    }
                }
            }
            Component.onCompleted: {
                refreshFilters()
                if(filters.count > 0){
                    currentChoosed = filters.get(0).index
                }
            }
            Rectangle{
                id:filterhead
                width: parent.width
                height: 28
                color: 'transparent'
                Text {
                    width: contentWidth
                    height: parent.height
                    id: catName
                    text: (typeof typename == 'undefined')? '':typename
                    color: '#ffffff'
                    font.pixelSize: 13
                    z:2
                    anchors{
                        left: parent.left
                        leftMargin: -10
                    }
                }
                Image {
                    id: line
                    source: 'qrc:///icons/light/32x32/line.png'
                    height:3
                    z:1
                    anchors{
                        left: catName.right
                        leftMargin:5
                        bottom: catName.bottom
                        bottomMargin:18
                    }
                }
            }
            Flow {
                id:itemFlow
                width: parent.width
                anchors{
                    top:filterhead.bottom
                }
                Repeater{
                    id:rep
                    model: ListModel{
                        id:filters
                    }

                    Rectangle{
                        width: repeaterItemWidth//130
                        height: repeaterItemHeight//96
                        color: 'transparent'

                        Button { 
                            id: button
                            width:27
                            height:27
                            z:2
                            anchors{
                                top:parent.top
                                topMargin:1
                                right:parent.right
                                rightMargin:21
                            }
                            // visible:id.checked ? true : false
                            visible:(id.hoverStat || hovered) ? true : false
                            checkable : true
                            onClicked:{
                                addFilter(index)
                            }
                            style: ButtonStyle { 
                                background:Rectangle{ 
                                    implicitHeight: parent.height 
                                    implicitWidth: parent.width 
                                    color: "transparent" 
                                    Image{ 
                                        anchors.fill: parent
                                        source: (control.hovered && control.pressed) ? 'qrc:///icons/light/32x32/filter_add-a.png' : 'qrc:///icons/light/32x32/filter_add.png';
                                    } 
                                } 
                            } 
                        }

                        Rectangle{
                            id:id
                            objectName:index
                            width: 94
                            height: 64
                            z:1
                            radius: 3 
                            color: (hoverStat || button.hovered) ?'#C0482C':'transparent'
                            property bool checked: (objectName === currentChoosed)?true:false
                            property bool hoverStat:false
                            Image {
                                id: myIcon
                                anchors.horizontalCenter : parent.horizontalCenter
                                width: 90
                                height: 60
                                source: imageSourcePath
                                anchors {
                                    top: parent.top
                                    topMargin: 2
                                }
                            }
                        }
                        Text {
                            height: 20
                            anchors {
                                top: id.bottom
                                topMargin: 3
                                left: id.left
                                leftMargin: 5
                                horizontalCenter: parent.horizontalLeft
                            }
                            text: translate2CH?Trans.transEn2Ch(name):Trans.transEn2Short(name)
                            color: '#ffffff'
                            font.pixelSize: 12
                        }
                        MouseArea {
                            id:mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            preventStealing:true
                            property int clickNum:0
                            onClicked: {
                                clickNum ++
                                if(clickNum == 1) { 
                                    clickTimer.start() 
                                } 
                                if(clickNum == 2) { 
                                    clickNum = 0 
                                    clickTimer.stop() 
                                    currentChoosed = id.objectName
                                    addFilter(index) 
                                }
                            }
                            onEntered:{
                                id.hoverStat = true
                            }
                            onExited:{
                                if(!containsMouse){
                                    id.hoverStat = false
                                }
                            }
                            Timer{ 
                                id: clickTimer 
                                //超过300ms还没有触发第二次点击证明时单击（并不一定非要用300ms延时，不过300ms是经典延时时间） 
                                interval: 300;
                                onTriggered: { 
                                    mouseArea.clickNum = 0 
                                    clickTimer.stop() 
                                    currentChoosed = id.objectName
                                    previewFilter(perviewSettingFilePath)
                                } 
                            }
                        }
                    }

                }
            }
        }
    }

    Rectangle{ //头部
        id: filterHead
        width: parent.width
        height: 30
        z:3
        color: '#000000'
        Label{
            id: tabName
            width: 50
            height: parent.height
            anchors{
//                horizontalCenter : parent.horizontalCenter
                verticalCenter : parent.verticalCenter
                top:parent.top
                topMargin: 7
                left: parent.left
                leftMargin: 11
            }
            text: qsTr('Filters')
            font.pixelSize: 12
            color: '#ffffff'
        }

        FilterComboBox {
            id:catCombox
            width: 150
            anchors{
                bottom: parent.bottom
                bottomMargin: 6
                right: parent.right
                rightMargin: 13
            }
            Component.onCompleted: {
                catCombox.setModel(catListAll)
                if(translate2CH)
                {
                    width = 85
                }
            }
            onCatChanged:{
                if(index <= 0){
                    catList.clear()
                    for(var i=1;i<catListAll.count;i++){
                        catList.append(catListAll.get(i))
                    }
                }else{
                    var chosed = catListAll.get(index)
                    catList.clear()
                    catList.append(chosed)
                }
            }
        }
    }

    ScrollView{
        id: scrollView
        width: parent.width
        height: parent.height - filterHead.height - 10
        anchors{
            top : filterHead.bottom
            topMargin: 10
        }
        style: ScrollViewStyle {
            transientScrollBars: false
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
            frame: Item {
                implicitWidth: 14
                implicitHeight: 14
                Rectangle {
                    color: "transparent"
                    anchors.fill: parent
                }
            }
            scrollBarBackground: Item {
                Rectangle {
                    color: "#323232"
                    anchors.fill: parent
                }
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

        }
//        ListView{
//            width:parent.width;
//            height:parent.height
//            model:catList
//            delegate:delegate
//            focus:true
//        }

        /*
         * 使用 Repeater代替 ListView
         * 一次性加载完所有 model
         * 使 view得到准确的高度
         */
        Column {
            Repeater{
                model:catList
                delegate:delegate
            }
        }
    }
}
