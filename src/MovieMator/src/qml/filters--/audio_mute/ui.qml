
import QtQuick 2.1

Item {
    width: 300
    height: 250
    property string gainParameter: 'gain'
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
           // filter.set(gainParameter, 0)
           filter.anim_set("gain", "0=0;200=50;400=0")
        }
    }
}
