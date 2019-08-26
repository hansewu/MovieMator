/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author:
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
import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.0

RowLayout{

    id: keyFrame
    visible: false
    
    /** the animation’s enable state to the value specified by bEnable. 
     * 更新关键帧功能是否开启bEnable，同时向QmlFilter对象中保存了当前动画功能是否激活。当动画激活时，可以增加关键帧设置动画，当动画没有激活时，不可以添加关键帧，不能给滤镜添加动画效果。
     *
     * \param bEnable the animation is enable or not
     * \return the animation's enable state
     */
    function updateEnableKeyFrame(bEnable)
    {
        bEnableKeyFrame = bEnable
        if(filter != null){
            filter.setEnableAnimation(bEnableKeyFrame)
        }
        return bEnableKeyFrame
    }

    /** update the Auto add keyframes function’s enable state to the value specified by bEnable.
     * 更新自动设置关键帧功能为bEnable，同时向QmlFilter对象中保存了当前自动更新关键帧的功能是否激活
     *
     * \param bEnable enables the Auto add keyframes function or not
     * \return the Auto add keyframes function’s enable state
     */
    function updateAutoSetAsKeyFrame(bEnable)
    {
        bAutoSetAsKeyFrame = bEnable
        if(filter != null){
            filter.setAutoAddKeyFrame(bAutoSetAsKeyFrame)
        }
        return bAutoSetAsKeyFrame
    }

    // A Boolean value that indicates whether the animation is enable state.
    property bool bEnableKeyFrame: updateEnableKeyFrame((filter.cache_getKeyFrameNumber() > 0))
    
    // A Boolean value that indicates whether the auto add keyframes function is on.
    property bool bAutoSetAsKeyFrame: updateAutoSetAsKeyFrame(true)
    
    // The current frame number. 当前的帧数
    property double currentFrame: 0

    // A Boolean value that indicates whether the frame is the key frame. 当前帧是否是关键帧
    property bool bKeyFrame: false

    // 屏蔽信号，主要用到当一个filter 有几个参数时，当一个参数set时，可能会出现加载更新界面，或获取所有参数，还未来得及设置的参数出现了获取新的值，改变了参数。filter的几个参数set的过程中，屏蔽信号，不调用loadframe，等加载完了，再处理。
    property bool bBlockUpdateUI: false
    property bool bBlockUIChangedSignal: false

    //存rect 变量，应该可以不用作为成员变量，可以在使用的地方使用局部变量
    property rect colorRect

    //同步数据信号，这里这个信号的响应主要是同步界面数值到project中，mlt底层
    signal syncUIDataToProject()

    //加载keyframe信号，这里主要用到的是同步数据值到界面中
    signal refreshUI()

    // 目前只有string 类型的参数用到
    function findDefaultIndex(strValue,modelList){
        if(modelList.length <= 0){
            return 0
        }
        for(var i=0;i<modelList.length;i++){

            if(strValue === modelList[i]){
                return i
            }
        }
        return 0
    }

    // 滤镜初始化，正常是滤镜参数界面加载完调用，更新滤镜参数界面的初始化显示
    function initFilter(layoutRoot){
        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }

        //if(metadata.keyframes.parameters.length === 1)
            filter.setInAndOut(0, timeline.getCurrentClipParentLength()-1)

        //导入上次工程保存的关键帧
        //bBlockUpdateUI = true
        var metaParamList = metadata.keyframes.parameters
        if((typeof metaParamList == 'undefined')||(metaParamList.length <= 0)) return

        /*
        // 由于string没有关键帧，所以通过string类型的参数获取关键帧个数会出错，因此不能用string类型的参数去获取关键帧个数和关键帧位置
        var keyParam = 0
        for(keyParam=0;keyParam<metaParamList.length;keyParam++){
            if(metaParamList[keyParam].paraType !== 'string'){
                break;
            }
        }

        var keyFrameCount = 0
        if(keyParam < metaParamList.length){
            keyFrameCount = filter.getKeyFrameCountOnProject(metaParamList[keyParam].property);
            if(keyFrameCount < 0){
                keyFrameCount = filter.getKeyFrameCountOnProject(metaParamList[keyParam].property);
            }
            
            bBlockUpdateUI = true
            for(var keyIndex=0; keyIndex<keyFrameCount;keyIndex++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(keyIndex, metaParamList[keyParam].property);
                for(var paramIndex=0;paramIndex<metaParamList.length;paramIndex++){
                    var prop = metaParamList[paramIndex].property
                    var keyValue = '';
                    if(metaParamList[paramIndex].paraType === 'rect'){
                        keyValue = filter.getAnimRectValue(nFrame, prop);
                        filter.cache_setKeyFrameParaRectValue(nFrame, prop, keyValue)
                    }else{
                        keyValue = filter.getKeyValueOnProjectOnIndex(keyIndex, prop);
                        filter.cache_setKeyFrameParaValue(nFrame, prop, keyValue.toString())
                    }
                }
            }
            bBlockUpdateUI = false
            filter.syncCacheToProject();
            if(keyFrameCount > 0){
                refreshUI()
            }
        }
        */

        // var keyFrameCount = filter.cache_getKeyFrameNumber()
        // // 初始化关键帧控件
        // if (filter.isNew && keyFrameCount<=0){
        //     for(var paramIndex=0;paramIndex<metaParamList.length;paramIndex++){
        //         var parameter = metaParamList[paramIndex]
        //         parameter.value = parameter.defaultValue
        //         var control = findControl(parameter.objectName,layoutRoot)
        //         if(control === null)
        //             continue;
        //         switch(parameter.controlType)
        //         {
        //         case "SliderSpinner":
        //             control.value = parseFloat(parameter.defaultValue) * 100
        //             break;

        //         case "CheckBox":
        //             control.checked = Math.round(parseFloat(parameter.defaultValue))
        //             break;

        //         case "ColorWheelItem":
        //             var parameter2 = metaParamList[paramIndex+1]
        //             var parameter3 = metaParamList[paramIndex+2]
        //             control.color = Qt.rgba(calcProjValByUIVal(parseFloat(parameter.defaultValue) * 100,parameter.factorFunc), calcProjValByUIVal(parseFloat(parameter2.defaultValue) * 100,parameter2.factorFunc), calcProjValByUIVal(parseFloat(parameter3.defaultValue) * 100,parameter3.factorFunc), 1.0 )
        //             filter.set(parameter.property,calcProjValByUIVal(parseFloat(parameter.defaultValue) * 100,parameter.factorFunc))
        //             filter.set(parameter2.property,calcProjValByUIVal(parseFloat(parameter2.defaultValue) * 100,parameter2.factorFunc))
        //             filter.set(parameter3.property,calcProjValByUIVal(parseFloat(parameter3.defaultValue) * 100,parameter3.factorFunc))
        //             paramIndex = paramIndex+2
                
        //             break;
                
        //         case "ColorPicker":
        //             var rgb = parameter.defaultValue.split(" ")
        //             control.value = Qt.rgba(parseFloat(rgb[0]),parseFloat(rgb[1]),parseFloat(rgb[2]),1.0)
        //             // filter.set(parameter.property,control.value)
        //             break;

        //         case "Slider":
        //             control.value = parseFloat(parameter.defaultValue)
        //             break;

        //         case "StringCtr":
        //             var index0 = findDefaultIndex(parameter.defaultValue,control.model)
        //             control.currentIndex = index0
        //             break
                
        //         default :
        //             break;
        //         }

        //     }
        // }

        refreshUI()
    }

    // 控件发生修改时响应的函数，更新filter的某个参数值，主要调用filter.setKeyFrame 和 filter.set
    function controlValueChanged(id){
        if(typeof id == 'undefined'){
            throw new Error("id is undefined:"+id)
        }
        var userChange = false
        var valueChange = false

        if(bBlockUIChangedSignal == true) return
       
        bBlockUpdateUI = true
        // 可能一个控件对应几个配置项
        var parameterList = getParamsAssociatedWithControl(id)
        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        for(var paramIndex=0;paramIndex<parameterList.length;paramIndex++){
            if(metadata.keyframes.parameters.length < parameterList[paramIndex]){
                throw new Error("metadata length abnormal:"+metadata.keyframes.parameters.length+" "+parameterList[paramIndex]);
            }
            var parameter = metadata.keyframes.parameters[parameterList[paramIndex]]
            switch(parameter.controlType)
            {
            case "SliderSpinner":
                if(filter.cache_bKeyFrame(currentFrame))
                {
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter.property, calcProjValByUIVal(id.value,parameter.factorFunc).toString())
                    filter.syncCacheToProject()
                //如果这次的改变是程序往里面写值，则不做处理，下同
                }else if((Math.abs((id.value - parameter.value) / (id.maximumValue - id.minimumValue)) < 0.01)||(Math.abs(id.value - parameter.value) < 1)){
                    
                }else if((parameter.value > id.value)&&(id.value === id.maximumValue)){
                    valueChange = true

                }else if(!bEnableKeyFrame)  //没有关键帧
                {
                    filter.set(parameter.property, calcProjValByUIVal(id.value,parameter.factorFunc))
                    //userChange = true

                }
                else if(bAutoSetAsKeyFrame)  
                {
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter.property, calcProjValByUIVal(id.value,parameter.factorFunc).toString())
                    filter.syncCacheToProject()
                    userChange = true
                }
                break;

            case "CheckBox":
                if(filter.cache_bKeyFrame(currentFrame))
                {
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter.property, Number(id.checked).toString())
                    filter.syncCacheToProject()
                }else if((id.checked === parameter.value)||(Math.abs(id.checked - parameter.value) < 1)){
                    
                }
                else if(!bEnableKeyFrame)  //没有关键帧
                {
                    filter.set(parameter.property, Number(id.checked))
                }
                else if(bAutoSetAsKeyFrame)  
                {
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter.property, Number(id.checked).toString())
                    filter.syncCacheToProject()
                    userChange = true
                }
                break;

            case "ColorWheelItem":
                var temp1 = (id.color).toString()
                var value10 = Number('0x' + temp1.substring(1,3))
                var value11 = Number('0x' + temp1.substring(3,5))
                var value12 = Number('0x' + temp1.substring(5,7))
                var temp2 = (parameter.value).toString()
                var value20 = Number('0x' + temp2.substring(1,3))
                var value21 = Number('0x' + temp2.substring(3,5))
                var value22 = Number('0x' + temp2.substring(5,7))

                
                var parameter2 = metadata.keyframes.parameters[parameterList[paramIndex+1]]
                var parameter3 = metadata.keyframes.parameters[parameterList[paramIndex+2]]

                if(filter.cache_bKeyFrame(currentFrame))
                {
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter.property, calcProjValByUIVal(id.red,parameter.factorFunc).toString())
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter2.property, calcProjValByUIVal(id.green,parameter2.factorFunc).toString())
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter3.property, calcProjValByUIVal(id.blue,parameter3.factorFunc).toString())
                    filter.syncCacheToProject()
                }else if((value10 - value20 <= 1)&&(value11 - value21 <= 1)&&(value12 - value22 <= 1)){
                    userChange = false
                }
                else if(!bEnableKeyFrame)  //没有关键帧
                {
                    filter.set(parameter.property,calcProjValByUIVal(id.red,parameter.factorFunc))
                    filter.set(parameter2.property,calcProjValByUIVal(id.green,parameter2.factorFunc))
                    filter.set(parameter3.property,calcProjValByUIVal(id.blue,parameter3.factorFunc))
                }
                else if(bAutoSetAsKeyFrame)  
                {
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter.property, calcProjValByUIVal(id.red,parameter.factorFunc).toString())
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter2.property, calcProjValByUIVal(id.green,parameter2.factorFunc).toString())
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter3.property, calcProjValByUIVal(id.blue,parameter3.factorFunc).toString())

                    filter.syncCacheToProject()

                    userChange = true
                }
                paramIndex = paramIndex+2
                break;

            case "ColorPicker":
                var rv = parseInt("0x" + id.value.slice(1, 3))
                var gv = parseInt("0x" + id.value.slice(3, 5))
                var bv = parseInt("0x" + id.value.slice(5, 7))
                colorRect.x = calcProjValByUIVal(rv,parameter.factorFunc)
                colorRect.y = calcProjValByUIVal(gv,parameter.factorFunc)
                colorRect.width = calcProjValByUIVal(bv,parameter.factorFunc)

                var rp = parseInt("0x" + parameter.value.slice(1, 3))
                var gp = parseInt("0x" + parameter.value.slice(3, 5))
                var bp = parseInt("0x" + parameter.value.slice(5, 7))
            
                if(filter.cache_bKeyFrame(currentFrame))
                {
                    filter.cache_setKeyFrameParaRectValue(currentFrame, parameter.property, colorRect)
                    filter.syncCacheToProject()
                //如果这次的改变是程序往里面写值，则不做处理，下同
                }else if((id.value === parameter.value)||(Math.abs(rv+gv+bv - rp - gp - bp) < 8)){

                }
                else if(!bEnableKeyFrame)  //没有关键帧
                {
                    filter.set(parameter.property, colorRect)
                }
                else if(bAutoSetAsKeyFrame)  
                {
                    filter.cache_setKeyFrameParaRectValue(currentFrame, parameter.property, colorRect)
                    filter.syncCacheToProject()
                    userChange = true
                }
                break;

            case "Slider":
                if(filter.cache_bKeyFrame(currentFrame))
                {
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter.property, calcProjValByUIVal(id.value,parameter.factorFunc).toString())
                    filter.syncCacheToProject()
                //如果这次的改变是程序往里面写值，则不做处理，下同
                }else if((id.value === parameter.value)||(Math.abs((id.value - parameter.value) / (id.maximumValue - id.minimumValue)) < 0.01)||(Math.abs(id.value - parameter.value) < 1)){

                }
                else if(!bEnableKeyFrame)  //没有关键帧
                {
                    filter.set(parameter.property, calcProjValByUIVal(id.value,parameter.factorFunc))
                }
                else if(bAutoSetAsKeyFrame)  
                {
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter.property, calcProjValByUIVal(id.value,parameter.factorFunc).toString())
                    filter.syncCacheToProject()
                    userChange = true
                }
                break;

            case "StringCtr":
                if(filter.cache_bKeyFrame(currentFrame))
                {
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter.property, id.currentText)
                    filter.syncCacheToProject()
                //如果这次的改变是程序往里面写值，则不做处理，下同
                }else if(id.currentText === parameter.value){

                }
                else if(!bEnableKeyFrame)  //没有关键帧
                {
                    filter.set(parameter.property, id.currentText)
                }
                else if(bAutoSetAsKeyFrame)  
                {
                    filter.cache_setKeyFrameParaValue(currentFrame, parameter.property, id.currentText)
                    filter.syncCacheToProject()
                    userChange = true
                }
                break;

            default :
                break;
            }
        }
        bBlockUpdateUI = false
        
        // 添加关键帧
        if (userChange)
        {
         //   if (!bAutoSetAsKeyFrame) 
         //   {
         //       return
         //  }

            bKeyFrame = true
            //syncUIDataToProject()
            addKeyFrame()
        }
    }

    InfoDialog { 
        id: addFrameInfoDialog
        text: qsTr('Auto set as key frame at postion')+ ": " + position + "."
        property int position: 0 
    }

    //弹出 显示自动增加关键信息的提示界面
    function showAddFrameInfo(position)
    {
        if (bAutoSetAsKeyFrame == false) return

        addFrameInfoDialog.show     = false
        addFrameInfoDialog.show     = true
        addFrameInfoDialog.position = position
    }

    // 在当前浮标尺所在位置添加为关键帧，如果添加的是第一帧，自动在首尾位置添加关键帧
    function addKeyFrame(){
        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        var position = timeline.getPositionInCurrentClip()
        if (position < 0) return
  

        bBlockUpdateUI = true

        //添加首尾关键帧
        if (filter.cache_getKeyFrameNumber() <= 0)
        {
            var paramCount = metadata.keyframes.parameterCount
            for(var i = 0; i < paramCount; i++)
            {
                var key = metadata.keyframes.parameters[i].property
                var paraType = metadata.keyframes.parameters[i].paraType
                var value = ''
                var position2 = timeline.getCurrentClipLength() - 1//filter.producerOut - filter.producerIn + 1
                if(paraType === 'rect'){
                    value = filter.getAnimRectValue(position,key)
                    filter.cache_setKeyFrameParaRectValue(0, key, value)
                    filter.cache_setKeyFrameParaRectValue(position2, key, value)
                    
                }else{
                    value = filter.get(key)
                    filter.cache_setKeyFrameParaValue(0, key, value.toString());
                    filter.cache_setKeyFrameParaValue(position2, key, value.toString());
                    
                }
            }
        }

        bBlockUpdateUI = false

        //重复点击不生效
        //var bKeyFrame = filter.cache_bKeyFrame(position)
        //if (bKeyFrame)
            //return
        bBlockUpdateUI = true
        //插入关键帧
        paramCount = metadata.keyframes.parameterCount
        for(i = 0; i < paramCount; i++)
        {
            
            key = metadata.keyframes.parameters[i].property
            paraType = metadata.keyframes.parameters[i].paraType
            var value ;
            switch(paraType){
            case 'int':
                value = filter.getAnimIntValue(position,key)
                filter.cache_setKeyFrameParaValue(position, key, value.toString());
                break;
            case 'double':
                value = filter.getAnimDoubleValue(position,key)
                filter.cache_setKeyFrameParaValue(position, key, value.toString());
                break;
            case 'string':
                value = filter.getAnimStringValue(position,key)
                filter.cache_setKeyFrameParaValue(position, key, value.toString());
                break;
            case 'rect':
                value = filter.getAnimRectValue(position,key)
                filter.cache_setKeyFrameParaRectValue(position, key, value);
                break;
            default:
                break;
            }
        }
        
        filter.syncCacheToProject();
        bKeyFrame = true

        bBlockUpdateUI = false


        for(var j = 0; j < paramCount; j++)
        {
            key = metadata.keyframes.parameters[j].property
            var t = filter.get(key)
        }

        showAddFrameInfo(position)
    }

    //帧位置改变时加载控件参数，从mlt底层读取数据更新到界面上
    function updateParamsUI(layoutRoot){
        if(typeof layoutRoot == 'undefined'){
            throw new Error("layoutRoot is undefined:"+layoutRoot)
        }

        if(bBlockUpdateUI == true) return
        currentFrame = timeline.getPositionInCurrentClip()
        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }

        bBlockUIChangedSignal = true
        var metaParamList = metadata.keyframes.parameters
        for(var paramIndex=0;paramIndex<metaParamList.length;paramIndex++){
            var parameter = metaParamList[paramIndex]
            var control = findControl(parameter.objectName,layoutRoot)
            if(control === null)
                continue;
            switch(parameter.controlType)
            {
            case "SliderSpinner":
                loadControlSlider(control, paramIndex)
                break;

            case "CheckBox":
                loadControlCheckbox(control,paramIndex)
                break;

            case "ColorWheelItem":
                var parameter2 = metaParamList[paramIndex+1]
                var parameter3 = metaParamList[paramIndex+2]
                loadControlColorWheel(control,paramIndex,paramIndex+1,paramIndex+2)
                // paramIndex = paramIndex+2
                break;

            case "ColorPicker":
                loadColorPicker(control, paramIndex)
                break;

            case "Slider":
                loadSlider(control,paramIndex)
                break;
            case "StringCtr":
                loadStringCtr(control,paramIndex)
                break;

            default :
                break;
            }
            // 一个控件对应几个参数的，取一次就可以反算出来了
            var paramList = getParamsAssociatedWithControl(control)
            paramIndex = paramIndex + paramList.length -1
        }

        bBlockUIChangedSignal = false
    }
    // 数据写入，将控件的数值set到filter里面，将界面参数值更新到project中
    function syncDataToProject(layoutRoot){

        if(typeof layoutRoot == 'undefined'){
            throw new Error("layoutRoot is undefined:"+layoutRoot)
        }
        clearAllFilterAnimationStatus()
        //resetAnim2No(layoutRoot)

        bBlockUpdateUI = true

        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        var metaParamList = metadata.keyframes.parameters
        for(var paramIndex=0;paramIndex<metaParamList.length;paramIndex++){
            var parameter = metaParamList[paramIndex]
            var control = findControl(parameter.objectName,layoutRoot)
            if(control === null)
                continue;
            
            switch(parameter.controlType)
            {
            case "SliderSpinner":
                filter.set(parameter.property,calcProjValByUIVal(control.value,parameter.factorFunc))
                break;

            case "CheckBox":
                filter.set(parameter.property,Number(control.checked))
                break;

            case "ColorWheelItem":
                var parameter2 = metaParamList[paramIndex+1]
                var parameter3 = metaParamList[paramIndex+2]
                filter.set(parameter.property,calcProjValByUIVal(control.red,parameter.factorFunc))
                filter.set(parameter2.property,calcProjValByUIVal(control.green,parameter2.factorFunc))
                filter.set(parameter3.property,calcProjValByUIVal(control.blue,parameter3.factorFunc))
                paramIndex = paramIndex+2
                break;

            case "ColorPicker":
                colorRect.x = calcProjValByUIVal(parseInt("0x" + control.value.slice(1, 3)),parameter.factorFunc)
                colorRect.y = calcProjValByUIVal(parseInt("0x" + control.value.slice(3, 5)),parameter.factorFunc)
                colorRect.width = calcProjValByUIVal(parseInt("0x" + control.value.slice(5, 7)),parameter.factorFunc)
                filter.set(parameter.property,colorRect)
                break;

            case "Slider":
                filter.set(parameter.property,calcProjValByUIVal(control.value,parameter.factorFunc))
                break;

            case "StringCtr":
                filter.set(parameter.property,control.currentText)
                break;

            default :
                //control.value = parameter.defaultValue
                //filter.set(parameter.property,control.value)
                break;
            }

        }

        bBlockUpdateUI = false
    }

    // 清除filter的动画状态，当没有keyframe时处理所有的filter 属性参数reset 清空  更改成切换动画非动画更好。
    function clearAllFilterAnimationStatus() {
        //if(filter.cache_getKeyFrameNumber() > 0) return 

        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        var metaParamList = metadata.keyframes.parameters
        for(var i = 0; i < metaParamList.length; i++)
        {
            var parameter = metaParamList[i]
            filter.resetProperty(parameter.property)
        }
    }

    //弃用的函数
    function resetAnim2No(layoutRoot){
        if(typeof layoutRoot == 'undefined'){
            throw new Error("layoutRoot is undefined:"+layoutRoot)
        }
        //最后一个关键帧移除之后触发用的
        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        var metaParamList1 = metadata.keyframes.parameters
        var parameter = metaParamList1[0]
        var control = findControl(parameter.objectName,layoutRoot)
        if(filter.cache_getKeyFrameNumber() <= 0){
            switch(parameter.controlType)
            {
            case "SliderSpinner":
                var tmp = control.value
                control.value = parseFloat(parameter.defaultValue) / 2
                control.value = parseFloat(parameter.defaultValue) * 2
                control.value = tmp
            break;

            case "CheckBox":
                var tmp1 = control.value
                control.value = Number(parameter.defaultValue) / 2
                control.value = Number(parameter.defaultValue) * 2
                control.value = tmp1
                break;

            case "ColorWheelItem":
                var tmp2 = Qt.rgba(control.red / 255, control.green / 255, control.blue / 255, 1.0 )
                control.color = Qt.rgba( 0, 0, 0, 1.0 )
                control.color = Qt.rgba( 0.5, 0.5, 0.5, 1.0 )
                control.color = tmp2
                break;

            case "ColorPicker":
            case "Slider":
            default :
                break;
            
            }
        }
    }
    //加减乘除 分别用 + - x c 被除b，对数log，指数pow
    // 控件到project mlt底层的写入保存计算
    function calcProjValByUIVal(value,factorFunc){
        var rt = value
        for(var i=0;i<factorFunc.length;i++){
            var calc = factorFunc[i]
            var calcSymbol = calc.substring(0,calc.lastIndexOf(":"))
            var calcValue = parseFloat(calc.substring(calc.lastIndexOf(":")+1,calc.length))
            switch(calcSymbol)
            {
            case '+':
                rt = rt + calcValue
                break;

            case '-':
                rt = rt - calcValue
                break;

            case 'x':
                rt = rt * calcValue
                break;

            case 'c':
                rt = rt / calcValue
                break;
            
            case 'b':
                if (rt === 0)
                   rt = 1
                else
                   rt = calcValue / rt
                //rt = calcValue / rt
                break;

            case 'log':
            //非线性插值，有可能超出规定范围
                rt = Math.log(Math.abs(rt)) / Math.log(Math.abs(calcValue))
                break;

            case 'pow':
                rt = Math.pow(calcValue, rt);
                break;
            }
        }
        return rt;
    }
    // project mlt底层到控件参数的加载计算，刚好与写入保存相反
    function calcUIValByProjVal(value,factorFunc){
        var rt = value
        for(var i=factorFunc.length-1;i>=0;i--){
            var calc = factorFunc[i]
            var calcSymbol = calc.substring(0,calc.lastIndexOf(":"))
            var calcValue = parseFloat(calc.substring(calc.lastIndexOf(":")+1,calc.length))
            switch(calcSymbol)
            {
            case '+':
                rt = rt - calcValue
                break;

            case '-':
                rt = rt + calcValue
                break;

            case 'x':
                rt = rt / calcValue
                break;

            case 'c':
                rt = rt * calcValue
                break;

            case 'b':
                if (rt === 0)
                   rt = 1
                else
                   rt = calcValue / rt
                //rt = calcValue / rt
                break;

            case 'log':
                rt = Math.pow(calcValue, rt);
                break;

            case 'pow':
                rt = Math.log(Math.abs(rt)) / Math.log(Math.abs(calcValue))
                break;
            }
        }
        return rt;
    }
    // 根据objectName和root节点查找root的孩子节点中objectName为objectName的子节点
    function findControl(objectName,root){
        if((typeof root == 'undefined')||(typeof root.children == 'undefined')){
            throw new Error("root is abnormal:"+root)
        }

        var controlList = root.children
        for(var i=0;i<controlList.length;i++){
            if(objectName === controlList[i].objectName){
                return controlList[i]
            }
        }
        for(i=0;i<controlList.length;i++){
            if(!isEmptyObject(controlList[i].children)){
                var controlList1 = controlList[i].children
                for(var j=0;j<controlList1.length;j++){
                    if(objectName === controlList1[j].objectName){
                        return controlList1[j]
                    }
                }
            }
        }
        return null;
    }
    // 对象是否有children
    function isEmptyObject(obj) {
      for (var key in obj) {
        return false;
      }
      return true;
    }
    // 根据控件id查找配置项，查找对应哪几个参数，因为会有一个控件对应几个参数
    function getParamsAssociatedWithControl(id){
        if((typeof id == 'undefined')||(typeof id.objectName == 'undefined')){
            throw new Error("id is abnormal:"+id)
        }
        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        var rt = [];
        for(var i=0;i<metadata.keyframes.parameters.length;i++){
            if(id.objectName === metadata.keyframes.parameters[i].objectName)
            rt.push(i)
        }
        return  rt;
    }
    // 加载单条滑条数据 : 由于是需要对meta里面的value进行直接修改，所以不能传对象，只能传地址
    function loadControlSlider(control,paramIndex){
        if((typeof control == 'undefined')||(typeof control.value == 'undefined')){
            throw new Error("control is abnormal:"+control)
        }

        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        if(metadata.keyframes.parameters.length < paramIndex){
            throw new Error("metadata.keyframes.parameters array not longer enough:"+metadata.keyframes.parameters.length + " "+paramIndex)
        }

        var parameter = metadata.keyframes.parameters[paramIndex]
        if(filter.cache_bKeyFrame(currentFrame)){
            var tempValue = filter.cache_getKeyFrameParaDoubleValue(currentFrame, parameter.property);
            if(tempValue !== -1.0)
            {
                control.value = calcUIValByProjVal(tempValue,parameter.factorFunc)
                
            }
        }else{
            filter.get(parameter.property)
            //filter.syncCacheToProject();
            var tempValue1 = filter.getAnimDoubleValue(currentFrame, parameter.property)
            filter.get(parameter.property)
            parameter.value = calcUIValByProjVal(tempValue1,parameter.factorFunc)
            
            // 一定要先设配置参数，再设control的value，不然control的value一旦改变，就会触发新的动作，而那里面会用到parameter的value
            var parameterList = getParamsAssociatedWithControl(control)
            for(var i=0;i< parameterList.length;i++){ //所有参数的value都要设，不然后面比较的时候会有问题
                metadata.keyframes.parameters[parameterList[i]].value = parameter.value
            }
            //非线性插值，有可能超出规定范围
            var changeValue = false
            if(parameter.value > control.maximumValue){
                control.value = control.maximumValue
                changeValue = true
            }else if(parameter.value < control.minimumValue){
                control.value = control.minimumValue
                changeValue = true
            }else{
                control.value = parameter.value
            }
            if(changeValue){
                for(i=0;i< parameterList.length;i++){
                    parameter = metadata.keyframes.parameters[parameterList[i]]
                    filter.set(parameter.property,calcProjValByUIVal(control.value,parameter.factorFunc))
                    // filter.cache_setKeyFrameParaValue(currentFrame, parameter.property, calcProjValByUIVal(control.value,parameter.factorFunc).toString());
                }
            }
        }
    }

    // 加载Checkbox数据，从mlt读取数据更新到界面
    function loadControlCheckbox(control,paramIndex){
        if((typeof control == 'undefined')||(typeof control.checked == 'undefined')){
            throw new Error("control is abnormal:"+control)
        }
        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        if(metadata.keyframes.parameters.length < paramIndex){
            throw new Error("metadata.keyframes.parameters array not longer enough:"+metadata.keyframes.parameters.length + " "+paramIndex)
        }

        var parameter = metadata.keyframes.parameters[paramIndex]
        if(filter.cache_bKeyFrame(currentFrame)){
            var test = filter.get(parameter.property)
            var tempValue = filter.cache_getKeyFrameParaDoubleValue(currentFrame, parameter.property);
            if(tempValue !== -1.0)
            {
                control.checked = Boolean(tempValue)
            }
        }
        else{
            filter.get(parameter.property)
            var tempValue1 = filter.getAnimIntValue(currentFrame, parameter.property)
            filter.get(parameter.property)
            control.checked = parameter.value = Boolean(tempValue1)
        }
    }

    // 加载ColorWheel数据，从mlt读取数据更新到界面
    function loadControlColorWheel(control,paramIndex1,paramIndex2,paramIndex3){
        if((typeof control == 'undefined')||(typeof control.color == 'undefined')){
            throw new Error("control is abnormal:"+control)
        }
        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        if(metadata.keyframes.parameters.length < paramIndex3){
            throw new Error("metadata.keyframes.parameters array not longer enough:"+metadata.keyframes.parameters.length + " "+paramIndex3)
        }
        var parameter1 = metadata.keyframes.parameters[paramIndex1]
        var parameter2 = metadata.keyframes.parameters[paramIndex2]
        var parameter3 = metadata.keyframes.parameters[paramIndex3]
        var rValue = filter.cache_getKeyFrameParaDoubleValue(currentFrame, parameter1.property);
        var gValue = filter.cache_getKeyFrameParaDoubleValue(currentFrame, parameter2.property);
        var bValue = filter.cache_getKeyFrameParaDoubleValue(currentFrame, parameter3.property);
        if(rValue === -1.0)
        {
            filter.get(parameter1.property)
            rValue = filter.getAnimDoubleValue(currentFrame, parameter1.property);
            filter.get(parameter1.property)
            filter.get(parameter2.property)
            gValue = filter.getAnimDoubleValue(currentFrame, parameter2.property);
            filter.get(parameter2.property)
            filter.get(parameter3.property)
            bValue = filter.getAnimDoubleValue(currentFrame, parameter3.property);
            filter.get(parameter3.property)
        }
        var tempRed = calcUIValByProjVal(rValue,parameter1.factorFunc)
        var tempGreen = calcUIValByProjVal(gValue,parameter2.factorFunc)
        var tempBlue = calcUIValByProjVal(bValue,parameter3.factorFunc)
        // 一定要先改参数值，再改control值
        if(!bKeyFrame){
            parameter1.value = parameter2.value = parameter3.value = Qt.rgba( tempRed / 255.0, tempGreen / 255.0, tempBlue / 255.0, 1.0 )
            control.color = parameter1.value
        }
        control.red = tempRed
        control.green = tempGreen
        control.blue = tempBlue
        
        
    }

    // 加载ColorPicker数据，从mlt读取数据更新到界面
    function loadColorPicker(control,paramIndex){
        if((typeof control == 'undefined')||(typeof control.value == 'undefined')){
            throw new Error("control is abnormal:"+control)
        }

        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        if(metadata.keyframes.parameters.length < paramIndex){
            throw new Error("metadata.keyframes.parameters array not longer enough:"+metadata.keyframes.parameters.length + " "+paramIndex)
        }
        var parameter = metadata.keyframes.parameters[paramIndex]

        filter.get(parameter.property)
        var tempValue = filter.getAnimRectValue(currentFrame, parameter.property)
        filter.get(parameter.property)
        parameter.value = Qt.rgba(parseFloat(tempValue.x),parseFloat(tempValue.y),parseFloat(tempValue.width),1.0)
        // 一定要先设配置参数，再设control的value，不然control的value一旦改变，就会触发新的动作，而那里面会用到parameter的value
        control.value = parameter.value
        
    }

    // 加载Slider数据，从mlt读取数据更新到界面
    function loadSlider(control,paramIndex){
        if((typeof control == 'undefined')||(typeof control.value == 'undefined')){
            throw new Error("control is abnormal:"+control)
        }

        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        if(metadata.keyframes.parameters.length < paramIndex){
            throw new Error("metadata.keyframes.parameters array not longer enough:"+metadata.keyframes.parameters.length + " "+paramIndex)
        }
        var parameter = metadata.keyframes.parameters[paramIndex]
        if(filter.cache_bKeyFrame(currentFrame)){
            var tempValue = filter.cache_getKeyFrameParaDoubleValue(currentFrame, parameter.property);
            if(tempValue !== -1.0)
            {
                control.value = calcUIValByProjVal(tempValue,parameter.factorFunc)
            }
        }else{
            filter.get(parameter.property)
            var tempValue1 = filter.getAnimDoubleValue(currentFrame, parameter.property)
            filter.get(parameter.property)
            parameter.value = calcUIValByProjVal(tempValue1,parameter.factorFunc)
            // 一定要先设配置参数，再设control的value，不然control的value一旦改变，就会触发新的动作，而那里面会用到parameter的value
            var parameterList = getParamsAssociatedWithControl(control)
            for(var i=0;i< parameterList.length;i++){ //所有参数的value都要设，不然后面比较的时候会有问题
                metadata.keyframes.parameters[parameterList[i]].value = parameter.value
            }
            control.value = parameter.value
        }
    }

    // 加载String数据，从mlt读取数据更新到界面
    function loadStringCtr(control,paramIndex){
        if((typeof control == 'undefined')||(typeof control.currentIndex == 'undefined')){
            throw new Error("control is abnormal:"+control)
        }

        if((typeof metadata == 'undefined')||(typeof metadata.keyframes == 'undefined')||(typeof metadata.keyframes.parameters == 'undefined')){
            throw new Error("metadata is abnormal")
        }
        if(metadata.keyframes.parameters.length < paramIndex){
            throw new Error("metadata.keyframes.parameters array not longer enough:"+metadata.keyframes.parameters.length + " "+paramIndex)
        }
        var parameter = metadata.keyframes.parameters[paramIndex]
        if(filter.cache_bKeyFrame(currentFrame)){
            var tempValue = filter.cache_getKeyFrameParaValue(currentFrame, parameter.property);
            if(tempValue !== -1.0)
            {
                var index0 = findDefaultIndex(tempValue,control.model)
                control.currentIndex = index0
            }
        }
    }

    //获取当前帧
    function getCurrentFrame(){
        return currentFrame;
    }

    Component.onCompleted:
    {
        currentFrame = timeline.getPositionInCurrentClip()
    }

    // 开启关键帧
    Connections {
        target: keyFrameControl
        onEnableKeyFrameChanged: {
            updateEnableKeyFrame(bEnable)
        }
    }

    // 自动添加关键帧信号，当参数改变时
    Connections {
        target: keyFrameControl
        onAutoAddKeyFrameChanged: {
            updateAutoSetAsKeyFrame(bEnable)
        }
    }

    // 添加关键帧信号
    Connections {
             target: keyFrameControl
             onAddFrameChanged: {
                 bKeyFrame = true
                 //syncUIDataToProject()
                 addKeyFrame()
             }
    }
    // 帧位置改变信号
    Connections {
             target: keyFrameControl
             onFrameChanged: {
                 currentFrame = keyFrameNum
                 bKeyFrame = filter.cache_bKeyFrame(currentFrame)
                 refreshUI()
             }
    }
    // 移除关键帧信号
    Connections {
             target: keyFrameControl
             onRemoveKeyFrame: {
                bKeyFrame = false
                var nFrame = keyFrame.getCurrentFrame();
                
                filter.removeKeyFrameParaValue(nFrame);
                //filter.syncCacheToProject();
                //syncUIDataToProject()
                
             }
    }
    // 移除所有关键帧信号
    Connections {
             target: keyFrameControl
             onRemoveAllKeyFrame: {
                bKeyFrame = false
                filter.removeAllKeyFrame()
                filter.syncCacheToProject();
                syncUIDataToProject()
             }
    }
}

