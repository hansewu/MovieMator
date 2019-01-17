
import QtQuick 2.1
import MovieMator.Controls 1.0

Flickable {
    property string rectProperty
    property string fillProperty
    property string distortProperty
    property string halignProperty
    property string valignProperty
    property var _locale: Qt.locale(application.numericLocale)

    width: 400
    height: 200
    interactive: false
    clip: true
    property real zoom: (video.zoom > 0)? video.zoom : 1.0
    property rect filterRect: filter.getRect(rectProperty)
    contentWidth: video.rect.width * zoom
    contentHeight: video.rect.height * zoom
    contentX: video.offset.x
    contentY: video.offset.y

    function getAspectRatio() {
        return (filter.get(fillProperty) === '1' && filter.get(distortProperty) === '0')? filter.producerAspect : 0.0
    }

    Component.onCompleted: {
        rectangle.setHandles(filter.getRect(rectProperty))
    }

    DropArea { anchors.fill: parent }

    Item {
        id: videoItem
        x: video.rect.x
        y: video.rect.y
        width: video.rect.width
        height: video.rect.height
        scale: zoom

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
            }
        }
    }
}
