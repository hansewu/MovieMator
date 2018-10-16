
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property string paramRed: '0'
    property string paramGreen: '1'
    property string paramBlue: '2'
    property string paramAction: '3'
    property var defaultParameters: [paramRed, paramGreen, paramBlue, paramAction]
    width: 300
    height: 250
    
    function loadWheels() {
        wheel.color = Qt.rgba(filter.getDouble(paramRed),
                              filter.getDouble(paramGreen),
                              filter.getDouble(paramBlue),
                              1.0)
    }
    
    Component.onCompleted: {
        if (filter.isNew)
            filter.savePreset(defaultParameters)
        modeCombo.currentIndex = filter.getDouble(paramAction) * 2
        loadWheels()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8

        Preset {
            parameters: defaultParameters
            onPresetSelected: {
                modeCombo.currentIndex = filter.getDouble(paramAction) * 2
                loadWheels()
            }
        }

        RowLayout {
            Label {
                text: qsTr('Mode')
                color: '#ffffff'

            }
            ComboBox {
                id: modeCombo
                Layout.minimumWidth: 200
                model: [qsTr('Shadows (Lift)'), qsTr('Midtones (Gamma)'), qsTr('Highlights (Gain)')]
                onCurrentIndexChanged: filter.set(paramAction, currentIndex / 2)
            }
        }

        ColorWheelItem {
            id: wheel
            Layout.columnSpan: 2
            implicitWidth: (Math.min(parent.width, parent.height) - 60) * 1.1
            implicitHeight: Math.min(parent.width, parent.height) - 60
            Layout.alignment : Qt.AlignCenter | Qt.AlignTop
            Layout.minimumHeight: 75
            Layout.maximumHeight: 300
            onColorChanged: {
                filter.set(paramRed, wheel.red / 255.0);
                filter.set(paramGreen, wheel.green / 255.0);
                filter.set(paramBlue, wheel.blue / 255.0);
            }
        }

        Item {
            Layout.fillHeight: true;
        }
    }
}
