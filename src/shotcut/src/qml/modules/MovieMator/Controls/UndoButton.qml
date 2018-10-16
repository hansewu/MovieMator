
import QtQuick.Controls 1.0

Button {
 //   iconName: 'edit-undo'
    iconSource: enabled?'qrc:///icons/light/32x32/toolbar-undo.png':'qrc:///icons/light/32x32/toolbar-undo-disable.png'
    tooltip: qsTr('Reset to default')
    implicitWidth: 20
    implicitHeight: 20
}
