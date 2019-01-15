
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Item {
    width: 300
    height: 250
    property string fromParameter: 'from'
    property string toParameter: 'to'
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            combo.currentIndex = 0
        } else {
            // Initialize parameter values
            combo.currentIndex = filter.get(fromParameter) * 1
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8

        RowLayout {
            Label { text: qsTr('Copy from')
            color: '#ffffff'
            }
            ComboBox {
                id: combo
                model: [qsTr('Left to right'), qsTr('Right to left')]
                onCurrentIndexChanged: {
                    filter.set(fromParameter, currentIndex)
                    filter.set(toParameter, 1 - currentIndex)
                }
            }
        }
        Item {
            Layout.fillHeight: true;
        }
    }
}
