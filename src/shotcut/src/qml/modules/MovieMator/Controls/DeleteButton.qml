import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Button {
//    iconName: 'document-save'
    iconSource: enabled?'qrc:///icons/light/32x32/list-remove.png':'qrc:///icons/light/32x32/list-remove-disable.png'
    style: ButtonStyle {
                padding {
                    left: 0
                    right: 0
                    top: 0
                    bottom: 0
                }
    }

    implicitWidth: 20
    implicitHeight: 20
}
