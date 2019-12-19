import QtQuick.Controls 1.0

Button
{
    property string strIndentifierOfParameter

    implicitWidth: 20
    implicitHeight: 20

    onClicked:
    {
        filter.emitEditKeyframeOfParameter(strIndentifierOfParameter)
    }
}

