/*
 * Copyright (c) 2014-2015 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl <wyl@pylwyl.local>
 * Author: Dragon-S <15919917852@163.com>
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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import MovieMator.Controls 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.4

Item {
    property string rectProperty: 'geometry'
    property string valignProperty: 'valign'
    property string halignProperty: 'halign'
    property string fgcolourProperty: "fgcolour"
    property string olcolourProperty: "olcolour"
    property string outlineProperty: "outline"
    property string bgcolourProperty: "bgcolour"
    property string padProperty: "pad"
    property string letterSpaceingProperty: "letter_spaceing"
    property rect filterRect
    property var _locale: Qt.locale(application.numericLocale)
    property bool blockUpdate: false

    width: 300
    height: 1500  //1150

    function getColorStr(colorValue)
    {
        var int_color = parseInt(colorValue)

        if(int_color <=0)
            int_color = 0

        if(int_color >= 255)
            int_color = 255

        var colorStr = int_color.toString(16)

        if(colorStr.length <= 1)
            colorStr = "0" + colorStr

        return colorStr
    }

    function isRectPosValid(rect)
    {
        if(rect.x == 2.22507e-308 || rect.y == 2.22507e-308 || rect.width == 2.22507e-308 || rect.height == 2.22507e-308)
            return false

        if(rect.x*10000 < -100 || rect.x > 0.8)
            return false
        if(rect.y*10000 < -100 || rect.y > 0.8)
            return false

        if(rect.width < 0.0001 || rect.width > 2)
            return false
        if(rect.height < 0.0001 || rect.height > 2)
            return false

        return true
    }

    function isColorRectValid(rect)
    {
        if(rect.x == 2.22507e-308 || rect.y == 2.22507e-308 || rect.width == 2.22507e-308 || rect.height == 2.22507e-308)
            return false
        if(rect.toString() === "QRectF(0, 0, 0, 2.22507e-308)")
            return false
        if(rect.x > 0 && rect.x < 0.00001)
            return false
        if(rect.y > 0 && rect.y < 0.00001)
            return false
        if(rect.width > 0 && rect.width < 0.00001)
            return false
        if(rect.height > 0 && rect.height < 0.00001)
            return false

        return true
    }
    function getHexStrColor(position, propertyName)
    {
        var colorRect = filter.getRectOfTextFilter(propertyName)

        console.log("propertyName = ", propertyName, " colorRect =", colorRect)

        if (position >= 0)
        {
            colorRect = filter.getAnimRectValue(position, propertyName)
        }

        if(isColorRectValid(colorRect) === false)
            return "invalid"

        var aStr = getColorStr(colorRect.x)
        var rStr = getColorStr(colorRect.y)
        var gStr = getColorStr(colorRect.width)
        var bStr = getColorStr(colorRect.height)

        return "#" + aStr + rStr + gStr + bStr
    }

    function getRectColor(hexStrColor)
    {
        if(hexStrColor.length != 7 && hexStrColor.length != 9)
            return Qt.rect(255, 255, 255, 255)

        var aStr = "FF",  rStr = "FF", gStr = "FF", bStr = "FF";

        if (hexStrColor.length == 7)
        {
            aStr = "FF"
            rStr = hexStrColor.substring(1, 3)
            gStr = hexStrColor.substring(3, 5)
            bStr = hexStrColor.substring(5)
        }
        else
        {
            aStr = hexStrColor.substring(1, 3)
            rStr = hexStrColor.substring(3, 5)
            gStr = hexStrColor.substring(5, 7)
            bStr = hexStrColor.substring(7)
        }

        return Qt.rect(parseInt(aStr, 16), parseInt(rStr, 16), parseInt(gStr, 16), parseInt(bStr, 16))
    }

    function getAbsoluteRect(position)
    {
        var rect = filter.getRectOfTextFilter(rectProperty)
        if (position >= 0)
        {
            rect = filter.getAnimRectValue(position, rectProperty)
        }
        return Qt.rect(Math.round(rect.x * profile.width), Math.round(rect.y * profile.height), Math.round(rect.width * profile.width), Math.round(rect.height * profile.height))
    }

    function getRelativeRect(absoluteRect)
    {
        return Qt.rect(absoluteRect.x / profile.width, absoluteRect.y / profile.height, absoluteRect.width / profile.width, absoluteRect.height / profile.height)
    }

    function getResetValue(property)
    {
        if(property == fgcolourProperty) return  Qt.rect(255.0, 255.0, 255.0, 255.0)
        if(property == bgcolourProperty) return  Qt.rect(0.0, 0.0, 0.0, 0.0)
        if(property == olcolourProperty) return  Qt.rect(255.0, 0.0, 0.0, 0.0)
        if(property == rectProperty)      return Qt.rect(0.0148437, 0.441667, 0.948438, 0.195833)

        if(property == 'trans_fix_rotate_x')        return  0.0
        if(property == 'trans_scale_x')             return  1.0
        if(property == 'trans_scale_aspect_ratio')  return  1.0
        if(property == 'trans_ox')                  return  0.0
        if(property == 'trans_oy')                  return  0.0
        if(property == 'transparent_alpha')         return  1.0
        if(property == 'size')                      return  filterRect.height

        return 0
    }

    function updateFilter(currentProperty, value)
    {
        if (blockUpdate === true) {
            return
        }

        filter.set(currentProperty, value)
    }

    Component.onCompleted: {
        console.log("Component.onCompleted")

        if (filter.isNew) {
            filter.set('size', filterRect.height)
        }

        setControls()
        keyframe.initFilter(idGridLayoutAnimation)
    }

    function setControls()
    {
        textArea.text = filter.get('argument')
        fontButton.text = filter.get('family')
        weightCombo.currentIndex = weightCombo.valueToIndex()
        insertFieldCombo.currentIndex = insertFieldCombo.valueToIndex()
        blockUpdate = true
        filterRect = getAbsoluteRect(-1)
        fgColor.value = getHexStrColor(-1, fgcolourProperty)
        outlineColor.value = getHexStrColor(-1, olcolourProperty)
        outlineSpinner.value = filter.getDouble(outlineProperty)
        letterSpaceing.value = filter.getDouble(letterSpaceingProperty)
        bgColor.value = getHexStrColor(-1, bgcolourProperty)
        padSpinner.value = filter.getDouble(padProperty)

        idShearX.value          = filter.getDouble('shear_x')
        idShadowDistance.value  = filter.getDouble('shadow_distance')
        idShadowAngle.value     = filter.getDouble('shadow_angle')
        idShadowRadius.value    = filter.getDouble('shadow_radius')
        idShadowColor.value     = getHexStrColor(-1, 'shadow_color')

        idGlowEnable.checked      = (filter.get('glow_enable') != 0)

        idGlowColor.value       = getHexStrColor(-1, 'glow_color')
        idGlowRadius.value    = filter.getDouble('glow_radius')

//        rotationSlider.value            = filter.getDouble('trans_fix_rotate_x')
//        scaleSlider.value               =  100.0/filter.getDouble('trans_scale_x')
//        xOffsetSlider.value             = filter.getDouble('trans_ox') * -1
//        yOffsetSlider.value             = filter.getDouble('trans_oy') * -1
//        scaleAspectRatioSlider.value    =  filter.getDouble('trans_scale_aspect_ratio')
//        transparentAlphaSlider.value    =  filter.getDouble('transparent_alpha')

        blockUpdate = false
        var align = filter.get(halignProperty)
        if (align === 'left')
            leftRadioButton.checked = true
        else if (align === 'center' || align === 'middle')
            centerRadioButton.checked = true
        else if (filter.get(halignProperty) === 'right')
            rightRadioButton.checked = true
        align = filter.get(valignProperty)
        if (align === 'top')
            topRadioButton.checked = true
        else if (align === 'center' || align === 'middle')
            middleRadioButton.checked = true
        else if (align === 'bottom')
            bottomRadioButton.checked = true
    }

    function setFilter()
    {
        if (blockUpdate === true)
        {
            return
        }
        var x = parseFloat(rectX.text)
        var y = parseFloat(rectY.text)
        var w = parseFloat(rectW.text)
        var h = parseFloat(rectH.text)
        if (x !== filterRect.x ||
            y !== filterRect.y ||
            w !== filterRect.width ||
            h !== filterRect.height)
            {
            filterRect.x = x
            filterRect.y = y
            filterRect.width = w
            filterRect.height = h

            updateFilter(rectProperty, getRelativeRect(filterRect))
        }
    }

    ExclusiveGroup { id: sizeGroup }
    ExclusiveGroup { id: halignGroup }
    ExclusiveGroup { id: valignGroup }

    SystemPalette { id: activePalette; colorGroup: SystemPalette.Active }

    YFKeyFrame
    {
        id: keyframe
        onRefreshUI:
        {
            keyframe.updateParamsUI(idGridLayoutAnimation)
        }
        onSyncUIDataToProject:
        {
            keyframe.syncDataToProject(idGridLayoutAnimation)
        }
    }

    GridLayout {
        id: idGridLayoutPreset
        width:parent.width
        columns: 5
        rows:3
       // anchors.fill: parent
        Layout.fillWidth: true
        anchors.margins: 8
        rowSpacing : 25

        Label {
            text: qsTr('Text')
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            color: activePalette.text//'#ffffff'
            font.bold:true
//            font.pointSize:13
        }
        TextArea {
            id: textArea
            Layout.columnSpan: 3
            textFormat: TextEdit.PlainText
            textColor: "#b9b9b9"//activePalette.text
            // 消除 warning：
            // ScrollView.qml: QML : Possible anchor loop detected on fill.
            // If you set a width, consider using TextEdit.Wrap.
            wrapMode: TextEdit.Wrap
            Layout.minimumHeight: 40
            Layout.maximumHeight: 100
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

                filter.set('argument', text)
            }
        }
        Label{}//占位符
        Label{}//占位符

//        Label {
//            text: qsTr('Insert')
//            Layout.alignment: Qt.AlignLeft
//            color: activePalette.text//'#ffffff'
//        }
        MyComboBox {
            id: insertFieldCombo
            Layout.columnSpan: 3
            width: 100
            height: 32
            Layout.minimumHeight: 32
            Layout.maximumHeight: 32
            Layout.minimumWidth: preset.width
            Layout.maximumWidth: preset.width
            listModel: ListModel {
                ListElement {name: qsTr('default')}
                ListElement {name: qsTr('Timecode')}
                ListElement {name: qsTr('Frame #', 'Frame number')}
                ListElement {name: qsTr('File date')}
                ListElement {name: qsTr('File name')}
            }
            property var values: ['welcome!', '#timecode#', '#frame#', '#localfiledate#', '#resource#']
            function valueToIndex() {
                var textStr = filter.get('argument')
                for (var i = 0; i < values.length; ++i)
                    if (values[i] === textStr) break;
                if (i === values.length) i = 0;
                return i;
            }
            onCurrentIndexChanged: {
                textArea.insert(textArea.cursorPosition, values[currentIndex])
            }
        }
        Label{}//占位符

        Label {
            text: qsTr('Style')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
            font.bold:true
//            font.pointSize:13
            //font.underline:true

        }

        Preset {
            id: preset
            Layout.columnSpan: 4
            parameters: [rectProperty, 'shear_x', halignProperty, valignProperty, 'size', //'argument',
            fgcolourProperty, 'family', 'weight', olcolourProperty, outlineProperty, bgcolourProperty, padProperty, letterSpaceingProperty,
            'shadow_distance', 'shadow_angle', 'shadow_radius', 'shadow_color', 'glow_enable', 'glow_radius', 'glow_color']

            m_strType: "tst"
            onBeforePresetLoaded:
            {
            }
            onPresetSelected:
            {
                //更新界面
                setControls()

                //？？？不清楚这句有什么作用
                if (filter.isNew) {
                    filter.set('size', filterRect.height)
                }
            }
        }

        Button {
            id: detailButton1
            text: qsTr('Style >')
            tooltip: qsTr('detail')
            Layout.columnSpan: 1
            Layout.alignment: Qt.AlignLeft
            style: ButtonStyle {
                background: Rectangle {
                    border.color: activePalette.text
                    implicitWidth: 80
                    implicitHeight: 20
                    border.width: control.activeFocus ? 2 : 1
                    color: activePalette.text
                }
                label: Label {
                    text: detailButton1.text//qsTr('Style >')
                    color: "#b9b9b9"
                }
            }
            onClicked:
            {
                if(idGridLayoutText.visible == false)
                {
                    idGridLayoutText.anchors.topMargin = 20
                    idGridLayoutText.visible = true
                    idGridLayoutAnimationProfile.anchors.top = idGridLayoutPreset.bottom //idGridLayoutPreset.bottom// idSeparatorTextStyleEnd.bottom //
                    //采用idSeparatorTextStyleEnd.bottom 没有作用，不知道什么原因，可能是初始隐藏了
                    idGridLayoutAnimationProfile.anchors.topMargin = 680
                    detailButton1.text = 'Style ^'
                }
                else
                {
                    idGridLayoutText.visible = false
                    idGridLayoutText.anchors.topMargin = 1000
                    idGridLayoutAnimationProfile.anchors.top = idGridLayoutPreset.bottom
                    idGridLayoutAnimationProfile.anchors.topMargin = 70
                    detailButton1.text = 'Style >'
                }
            }

        }
        SeparatorLine {
            Layout.columnSpan: 4
            Layout.minimumWidth: parent.width
            Layout.maximumWidth: parent.width
        }
    }

    GridLayout {
        id: idGridLayoutText
        width:parent.width - 24

        visible:false
        Layout.fillWidth: true
        columns: 5
        anchors.top:idGridLayoutPreset.bottom
        anchors.topMargin: 1000

        anchors.margins: 12
        anchors.left : parent.left
        anchors.leftMargin: 12
        rowSpacing : 25

        Label
        {
            text: qsTr('Font')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        RowLayout {
            Layout.columnSpan: 4

            ColorPicker {
                id: fgColor
                eyedropper: false
                alpha: true
                onCancel: {
                }
                onTemporaryColorChanged: {
                }
                onValueChanged:
                {
                    console.log("fgcolourProperty = ", value, "rectcolor=", getRectColor(value))
                    updateFilter(fgcolourProperty, getRectColor(value))
                }
            }

            Button {
                id: fontButton
                Layout.minimumHeight: 32
                Layout.maximumHeight: 32
                Layout.minimumWidth: (preset.width - 8) / 2 - 8 - fgColor.width
                Layout.maximumWidth: (preset.width - 8) / 2 - 8 - fgColor.width
                onClicked: {
                    fontDialog.font = Qt.font({ family: filter.get('family'), pointSize: 24, weight: Font.Normal })
                    fontDialog.open()
                }
                FontDialog {
                    id: fontDialog
                    title: "Please choose a font"
                    onFontChanged: {
                         filter.set('family', font.family)
                    }
                    onAccepted: fontButton.text = font.family
                    onRejected: {
                         filter.set('family', fontButton.text)
                    }
                }
            }

            MyComboBox {
                id: weightCombo
                Layout.minimumHeight: 32
                Layout.maximumHeight: 32
                Layout.minimumWidth: (preset.width - 8) / 2
                Layout.maximumWidth: (preset.width - 8) / 2
                listModel: ListModel {
                    ListElement {name: qsTr('Normal')}
                    ListElement {name: qsTr('Bold')}
                    ListElement {name: qsTr('Light', 'thin font stroke')}
                }
                property var values: [Font.Normal, Font.Bold, Font.Light]
                function valueToIndex() {
                    var w = filter.getDouble('weight')
                    for (var i = 0; i < values.length; ++i)
                        if (values[i] === w) break;
                    if (i === values.length) i = 0;
                    return i;
                }
                onCurrentIndexChanged: {
                     filter.set('weight', 10 * values[currentIndex])
                }
           }
        }

        Label
        {
            text: qsTr('Shear X')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        SliderSpinner {
            Layout.columnSpan: 3
            objectName: 'shearXSlider'
            id: idShearX
            minimumValue: -1
            maximumValue: 1
            decimals: 1
            spinnerWidth: 80

            onValueChanged:{
                updateFilter('shear_x', value.toString())
            }
        }
         UndoButton
         {
            Layout.columnSpan: 1
            onClicked: idShearX.value = 0
        }



        Label {
            text: qsTr('Letter Spaceing')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        MySpinBox {
            id: letterSpaceing
            Layout.minimumWidth: preset.width
            Layout.maximumWidth: preset.width
            Layout.columnSpan: 4
            minimumValue: 0
            maximumValue: 500
            horizontalAlignment:Qt.AlignLeft
            decimals: 0
            onValueChanged: {
                updateFilter(letterSpaceingProperty, value.toString())
            }
        }

        Label {
            text: qsTr('Outline')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        ColorPicker {
            id: outlineColor
            eyedropper: false
            alpha: true
            onCancel: {
            }
            onTemporaryColorChanged: {
            }
            onValueChanged: {
                updateFilter(olcolourProperty, getRectColor(value))
            }
        }
        Label {
            text: qsTr('Thickness')
            Layout.alignment: Qt.AlignRight
            color: activePalette.text//'#ffffff'
        }
        MySpinBox {
            id: outlineSpinner
            Layout.minimumWidth: 150
            Layout.maximumWidth: 150
            Layout.columnSpan: 2
            minimumValue: 0
            maximumValue: 30
            horizontalAlignment:Qt.AlignLeft
            decimals: 0
            onValueChanged: {
                updateFilter(outlineProperty, value.toString())
            }
        }

        Label {
            text: qsTr('Background')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        ColorPicker {
            id: bgColor
            eyedropper: false
            alpha: true
            onCancel: {
            }
            onTemporaryColorChanged: {
            }
            onValueChanged: {
                updateFilter(bgcolourProperty, getRectColor(value))
            }
        }
        Label {
            text: qsTr('Padding')
            Layout.alignment: Qt.AlignRight
            color: activePalette.text//'#ffffff'
        }
        MySpinBox {
            id: padSpinner
            Layout.minimumWidth: 150
            Layout.maximumWidth: 150
            Layout.columnSpan: 2
            minimumValue: 0
            maximumValue: 100
            horizontalAlignment:Qt.AlignLeft
            decimals: 0
            onValueChanged: {
                updateFilter(padProperty, value.toString())
            }
        }

        Label
        {
            text: qsTr('Shadow')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        Label
        {
            text: qsTr('Distance')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        MySpinBox
        {
            id: idShadowDistance
            Layout.minimumWidth: preset.width/4
            Layout.maximumWidth: preset.width/4
            Layout.columnSpan: 1
            minimumValue: 0
            maximumValue: 30
            horizontalAlignment:Qt.AlignLeft
            decimals: 0
            onValueChanged:
            {
                updateFilter('shadow_distance', value.toString())
            }
        }

        Label
        {
            text: qsTr('Angle')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        MySpinBox
        {
            id: idShadowAngle
            Layout.minimumWidth: preset.width/4
            Layout.maximumWidth: preset.width/4
            Layout.columnSpan: 1
            minimumValue: 0
            maximumValue: 360
            horizontalAlignment:Qt.AlignLeft
            decimals: 0
            onValueChanged:
            {
                updateFilter('shadow_angle', value.toString())
            }
        }

        Label
        {
            text: qsTr('')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        Label
        {
            text: qsTr('Color')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }

        ColorPicker
        {
                id: idShadowColor
                Layout.columnSpan: 1
                eyedropper: false
                alpha: true
                onCancel: {
                }
                onTemporaryColorChanged: {
                }
                onValueChanged:
                {
                    updateFilter("shadow_color", getRectColor(value))
                }
        }

        Label
        {
            text: qsTr('Blur')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        MySpinBox
        {
            id: idShadowRadius
            Layout.minimumWidth: preset.width/4
            Layout.maximumWidth: preset.width/4
            Layout.columnSpan: 1
            minimumValue: 0
            maximumValue: 130
            horizontalAlignment:Qt.AlignLeft
            decimals: 0
            onValueChanged:
            {
                updateFilter('shadow_radius', value.toString())
            }
        }

        Label
        {
            text: qsTr('Glow')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        CheckBox {
            id: idGlowEnable
            Layout.columnSpan: 1
            checked: filter.get('glow_enable') != 0

            onCheckedChanged:
            {
                if(idGlowEnable.checked)
                    filter.set('glow_enable', '1')
                else
                    filter.set('glow_enable', '0')
            }
        }

        ColorPicker
        {
                id: idGlowColor
                Layout.columnSpan: 1
                eyedropper: false
                alpha: true
                onCancel: {
                }
                onTemporaryColorChanged: {
                }
                onValueChanged:
                {
                    updateFilter("glow_color", getRectColor(value))
                }
        }

         Label
        {
            text: qsTr('Range')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        MySpinBox
        {
            id: idGlowRadius
            Layout.minimumWidth: preset.width/4
            Layout.maximumWidth: preset.width/4
            Layout.columnSpan: 1
            minimumValue: 0
            maximumValue: 130
            horizontalAlignment:Qt.AlignLeft
            decimals: 0
            onValueChanged:
            {
                updateFilter('glow_radius', value.toString())
           }
        }

        SeparatorLine {
            Layout.columnSpan: 5
            Layout.minimumWidth: parent.width
            Layout.maximumWidth: parent.width
            width: parent.width
        }

        Label {
            text: qsTr('Position')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        RowLayout {
            Layout.columnSpan: 4
            TextField {
                id: rectX
                text: filterRect.x.toFixed()
                Layout.minimumWidth: (preset.width - 8) / 2 - 10
                Layout.maximumWidth: (preset.width - 8) / 2 - 10
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
                textColor: "#b9b9b9"
            }
            Label {
                text: ','
                color: activePalette.text//'#ffffff'
            }
            TextField {
                id: rectY
                text: filterRect.y.toFixed()
                Layout.minimumWidth: (preset.width - 8) / 2 - 10
                Layout.maximumWidth: (preset.width - 8) / 2 - 10
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
                textColor: "#b9b9b9"
            }
        }
        Label {
            text: qsTr('Size')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        RowLayout {
            Layout.columnSpan: 4
            TextField {
                id: rectW
                text: filterRect.width.toFixed()
                Layout.minimumWidth: (preset.width - 8) / 2 - 10
                Layout.maximumWidth: (preset.width - 8) / 2 - 10
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
                textColor: "#b9b9b9"
            }
            Label {
                text: 'x'
                color: activePalette.text//'#ffffff'
            }
            TextField {
                id: rectH
                text: filterRect.height.toFixed()
                Layout.minimumWidth: (preset.width - 8) / 2 - 10
                Layout.maximumWidth: (preset.width - 8) / 2 - 10
                horizontalAlignment: Qt.AlignRight
                onEditingFinished: setFilter()
                textColor: "#b9b9b9"
            }
        }

        Label {
            text: qsTr('X fit')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        RadioButton {
            id: leftRadioButton
            text: qsTr('Left')
            exclusiveGroup: halignGroup
            onClicked:
            {
                filter.set(halignProperty, 'left')
            }
        }
        RadioButton {
            id: centerRadioButton
            text: qsTr('Center')
            exclusiveGroup: halignGroup
            Layout.leftMargin: 30
            onClicked: {
                filter.set(halignProperty, 'center')
            }
        }
        RadioButton {
            id: rightRadioButton
            text: qsTr('Right')
            exclusiveGroup: halignGroup
            Layout.leftMargin: 60
            onClicked:{
                filter.set(halignProperty, 'right')
            }
        }
        Item { Layout.fillWidth: true }

        Label {
            text: qsTr('Vertical fit')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
        }
        RadioButton {
            id: topRadioButton
            text: qsTr('Top')
            exclusiveGroup: valignGroup
            onClicked:
            {
                filter.set(valignProperty, 'top')
            }
        }
        RadioButton {
            id: middleRadioButton
            text: qsTr('Middle')
            exclusiveGroup: valignGroup
            Layout.leftMargin: 30
            onClicked:
            {
                filter.set(valignProperty, 'middle')
            }
        }
        RadioButton {
            id: bottomRadioButton
            text: qsTr('Bottom')
            exclusiveGroup: valignGroup
            Layout.leftMargin: 60
            onClicked:
            {
                filter.set(valignProperty, 'bottom')
            }
        }

        SeparatorLine {
            id: idSeparatorTextStyleEnd
            Layout.columnSpan: 5
            Layout.minimumWidth: parent.width
            Layout.maximumWidth: parent.width
            width: parent.width
        }
    }

    GridLayout {
        id: idGridLayoutAnimationProfile
        width:parent.width

        visible:true
        Layout.fillWidth: true
        columns: 5
        anchors.top:idGridLayoutPreset.bottom
        anchors.topMargin: 70
        anchors.margins: 8
        rowSpacing : 25

        Label {
            text: qsTr('Animation')
            Layout.alignment: Qt.AlignLeft
            color: activePalette.text//'#ffffff'
            font.bold:true
//            font.pointSize:13
        }
        Preset
        {
            id: presetPositionAnimation
            Layout.columnSpan: 4
            parameters: [  'trans_fix_rotate_x', 'trans_scale_x', 'trans_scale_aspect_ratio', 'trans_ox', 'trans_oy', 'transparent_alpha']
            m_strType: "pan"
            onBeforePresetLoaded:
            {
                filter.removeAllKeyFrame()
            }
            onPresetSelected: {
                //更新界面
                keyframe.updateParamsUI(idGridLayoutAnimation)

                if (filter.isNew) {
                    filter.set('size', filterRect.height)
                }
                //presetPositionAnimation.presetCombo.currentIndex = 0
            } 
        } 

        Button {
            id: detailButtonAnimation
            text: qsTr('Animation >')

            tooltip: qsTr('detail')
            Layout.columnSpan: 1
            Layout.alignment: Qt.AlignLeft
            style: ButtonStyle {
                background: Rectangle {
                    border.color: activePalette.text
                    implicitWidth: 80
                    implicitHeight: 20
                    border.width: control.activeFocus ? 2 : 1
                    color: activePalette.text
                }
                label: Label {
                    text: detailButtonAnimation.text//qsTr('Style >')
                    color: "#b9b9b9"
                }
            }
            onClicked:
            {
                if(idGridLayoutAnimation.visible == false)
                {

                    idGridLayoutAnimation.anchors.topMargin = 20
                    idGridLayoutAnimation.visible = true
                    detailButtonAnimation.text = 'Animation ^'
                }
                else
                {
                    idGridLayoutAnimation.visible = false
                    idGridLayoutAnimation.anchors.topMargin = 1000
                    detailButtonAnimation.text = 'Animation >'
                }
            }
        }

        SeparatorLine {
            Layout.columnSpan: 4
            Layout.minimumWidth: parent.width
            Layout.maximumWidth: parent.width
            width: parent.width
        }
    }

    GridLayout {
        id: idGridLayoutAnimation
        width:parent.width -24

        visible:false
        Layout.fillWidth: true
        columns: 5
        anchors.top:idGridLayoutAnimationProfile.bottom
        anchors.topMargin: 20
        anchors.margins: 12
        anchors.left : parent.left
        anchors.leftMargin: 12
        rowSpacing : 25

        Label {
            Layout.columnSpan: 1
            Layout.alignment: Qt.AlignLeft
            text: qsTr('Rotation')
            color: activePalette.text//'#ffffff'
        }
        SliderSpinner {
            Layout.columnSpan: 3
            objectName: 'rotationSlider'
            id: rotationSlider
            minimumValue: -360
            maximumValue: 360
          //  decimals: 1
            spinnerWidth: 80
            suffix: qsTr(' °')//qsTr(' degree')
            onValueChanged:{
                keyframe.controlValueChanged(rotationSlider)
            }
        }
        UndoButton {
            //Layout.columnSpan: 1
            onClicked: rotationSlider.value = 0
        }

        Label {
            Layout.columnSpan: 1
            Layout.alignment: Qt.AlignLeft
            text: qsTr('Scale')
            color: activePalette.text//'#ffffff'
        }
        SliderSpinner {
            Layout.columnSpan: 3
            objectName: 'scaleSlider'
            id: scaleSlider
            minimumValue: 0.1
            maximumValue: 500
            decimals: 1
            spinnerWidth: 80
            suffix: ' %'
            onValueChanged: {
                    keyframe.controlValueChanged(scaleSlider)
            }
        }
        UndoButton {
            Layout.columnSpan: 1
            onClicked: scaleSlider.value = 100 //
        }

        Label
        {
            Layout.columnSpan: 1
            Layout.alignment: Qt.AlignLeft
            text: qsTr('Aspect')
            color: activePalette.text//'#ffffff'
        }
        SliderSpinner
        {
            Layout.columnSpan: 3
            objectName: 'scaleAspectRatioSlider'
            id: scaleAspectRatioSlider
            minimumValue: 0.2
            maximumValue: 5
            decimals: 1
            spinnerWidth: 80

            onValueChanged:
            {
                keyframe.controlValueChanged(scaleAspectRatioSlider)
            }
        }
        UndoButton
        {
            onClicked: scaleAspectRatioSlider.value = 1
        }

        Label
        {
            Layout.columnSpan: 1
            Layout.alignment: Qt.AlignLeft
            text: qsTr('X offset')
            color: activePalette.text//'#ffffff'
        }

        SliderSpinner {
            Layout.columnSpan: 3
            objectName: 'xOffsetSlider'
            id: xOffsetSlider
            minimumValue: -1000
            maximumValue: 1000
            spinnerWidth: 80
            onValueChanged:{
                keyframe.controlValueChanged(xOffsetSlider)
            }
        }
        UndoButton {
            Layout.columnSpan: 1
            onClicked: xOffsetSlider.value = 0
        }

        Label {
            Layout.columnSpan: 1
            Layout.alignment: Qt.AlignLeft
            text: qsTr('Y offset')
            color: activePalette.text//'#ffffff'
        }
        SliderSpinner {
            Layout.columnSpan: 3
            objectName: 'yOffsetSlider'
            id: yOffsetSlider
            minimumValue: -1000
            maximumValue: 1000
            spinnerWidth: 80
            onValueChanged:{
                keyframe.controlValueChanged(yOffsetSlider)
            }
        }
        UndoButton {
            Layout.columnSpan: 1
            onClicked: yOffsetSlider.value = 0
        }


       Label
        {
            Layout.columnSpan: 1
            Layout.alignment: Qt.AlignLeft
            text: qsTr('Transparent')
            color: activePalette.text//'#ffffff'
        }
        SliderSpinner
        {
            Layout.columnSpan: 3
            objectName: 'transparentAlphaSlider'
            id: transparentAlphaSlider
            minimumValue: 0
            maximumValue: 1
            decimals: 2
            spinnerWidth: 80

            onValueChanged:
            {
                keyframe.controlValueChanged(transparentAlphaSlider)
            }
        }
        UndoButton
        {
            onClicked: transparentAlphaSlider.value = 1
        }

//        Item { Layout.fillWidth: true }

//        Item { Layout.fillHeight: true }
    }


//    Connections
//    {
//        target: filter
//        onFilterPropertyValueChanged:
//        {
//            console.log("---filter onChanged---")

//            var position        = timeline.getPositionInCurrentClip()
//            var newRect         = getAbsoluteRect(-1)
//            var keyFrameCount   = filter.getKeyFrameCountOnProject(rectProperty);
//            //判断是否有关键帧
//            if(keyFrameCount > 0)
//            {
//                newRect = getAbsoluteRect(position)
//            }

//            if (isRectPosValid(newRect) && filterRect !== newRect)
//            {
//                filterRect = newRect
//                filter.set('size', filterRect.height)
//            }
//        }
//    }

}
