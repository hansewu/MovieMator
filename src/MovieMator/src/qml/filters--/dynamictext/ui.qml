
import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import MovieMator.Controls 1.0
import QtQuick.Dialogs 1.1

Item {
    property string rectProperty: 'geometry'
    property string valignProperty: 'valign'
    property string halignProperty: 'halign'
    property rect filterRect: filter.getRect(rectProperty)
    property var _locale: Qt.locale(application.numericLocale)
    property bool _bKeyFrame: false
    width: 330
    height: 400

    Component.onCompleted: {
        if (filter.isNew) {
            if (application.OS === 'Windows')
                filter.set('family', 'Verdana')
            filter.set('fgcolour', '#ffffffff')
            filter.set('bgcolour', '#00000000')
            filter.set('olcolour', '#ff000000')
            filter.set('weight', 500)

            filter.set(rectProperty,   '0/50%:50%x50%')
            filter.set(valignProperty, 'bottom')
            filter.set(halignProperty, 'left')
            filter.savePreset(preset.parameters, qsTr('Bottom Left'))

            filter.set(rectProperty,   '50%/50%:50%x50%')
            filter.set(valignProperty, 'bottom')
            filter.set(halignProperty, 'right')
            filter.savePreset(preset.parameters, qsTr('Bottom Right'))

            filter.set(rectProperty,   '0/0:50%x50%')
            filter.set(valignProperty, 'top')
            filter.set(halignProperty, 'left')
            filter.savePreset(preset.parameters, qsTr('Top Left'))

            filter.set(rectProperty,   '50%/0:50%x50%')
            filter.set(valignProperty, 'top')
            filter.set(halignProperty, 'right')
            filter.savePreset(preset.parameters, qsTr('Top Right'))

            filter.set(rectProperty,   '0/76%:100%x14%')
            filter.set(valignProperty, 'bottom')
            filter.set(halignProperty, 'center')
            filter.savePreset(preset.parameters, qsTr('Lower Third'))

            filter.set(rectProperty, '0/0:100%x100%')
            filter.set(valignProperty, 'bottom')
            filter.set(halignProperty, 'center')
            filter.set('size', filterRect.height)
            filter.savePreset(preset.parameters)
        }
        setControls()
    }

    function setFilter() {
        var x = parseFloat(rectX.text)
        var y = parseFloat(rectY.text)
        var w = parseFloat(rectW.text)
        var h = parseFloat(rectH.text)
        if (x !== filterRect.x ||
            y !== filterRect.y ||
            w !== filterRect.width ||
            h !== filterRect.height) {
            filterRect.x = x
            filterRect.y = y
            filterRect.width = w
            filterRect.height = h
            if(_bKeyFrame)
            {
                var nFrame = keyFrame.getCurrentFrame()
                filter.setKeyFrameParaValue(nFrame, rectProperty, '%1%/%2%:%3%x%4%'
                           .arg((x / profile.width * 100).toLocaleString(_locale))
                           .arg((y / profile.height * 100).toLocaleString(_locale))
                           .arg((w / profile.width * 100).toLocaleString(_locale))
                           .arg((h / profile.height * 100).toLocaleString(_locale)))
            }
            else
                filter.set(rectProperty, '%1%/%2%:%3%x%4%'
                       .arg((x / profile.width * 100).toLocaleString(_locale))
                       .arg((y / profile.height * 100).toLocaleString(_locale))
                       .arg((w / profile.width * 100).toLocaleString(_locale))
                       .arg((h / profile.height * 100).toLocaleString(_locale)))
        }
    }

    function setControls() {
        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-argument");
        var index=0
        for(index=0; index<keyFrameCount;index++)
        {
            var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-argument");
            var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-argument");
            filter.setKeyFrameParaValue(nFrame, "argument", keyValue.toString())

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-fgcolour");
            filter.setKeyFrameParaValue(nFrame, "fgcolour", keyValue.toString())

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-family");
            filter.setKeyFrameParaValue(nFrame, "family", textValue.toString())

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-weight");
            filter.setKeyFrameParaValue(nFrame, "weight", textValue.toString())

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-olcolour");
            filter.setKeyFrameParaValue(nFrame, "olcolour", textValue.toString())

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-outline");
            filter.setKeyFrameParaValue(nFrame, "outline", textValue.toString())

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-bgcolour");
            filter.setKeyFrameParaValue(nFrame, "bgcolour", textValue.toString())

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-pad");
            filter.setKeyFrameParaValue(nFrame, "pad", textValue.toString())

            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-halign");
            filter.setKeyFrameParaValue(nFrame, halignProperty, textValue)


            keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-valign");
            filter.setKeyFrameParaValue(nFrame, valignProperty, textValue)

        }

        filter.combineAllKeyFramePara();

        textArea.text = filter.getKeyValueOnProjectOnIndex(0, "anim-argument")
        fgColor.value = filter.getKeyValueOnProjectOnIndex(0, "anim-fgcolour")
        fontButton.text = filter.getKeyValueOnProjectOnIndex(0, "anim-family")
        weightCombo.currentIndex = weightCombo.valueToIndex()
        outlineColor.value = filter.getKeyValueOnProjectOnIndex(0, "anim-olcolour")
        outlineSpinner.value = filter.getKeyValueOnProjectOnIndex(0, "anim-outline")
        bgColor.value = filter.getKeyValueOnProjectOnIndex(0, "anim-bgcolour")
        padSpinner.value = parseFloat(filter.getKeyValueOnProjectOnIndex(0, "anim-pad"))
        var align = filter.getKeyValueOnProjectOnIndex(0, "anim-halign")
        if (align === 'left')
            leftRadioButton.checked = true
        else if (align === 'center' || align === 'middle')
            centerRadioButton.checked = true
        else if (filter.get(halignProperty) === 'right')
            rightRadioButton.checked = true
        align = filter.getKeyValueOnProjectOnIndex(0, "anim-valign")
        if (align === 'top')
            topRadioButton.checked = true
        else if (align === 'center' || align === 'middle')
            middleRadioButton.checked = true
        else if (align === 'bottom')
            bottomRadioButton.checked = true
    }

    function setKeyFrameValue(bKeyFrame)
    {
        console.log("dynamictext, setKeyFrameValue is called")
        var nFrame = keyFrame.getCurrentFrame();
        if(bKeyFrame)
        {
            var textValue = fgColor.value
            filter.setKeyFrameParaValue(nFrame, "fgcolour", textValue.toString())

            textValue = textArea.text
            filter.setKeyFrameParaValue(nFrame, "argument", textValue.toString())

            textValue = fontButton.text
            filter.setKeyFrameParaValue(nFrame, "family", textValue.toString())

            textValue = 10*weightCombo.values[weightCombo.currentIndex]
            filter.setKeyFrameParaValue(nFrame, "weight", textValue.toString())

            textValue = outlineColor.value
            filter.setKeyFrameParaValue(nFrame, "olcolour", textValue.toString())

            textValue = outlineSpinner.value
            filter.setKeyFrameParaValue(nFrame, "outline", textValue.toString())

            textValue = bgColor.value
            filter.setKeyFrameParaValue(nFrame, "bgcolour", textValue.toString())

            textValue = padSpinner.value
            filter.setKeyFrameParaValue(nFrame, "pad", textValue.toString())

            if(halignGroup.current === leftRadioButton)
                textValue = "left"
            else if(halignGroup.current === centerRadioButton)
                textValue = "center"
            else if(halignGroup.current === rightRadioButton)
                textValue = "right"
            filter.setKeyFrameParaValue(nFrame, halignProperty, textValue)


            if(valignProperty.current === topRadioButton)
                textValue = "top"
            else if(valignProperty.current === middleRadioButton)
                textValue = "middle"
            else if(valignProperty.current === bottomRadioButton)
                textValue = "bottom"
            filter.setKeyFrameParaValue(nFrame, valignProperty, textValue)

            setFilter()

            filter.combineAllKeyFramePara();
        }
        else
        {
            //Todo, delete the keyframe date of the currentframe
            filter.removeKeyFrameParaValue(nFrame);

            var textValue = fgColor.value
            filter.set("fgcolour", textValue)

            textValue = textArea.text
            filter.set("argument", textValue)

            textValue = fontButton.text
            filter.set("family", textValue)

            textValue = 10*weightCombo.values[weightCombo.currentIndex]
            filter.set("weight", textValue)

            textValue = outlineColor.value
            filter.set("olcolour", textValue)

            textValue = outlineSpinner.value
            filter.set("outline", textValue)

            textValue = bgColor.value
            filter.set("bgcolour", textValue)

            textValue = padSpinner.value
            filter.set("pad", textValue)

            if(halignGroup.current === leftRadioButton)
                textValue = "left"
            else if(halignGroup.current === centerRadioButton)
                textValue = "center"
            else if(halignGroup.current === rightRadioButton)
                textValue = "right"
            filter.set(halignProperty, textValue)

            if(valignProperty.current === topRadioButton)
                textValue = "top"
            else if(valignProperty.current === middleRadioButton)
                textValue = "middle"
            else if(valignProperty.current === bottomRadioButton)
                textValue = "bottom"
            filter.set(valignProperty, textValue)

            setFilter()


        }
    }

    ExclusiveGroup { id: sizeGroup }
    ExclusiveGroup { id: halignGroup }
    ExclusiveGroup { id: valignGroup }

    GridLayout {
        columns: 5
        anchors.fill: parent
        anchors.margins: 8


        KeyFrame{
            id: keyFrame
            Layout.columnSpan:5
       //   currentPosition: filterDock.getCurrentPosition()
            onSetAsKeyFrame:
            {
                setKeyFrameValue(bKeyFrame)
                _bKeyFrame = bKeyFrame;
            }

            onLoadKeyFrame:
            {

                var textValue = filter.getKeyFrameParaValue(keyFrameNum, "fgcolour");
                if(textValue != "")
                    fgColor.value = textValue;

                textValue = filter.getKeyFrameParaValue(keyFrameNum, "argument");
                if(textValue != "")
                    textArea.text = textValue;

                textValue = filter.getKeyFrameParaValue(keyFrameNum, "family");
                if(textValue != "")
                    fontButton.text = textValue;

                textValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "weight");
                if(textValue != "")
                    weightCombo.currentIndex = weightCombo.valueToIndex();

                textValue = filter.getKeyFrameParaValue(keyFrameNum, "olcolour");
                if(textValue != "")
                    outlineColor.value = textValue;

                textValue = filter.getKeyFrameParaValue(keyFrameNum, "bgcolour");
                if(textValue != "")
                    bgColor.value = textValue;

                textValue = filter.getKeyFrameParaValue(keyFrameNum, "pad");
                if(textValue != "")
                    padSpinner.value = textValue;

                textValue = filter.getKeyFrameParaValue(keyFrameNum, halignProperty);
                if(textValue === "left")
                    leftRadioButton.checked = true;
                else if(textValue === "center")
                    centerRadioButton.checked = true;
                else if(textValue === "right")
                    rightRadioButton.checked = true;

                textValue = filter.getKeyFrameParaValue(keyFrameNum, valignProperty);
                if(textValue === "top")
                    topRadioButton.checked = true;
                else if(textValue === "middle")
                    middleRadioButton.checked = true;
                else if(textValue === "bottom")
                    bottomRadioButton.checked = true;
            }
        }


        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        Preset {
            id: preset

            parameters: [rectProperty, halignProperty, valignProperty, 'argument', 'size',
            'fgcolour', 'family', 'weight', 'olcolour', 'outline', 'bgcolour', 'pad']
            Layout.columnSpan: 4
            onPresetSelected: setControls()
        }

        Label {
            text: qsTr('Text')
            Layout.alignment: Qt.AlignRight | Qt.AlignTop
            color: '#ffffff'
        }
        TextArea {
            id: textArea
            Layout.columnSpan: 4
            textFormat: TextEdit.PlainText
            wrapMode: TextEdit.NoWrap
            Layout.minimumHeight: 40
            Layout.maximumHeight: 80
            Layout.minimumWidth: preset.width
            Layout.maximumWidth: preset.width
            text: '__empty__' // workaround initialization problem
            property int maxLength: 256
            onTextChanged: {

                if (text === '__empty__') return
                if (length > maxLength) {
                    text = text.substring(0, maxLength)
                    cursorPosition = maxLength
                }

                var nFrame = keyFrame.getCurrentFrame();
                if(_bKeyFrame)
                {
                    filter.setKeyFrameParaValue(nFrame, "argument", text)
                    filter.combineAllKeyFramePara();
                }
                else
                    filter.set('argument', text)
               // filter.anim_set("argument", "0=198;100=0;400=c")
            }
        }

        Label {
            text: qsTr('Insert field')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        RowLayout {
            Layout.columnSpan: 4
            Button {
                text: qsTr('Timecode')
                onClicked: textArea.insert(textArea.cursorPosition, '#timecode#')
            }
            Button {
                text: qsTr('Frame #', 'Frame number')
                onClicked: textArea.insert(textArea.cursorPosition, '#frame#')
            }
        }
        
        Label {
               text: qsTr('Insert field')
               Layout.alignment: Qt.AlignRight
               color: '#ffffff'
            }

        RowLayout{
            Layout.columnSpan: 4
            

            Button {
                text: qsTr('File date')
                onClicked: textArea.insert(textArea.cursorPosition, '#localfiledate#')
            }
            Button {
                text: qsTr('File name')
                onClicked: textArea.insert(textArea.cursorPosition, '#resource#')
            }
        }

        Label {
            text: qsTr('Font')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        
        RowLayout {
            Layout.columnSpan: 4

            ColorPicker {
                id: fgColor
                eyedropper: false
                alpha: true
                onValueChanged: 
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    if(_bKeyFrame)
                    {
                        filter.setKeyFrameParaValue(nFrame, "fgcolour",value.toString())
                        filter.combineAllKeyFramePara()
                    }
                    else
                        filter.set("fgcolour", value);
                  //  filter.anim_set("fgcolour", "0=#ffff0000;200=#ff00ff00;400=#ff0000ff")

                }
            }

            Button {
                id: fontButton
                onClicked: {
                    fontDialog.font = Qt.font({ family: filter.get('family'), pointSize: 24, weight: Font.Normal })
                    fontDialog.open()
                }
                FontDialog {
                    id: fontDialog
                    title: "Please choose a font"
                    onFontChanged: {
                        if(_bKeyFrame)
                        {
                            var nFrame = keyFrame.getCurrentFrame();
                            filter.setKeyFrameParaValue(nFrame, "family", font.family)
                            filter.combineAllKeyFramePara()
                        }
                        else
                        {
                            filter.set('family', font.family)
                        }
                    }
                    onAccepted: fontButton.text = font.family
                    onRejected: {
                        if(_bKeyFrame)
                        {
                            var nFrame = keyFrame.getCurrentFrame();
                            filter.setKeyFrameParaValue(nFrame, "family", fontButton.text)
                            filter.combineAllKeyFramePara()
                        }
                        else
                            filter.set('family', fontButton.text)
                    }
                }
            }
            ComboBox {
                id: weightCombo
                model: [qsTr('Normal'), qsTr('Bold'), qsTr('Light', 'thin font stroke')]
                property var values: [Font.Normal, Font.Bold, Font.Light]
                function valueToIndex() {
                    var w = filter.getDouble('weight')
                    for (var i = 0; i < values.length; ++i)
                        if (values[i] === w) break;
                    if (i === values.length) i = 0;
                    return i;
                }
                onActivated: {
                    if(_bKeyFrame)
                    {
                        var nFrame = keyFrame.getCurrentFrame();
                        filter.setKeyFrameParaValue(nFrame, "weight", (10 * values[index]).toString() )
                        filter.combineAllKeyFramePara()
                    }
                    else
                        filter.set('weight', 10 * values[index])
                }
           }
        }
        Label {
            text: qsTr('Outline')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
          //  Layout.column: 0
        }
        ColorPicker {
            id: outlineColor
            eyedropper: false
            alpha: true
            onValueChanged: {
                if(_bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "olcolour", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('olcolour', value)
            }
        }
        Label {
            text: qsTr('Thickness')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SpinBox {
            id: outlineSpinner
            Layout.minimumWidth: 50
            Layout.columnSpan: 2
            minimumValue: 0
            maximumValue: 30
            decimals: 0
            onValueChanged: {
                if(_bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "outline", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('outline', value)
            }
        }

        Label {
            text: qsTr('Background')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        ColorPicker {
            id: bgColor
            eyedropper: false
            alpha: true
            onValueChanged:
            {
                if(_bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "bgcolour",value.toString)
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('bgcolour', value)
            }
        }
        Label {
            text: qsTr('Padding')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SpinBox {
            id: padSpinner
            Layout.minimumWidth: 50
            Layout.columnSpan: 2
            minimumValue: 0
            maximumValue: 100
            decimals: 0
            onValueChanged:
            {
                if(_bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, "pad", value.toString())
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('pad', value)
            }
        }

        Label {
            text: qsTr('Position')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        RowLayout {
            Layout.columnSpan: 4
            TextField {
                id: rectX
                text: filterRect.x
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
            }
            Label {
                text: ','
                color: '#ffffff'
            }
            TextField {
                id: rectY
                text: filterRect.y
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
            }
        }
        Label {
            text: qsTr('Size')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        RowLayout {
            Layout.columnSpan: 4
            TextField {
                id: rectW
                text: filterRect.width
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
            }
            Label {
                text: 'x'
                color: '#ffffff'
            }
            TextField {
                id: rectH
                text: filterRect.height
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
            }
        }

        Label {
            text: qsTr('X fit')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        RadioButton {
            id: leftRadioButton
            text: qsTr('Left')
            exclusiveGroup: halignGroup
            onClicked:
            {
                if(_bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, halignProperty, "left")
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set(halignProperty, 'left')
            }
        }
        RadioButton {
            id: centerRadioButton
            text: qsTr('Center')
            exclusiveGroup: halignGroup
            onClicked: {
                if(_bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, halignProperty, "center")
                }
                else
                     filter.set(halignProperty, 'center')
            }
        }
        RadioButton {
            id: rightRadioButton
            text: qsTr('Right')
            exclusiveGroup: halignGroup
            onClicked:{
                if(_bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, halignProperty, "right")
                }
                else
                    filter.set(halignProperty, 'right')
            }
        }
        Item { Layout.fillWidth: true }

        Label {
            text: qsTr('Y fit')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        RadioButton {
            id: topRadioButton
            text: qsTr('Top')
            exclusiveGroup: valignGroup
            onClicked:
            {
                if(_bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, valignProperty, "top")
                }
                else
                    filter.set(valignProperty, 'top')
            }
        }
        RadioButton {
            id: middleRadioButton
            text: qsTr('Middle')
            exclusiveGroup: valignGroup
            onClicked:
            {
                if(_bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, valignProperty, "middle")
                }
                else
                    filter.set(valignProperty, 'middle')
            }
        }
        RadioButton {
            id: bottomRadioButton
            text: qsTr('Bottom')
            exclusiveGroup: valignGroup
            onClicked:
            {
                if(_bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame, valignProperty, "bottom")
                }
                else
                    filter.set(valignProperty, 'bottom')
            }
        }
        Item { Layout.fillWidth: true }

        Item { Layout.fillHeight: true }
    }

    Connections {
        target: filter
        onChanged: {
            var newValue = filter.getRect(rectProperty)
            if (filterRect !== newValue)
                filterRect = newValue
        }
    }
}

