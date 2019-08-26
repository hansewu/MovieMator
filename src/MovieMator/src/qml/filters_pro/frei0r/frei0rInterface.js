//  const char* name; 滤镜(插件)的名称
//  const char* author;  作者
//  int plugin_type;    滤镜类型（详细见下面）
//  int color_model;   是否启用color模型
//  int frei0r_version;  frei0r的版本
//  int major_version;   滤镜的主要版本
//  int minor_version;   滤镜的次要版本
//  int num_params;    滤镜参数的个数
//  const char* explanation;  滤镜的其他辅助描述
//} f0r_plugin_info_t;

//F0R_PARAM_BOOL      0  布尔类型  f0r_param_bool
//F0R_PARAM_DOUBLE    1  double类型  f0r_param_double
//F0R_PARAM_COLOR     2  color类型   f0r_param_color
//F0R_PARAM_POSITION  3  位置类型	f0r_param_position
//F0R_PARAM_STRING    4  string类型  f0r_param_string
var filterName = metadata.name
var metaParamList = metadata.keyframes.parameters
var paramNum = metaParamList.length

// 生成引入关键帧控件
function createKeyFrame(){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'import MovieMator.Controls 1.0;'
        +   'YFKeyFrame{'
        +       'objectName: "keyFrame";'
        +       'onSyncUIDataToProject:{keyFrameSynchroData();}'
        +       'onRefreshUI:{keyFrameLoad();}'
        +   '}'
    Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
}

// 生成一个空白站位控件
function createLableEmpty(){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'import QtQuick.Layouts 1.0;'
        +   'Label {}'

    Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
}
// 生成每行首部描述控件
function createLable(paramInfo,index){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'import QtQuick.Controls.Styles 1.4;'
        +   'Label {'
        +    'text: qsTr("'+paramInfo.name+'");'
        +    'color: "#ffffff";'
        +    'horizontalAlignment:Text.AlignRight;'
        +   '}'

    Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
}
// 生成复选框
function createCheckBox(paramInfo,index){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'import QtQuick.Controls.Styles 1.4;'
        +   'CheckBox {'
        +        'objectName: "'+filterName+paramInfo.name+index+'";'
        +        'onClicked: {checkBoxClicked(objectName);}'
        +        'style: CheckBoxStyle {'
        +            'label: Text {'
        +                'color: "white";'
        +                'text: qsTr("'+paramInfo.name+'?")'
        +            '}'
        +       '}'
        +    '}'
    var ctr = Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
    // ctr.checked = Math.round(parseFloat(paramInfo.defaultValue));
}
// 生成滑条
function createSlider(paramInfo,index){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'import MovieMator.Controls 1.0;'
        +   'SliderSpinner {'
        +       'objectName: "'+filterName+paramInfo.name+index+'";'
        +       'minimumValue: 0;'
        +       'maximumValue: 100;'
        +       'value:26;'
        +       'onValueChanged: {sliderValueChanged(objectName);}'
        +   '}'
    
    var ctr = Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
    // ctr.value = parseFloat(paramInfo.defaultValue) * 100
}
// 生成滑条还原操作按钮
function createSliderUndo(paramInfo,index){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'import MovieMator.Controls 1.0;'
        +   'UndoButton{onClicked:sliderUndoClicked("'+filterName+paramInfo.name+index+'");}'

    Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
}
// 生成颜色选择轮
function createColorPicker(paramInfo,index){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'import MovieMator.Controls 1.0;'
        +   'ColorPicker {'
        +        'objectName: "'+filterName+paramInfo.name+index+'";'
        +        'onValueChanged: {colorPickerChanged(objectName);}'
        +    '}'
    var ctr = Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
    // var rgb = paramInfo.defaultValue.split(" ")
    // ctr.value = Qt.rgba(parseFloat(rgb[0]),parseFloat(rgb[1]),parseFloat(rgb[2]),1.0)
}
// 颜色选择轮操作按钮
function createColorUndo(paramInfo,index){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'import MovieMator.Controls 1.0;'
        +   'UndoButton{onClicked:colorUndoClicked("'+filterName+paramInfo.name+index+'");}'

    Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
}
// 生成坐位置标输入选择框
function createPositionControl(paramInfo,index){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +    'Position{'
        +        'objectName: "'+filterName+paramInfo.name+index+'";'
        +    '}'
    Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
}
// 位置信息还原操作按钮
function createPositionUndo(paramInfo,index){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'import MovieMator.Controls 1.0;'
        +   'UndoButton{onClicked:positionUndoClicked("'+filterName+paramInfo.name+index+'");}'

    Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
}
// 提取explanation里面的枚举值
function extractStringList(paramInfo){
    var modelStr = paramInfo.explanation
    if(modelStr.indexOf(":") != -1){
        modelStr = modelStr.substring(modelStr.indexOf(":")+1,modelStr.length)
        modelStr = modelStr.replace(/\'/g,"")
        // modelStr = "qsTr(\""+modelStr.replace(/,/g,"\"), qsTr(\"") + "\")"
        modelStr = modelStr.replace(/ /g,"")
    }else if(modelStr.indexOf("[") != -1){
        modelStr = modelStr.substring(modelStr.indexOf("[")+1,modelStr.indexOf("]"))
        modelStr = modelStr.replace(/ /g,"")
    }else if(modelStr.indexOf("One of") != -1){
        modelStr = modelStr.substring(modelStr.indexOf("One of")+6,modelStr.length)
        modelStr = modelStr.replace(/or/g,",")
        modelStr = modelStr.replace(/\'/g,"")
        modelStr = modelStr.replace(".","")
        modelStr = modelStr.replace(/, ,/g,",")
        modelStr = modelStr.replace(/ /g,"")
    }else if(modelStr.indexOf(" or ") != -1){    
        modelStr = modelStr.replace(/ or /g,",")
        modelStr = modelStr.substring(0,modelStr.indexOf(" "))
        modelStr = modelStr.replace(/ /g,"")
    }else{
        modelStr = ""
    }

    var preFix = ''
    var po = paramInfo.defaultValue.indexOf("_")
    if(po > 0){
        preFix = paramInfo.defaultValue.slice(0,po+1)
    }
    var modelList = modelStr.split(",")
    for(var i=0;i<modelList.length;i++){
        modelList[i] = preFix + modelList[i].toLowerCase()
    }
    return modelList
}

// 生成文本输入框
function createStringCtr(paramInfo,index){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'ComboBox {'
        +       'objectName: "'+filterName+paramInfo.name+index+'";'
        +       'editable:(model.length <= 0)?true:false ;'
        // +       'model: ['+modelStr+'];'
        +       'onCurrentIndexChanged: {stringValueChanged("'+filterName+paramInfo.name+index+'");}'
        +   '}'
    var ctr = Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
    ctr.model = extractStringList(paramInfo)
}
// 文本框还原操作按钮
function createStringCtrUndo(paramInfo,index){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'import MovieMator.Controls 1.0;'
        +   'UndoButton{onClicked:strinCtrUndoClicked("'+filterName+paramInfo.name+index+'");}'

    Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
}
// 最后填充空白区
function createFootFill(){
    var str = ''
    str +=  'import QtQuick 2.0;'
        +   'import QtQuick.Controls 1.4;'
        +   'import QtQuick.Layouts 1.0;'
        +   'Item {Layout.fillHeight: true}'
    Qt.createQmlObject(str,layoutRoot, "dynamicSnippet1");
}

// 根据参数类型生成UI控件
function createUIbyParamType(paramInfo,index){
    var rt = index
    switch(paramInfo.controlType){
    case 'CheckBox':
        createLableEmpty()
        //name加序号作为唯一标识id,objectName
        createCheckBox(paramInfo,index)
        createLableEmpty()
        break;
    case 'SliderSpinner':
        createLable(paramInfo,index)
        createSlider(paramInfo,index)
        createSliderUndo(paramInfo,index)
        break;
    case 'ColorPicker':
        createLable(paramInfo,index)
        createColorPicker(paramInfo,index)
        createColorUndo(paramInfo,index)
        break;
    case 'Position':
        createLable(paramInfo,index)
        createPositionControl(paramInfo,index)
        createPositionUndo(paramInfo,index)
        rt = idnex+1
        break;
    case 'StringCtr':
        createLable(paramInfo,index)
        createStringCtr(paramInfo,index)
        createStringCtrUndo(paramInfo,index)
        break;
    default:
        break;
    }
    return rt
}

// 设置bool型配置项
function setBoolParam(index){
    var paramInfo1 = metadata.keyframes.parameters[index]
    var objectName = filterName + paramInfo1.name + index + ''
    paramInfo1.paraType = 'int'
    paramInfo1.property = ''+index
    paramInfo1.objectName = objectName
    paramInfo1.controlType = 'CheckBox'
    // paramInfo1.defaultValue = ''
    paramInfo1.value = ''
    paramInfo1.factorFunc = []
}
//设置double类型的配置项
function setDoubleParam(index){
    var paramInfo1 = metadata.keyframes.parameters[index]
    var objectName = filterName + paramInfo1.name + index + ''
    paramInfo1.paraType = 'double'
    paramInfo1.property = ''+index
    paramInfo1.objectName = objectName
    paramInfo1.controlType = 'SliderSpinner'
    // paramInfo1.defaultValue = '26'
    paramInfo1.value = '26'
    paramInfo1.factorFunc = ['c:100.0']
}
// 设置color类型的配置项
function setColorParam(index){
    // colorPicker
    var paramInfo1 = metadata.keyframes.parameters[index]
    var objectName = filterName + paramInfo1.name + index + ''
    paramInfo1.paraType = 'rect'
    paramInfo1.property = ''+index
    paramInfo1.objectName = objectName
    paramInfo1.controlType = 'ColorPicker'
    // paramInfo1.defaultValue = ''
    paramInfo1.value = '#000000'
    paramInfo1.factorFunc = ['c:255.0']
}
// 设置position类型的配置项
function setPositionParam(index){
    var paramInfo1 = metadata.keyframes.parameters[index]
    var objectName = filterName + paramInfo1.name + index + ''
    paramInfo1.paraType = 'double'
    paramInfo1.property = ''+index
    paramInfo1.objectName = objectName
    paramInfo1.controlType = 'Position'
    // paramInfo1.defaultValue = '0'
    paramInfo1.value = '0'
    paramInfo1.factorFunc = []

    index++
    var paramInfo2 = metadata.keyframes.parameters[index]
    paramInfo2.paraType = 'double'
    paramInfo2.property = ''+index
    paramInfo2.objectName = objectName
    paramInfo2.controlType = 'Position'
    // paramInfo2.defaultValue = '0'
    paramInfo2.value = '0'
    paramInfo2.factorFunc = []
}
// 设置string类型配置项
function setStringParam(index){
    var paramInfo1 = metadata.keyframes.parameters[index]
    var objectName = filterName + paramInfo1.name + index + ''
    paramInfo1.paraType = 'string'
    paramInfo1.property = ''+index
    paramInfo1.objectName = objectName
    paramInfo1.controlType = 'StringCtr'
    // paramInfo1.defaultValue = ''
    paramInfo1.value = ''
    paramInfo1.factorFunc = []
}

function setMetaKeyFrameParams(paramInfo,index){
    var rt = index
    switch(paramInfo.paraType){
    case 'bool':
        setBoolParam(index)
        break;
    case 'double':
        setDoubleParam(index)
        break;
    case 'color':
        setColorParam(index)
        break;
    case 'position':
        setPositionParam(index)
        rt = index+1
        break;
    case 'string':
        setStringParam(index)
        break;
    default:
        return index;
    }
    return rt
}

function init(isNew){  //滤镜UI界面初始化
    for(var i=0;i<paramNum;i++){ 
        var index = i
        var paramInfo = metaParamList[i] 
        if('' == paramInfo.controlType){//如果已经设过了就不设了
            setMetaKeyFrameParams(paramInfo,i)
        }
        index = createUIbyParamType(paramInfo,i) 
        i=index
    }
    createFootFill()//填充空白区域
}

// 根据objectName和root节点查找子节点
function findControl(objectName,root){
    var controlList = root.children
    for(var i=0;i<controlList.length;i++){
        if(objectName === controlList[i].objectName){
            return controlList[i]
        }
    }
    for(var i=0;i<controlList.length;i++){
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