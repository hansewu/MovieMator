
import QtQuick 2.1
import MovieMator.Controls 1.0

Flickable {
    property string fillProperty: 'fill'
    property string rectProperty: 'geometry'
    property string halignProperty: 'valign'
    property string valignProperty: 'halign'
    property var _locale: Qt.locale(application.numericLocale)

    width: 300
    height: 250
    interactive: false
    clip: true
    property real zoom: (video.zoom > 0)? video.zoom : 1.0
    property rect filterRect: filter.getRect(rectProperty)
    contentWidth: video.rect.width * zoom
    contentHeight: video.rect.height * zoom
    contentX: video.offset.x
    contentY: video.offset.y

    function getAspectRatio() {
        return (filter.get(fillProperty) === '1')? filter.producerAspect : 0.0
    }

    Component.onCompleted: {
        if (filter.isNew) {
            filter.set('size', filterRect.height)
        }
        rectangle.setHandles(filter.getRect(rectProperty))
    }

    MouseArea {
        anchors.fill: parent
        onClicked: textEdit.focus = false
    }
    DropArea { anchors.fill: parent }

    Item {
        id: videoItem
        x: video.rect.x
        y: video.rect.y
        width: video.rect.width
        height: video.rect.height
        scale: zoom

        Rectangle {
            visible: false // DISABLED FOR NOW
            anchors.fill: textEdit
            color: 'white'
            opacity: textEdit.opacity * 0.5
        }
        TextEdit {
            visible: false // DISABLED FOR NOW
            id: textEdit
            x: Math.round(filterRect.x * rectangle.widthScale) + rectangle.handleSize
            y: Math.round(filterRect.y * rectangle.heightScale) + rectangle.handleSize
            width: Math.round(filterRect.width * rectangle.widthScale) - 2 * rectangle.handleSize
            height: Math.round(filterRect.height * rectangle.heightScale) - 2 * rectangle.handleSize
            horizontalAlignment: (filter.get('halign') === 'left')? TextEdit.AlignLeft
                               : (filter.get('halign') === 'right')? TextEdit.AlignRight
                               : TextEdit.AlignHCenter
            verticalAlignment: (filter.get('valign') === 'top')? TextEdit.AlignTop
                             : (filter.get('valign') === 'bottom')? TextEdit.AlignBottom
                             : TextEdit.AlignVCenter
            text: filter.get('argument')
            font.family: filter.get('family')
            font.pixelSize: 24 //0.85 * height / text.split("\n").length
            textMargin: filter.get('pad')
            opacity: activeFocus
            onActiveFocusChanged: filter.set('disable', activeFocus)
            onTextChanged: filter.set('argument', text)
        }

        RectangleControl {
            id: rectangle
            widthScale: video.rect.width / profile.width
            heightScale: video.rect.height / profile.height
            aspectRatio: getAspectRatio()
            handleSize: Math.max(Math.round(8 / zoom), 4)
            borderSize: Math.max(Math.round(1.33 / zoom), 1)
            onWidthScaleChanged: setHandles(filter.getRect(rectProperty))
            onHeightScaleChanged: setHandles(filter.getRect(rectProperty))
            onRectChanged:  {
                filterRect.x = Math.round(rect.x / rectangle.widthScale)
                filterRect.y = Math.round(rect.y / rectangle.heightScale)
                filterRect.width = Math.round(rect.width / rectangle.widthScale)
                filterRect.height = Math.round(rect.height / rectangle.heightScale)
                filter.set(rectProperty, '%1%/%2%:%3%x%4%'
                           .arg((filterRect.x / profile.width * 100).toLocaleString(_locale))
                           .arg((filterRect.y / profile.height * 100).toLocaleString(_locale))
                           .arg((filterRect.width / profile.width * 100).toLocaleString(_locale))
                           .arg((filterRect.height / profile.height * 100).toLocaleString(_locale)))
//                filter.set(rectProperty, filter.getRect(rectProperty))
                filter.set('size', filterRect.height)
            }
        }
    }

    Connections {
        target: filter
        onChanged: {
            var newRect = filter.getRect(rectProperty)
            if (filterRect !== newRect) {
                filterRect = newRect
                rectangle.setHandles(filterRect)
                filter.set('size', filterRect.height)
            }
            if (rectangle.aspectRatio !== getAspectRatio()) {
                rectangle.aspectRatio = getAspectRatio()
                rectangle.setHandles(filterRect)
                var rect = rectangle.rectangle
                filter.set(rectProperty, '%1%/%2%:%3%x%4%'
                           .arg((Math.round(rect.x / rectangle.widthScale) / profile.width * 100).toLocaleString(_locale))
                           .arg((Math.round(rect.y / rectangle.heightScale) / profile.height * 100).toLocaleString(_locale))
                           .arg((Math.round(rect.width / rectangle.widthScale) / profile.width * 100).toLocaleString(_locale))
                           .arg((Math.round(rect.height / rectangle.heightScale) / profile.height * 100).toLocaleString(_locale)))
//                filter.set(rectProperty, filter.getRect(rectProperty))
            }
        }
    }
}
