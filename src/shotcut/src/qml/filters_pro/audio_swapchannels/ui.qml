
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Item {
    Component.onCompleted: {
        filter.set('swap', 1)
        filter.set('from', 0)
        filter.set('top', 1)
    }
}
