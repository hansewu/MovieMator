import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
RowLayout {
    TextField {
        id: rectX;
        text:"X";
        horizontalAlignment: Qt.AlignRight;
        onEditingFinished:{positionChanged(parent.objectName)}
    }
    Label {
        text: ',';
        color: '#ffffff';
    }
    TextField {
        id: rectY;
        text: "Y";
        horizontalAlignment: Qt.AlignRight;
        onEditingFinished:{positionChanged(parent.objectName)}
    }
}
