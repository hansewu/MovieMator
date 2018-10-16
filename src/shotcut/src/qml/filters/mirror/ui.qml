
import QtQuick 2.1

Item {
    width: 300
    height: 250

    Component.onCompleted: {
        if (filter.get("mlt_service").indexOf("movit.") == 0 ) {
            // Movit filter
        } else {
            filter.set('mirror', 'flip')
        }
    }
}
