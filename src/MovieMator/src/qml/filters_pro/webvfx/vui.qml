import QtQuick 2.1
import MovieMator.Controls 1.0

Flickable {
    property string rectProperty: 'transition.rect_anim_relative'
    property string fillProperty: 'transition.fill'
    property string distortProperty: 'transition.distort'
    property string halignProperty: 'transition.halign'
    property string valignProperty: 'transition.valign'
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

    function getAspectRatio() 
    {
        return (filter.get(fillProperty) === '1' && filter.get(distortProperty) === '0')? filter.producerAspect : 0.0
    }

    function toRelativeRect(rect)
    {
        rect.x = rect.x / profile.width * 100
        rect.y = rect.y / profile.height * 100
        rect.width = rect.width /  profile.width * 100
        rect.height = rect.height /  profile.height * 100
        return rect
    }

    Component.onCompleted: {
        var rectT = filter.getRect(rectProperty)
        
        rectT.x = rectT.x * profile.width
        rectT.y = rectT.y * profile.height
        rectT.width = rectT.width * profile.width
        rectT.height = rectT.height * profile.height

        rectangle.setHandles(rectT)
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
            onWidthScaleChanged: setHandles(toRelativeRect(filter.getRect(rectProperty)))
            onHeightScaleChanged: setHandles(toRelativeRect(filter.getRect(rectProperty)))
            onRectChanged:  {
                filterRect.x = rect.x / rectangle.widthScale / profile.width
                filterRect.y = rect.y / rectangle.heightScale / profile.height
                filterRect.width = rect.width / rectangle.widthScale / profile.width
                filterRect.height = rect.height / rectangle.heightScale / profile.height
                
                filter.set(rectProperty, filterRect)
            }
        }
    }

    // Connections {
    //     target: filter
    //     onChanged: {
    //         var newRect = filter.getRect(rectProperty)
    //         if (filterRect !== newRect) {
    //             filterRect = newRect
    //             rectangle.setHandles(toRelativeRect(filterRect))
    //         }
    //         if (rectangle.aspectRatio !== getAspectRatio()) {
    //             rectangle.aspectRatio = getAspectRatio()
    //             rectangle.setHandles(toRelativeRect(filterRect))
    //             var rect = rectangle.rectangle
    //             filter.set(rectProperty, '%1%/%2%:%3%x%4%'
    //                        .arg((Math.round(rect.x / rectangle.widthScale)).toLocaleString(_locale))
    //                        .arg((Math.round(rect.y / rectangle.heightScale)).toLocaleString(_locale))
    //                        .arg((Math.round(rect.width / rectangle.widthScale)).toLocaleString(_locale))
    //                        .arg((Math.round(rect.height / rectangle.heightScale)).toLocaleString(_locale)))
    //         }
    //     }
    // }
}
