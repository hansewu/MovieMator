
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import MovieMator.Controls 1.0
import "frei0rInterface.js" as Handdle

Item {
    id:root
    width: 350
    height: 250

    property rect color
    property double levelValue: 100
    property var keyFrame
    function findControl(objectName,root){
        var controlList = root.children
        for(var i=0;i<controlList.length;i++){
            if(objectName === controlList[i].objectName){
                return controlList[i]
            }
        }
        return null;
    }
    function getParamsAssociatedWithControl(id){
        var rt = [];
        for(var i=0;i<metadata.keyframes.parameters.length;i++){
            if(id.objectName === metadata.keyframes.parameters[i].objectName)
            rt.push(i)
        }
        return  rt;
    }
    // 复选框响应函数
    function checkBoxClicked(objectName){
        var ctr = keyFrame.findControl(objectName,layoutRoot)
        keyFrame.controlValueChanged(ctr)
    }
    // 滑条响应函数
    function sliderValueChanged(objectName){
        var ctr = keyFrame.findControl(objectName,layoutRoot)
        keyFrame.controlValueChanged(ctr)
    }
    // 滑条还原函数
    function sliderUndoClicked(objectName){
        var ctr = keyFrame.findControl(objectName,layoutRoot)
        var paramIndexList = getParamsAssociatedWithControl(ctr)
        ctr.value = parseFloat(Handdle.metaParamList[paramIndexList[0]].defaultValue) * 100
    }
    // 颜色响应函数
    function colorPickerChanged(objectName){
        var ctr = findControl(objectName,layoutRoot)
        keyFrame.controlValueChanged(ctr)
    }

    function colorUndoClicked(objectName){
        var ctr = keyFrame.findControl(objectName,layoutRoot)
        var paramIndexList = getParamsAssociatedWithControl(ctr)
        var rgb = Handdle.metaParamList[paramIndexList[0]].defaultValue.split(" ")
        ctr.value = Qt.rgba(parseFloat(rgb[0]),parseFloat(rgb[1]),parseFloat(rgb[2]),1.0)
    }
    // 位置响应函数
    function positionChanged(objectName){
        var ctr = keyFrame.findControl(objectName,layoutRoot)
        var paramIndexList = keyFrame.getParamsAssociatedWithControl(ctr)
        if(paramIndexList.length !== 2){
            console.log("getParamsAssociatedWithControl Error: "+paramIndexList.length)
            return;
        }
        filter.set(filter.set(Handdle.metaParamList[paramIndexList[0]].property,(ctr.children)[0].text))
        filter.set(filter.set(Handdle.metaParamList[paramIndexList[1]].property,(ctr.children)[2].text))
    }

    function positionUndoClicked(objectName){
        var ctr = keyFrame.findControl(objectName,layoutRoot)
        (ctr.children)[0].value = String(0)
        (ctr.children)[2].value = String(0)
    }
    // 文本响应函数
    function stringValueChanged(objectName){
        var ctr = findControl(objectName,layoutRoot)
        keyFrame.controlValueChanged(ctr)
    }

    function strinCtrUndoClicked(objectName){
        var ctr = findControl(objectName,layoutRoot)
        ctr.currentIndex = 0
    }
    // 关键帧操作——同步数据响应
    function keyFrameSynchroData(){
        keyFrame.syncDataToProject(layoutRoot)
    }
    // 关键帧操作——时间线改变之后加载数据
    function keyFrameLoad(){
        keyFrame.updateParamsUI(layoutRoot)
    }
    // 初始化所有控件的值并将默认值写入滤镜中
    function initFilter(){
        filter.setInAndOut(0, timeline.getCurrentClipParentLength()-1)
        keyFrame.initFilter(layoutRoot)
    }
    
    Component.onCompleted: {
        // 根据参数计算参数控件容器的高度
        var paramNum = metadata.keyframes.parameters.length
        if(paramNum > 0){
            var h = paramNum * 26 + 50
            root.height = (h > root.height)? h : root.height
        }
        
        Handdle.createKeyFrame()
        keyFrame = findControl("keyFrame",layoutRoot)
        Handdle.init(filter.isNew)
        initFilter()
        
    }

    GridLayout{
        id:layoutRoot
        anchors.fill: parent
        columns: 3
        anchors.margins: 8
    }
}
