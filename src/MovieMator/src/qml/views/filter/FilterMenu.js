
var ITEM_HEIGHT = 30
 
function maxMenuHeight(pad) {
    // Calculate the max possible height of the menu
    var i = 0
    var visibleItems = 0;
    for (i = 0; i < metadatamodel.rowCount(); i++) {
        if (metadatamodel.isVisible(i))
            visibleItems++;
    }
    return (Math.min(Math.max(visibleItems, 5), 15) * ITEM_HEIGHT) + pad
}

function calcMenuRect(triggerItem, pad) {
    var result = Qt.rect(0, 0, 0, 0)
    var itemPos = triggerItem.mapToItem(null,0,0)
    var triggerPos = Qt.point(itemPos.x + view.pos.x, itemPos.y + view.pos.y)
    var mainWinRect = application.mainWinRect

    result.height = 300//Math.min(maxMenuHeight(pad), mainWinRect.height)
    
    // Calculate the y position
    result.y = mainWinRect.y+150//triggerPos.y - result.height / 2 // Ideal position is centered
//    if (result.y < mainWinRect.y) {
//        // Window would be higher than the application window. Move it down
//        result.y = mainWinRect.y
//    } else if (result.y + result.height > mainWinRect.y + mainWinRect.height) {
//        // Window would be lower than the application window. Move it up
//        result.y =  mainWinRect.y + mainWinRect.height - result.height
//    }
    
    // Calculate the x position
    result.x = mainWinRect.x+50//triggerPos.x
    
    result.y = mainWinRect.y + triggerItem.y - 50
  	result.x = mainWinRect.x + triggerItem.x + 50

    return result
}
