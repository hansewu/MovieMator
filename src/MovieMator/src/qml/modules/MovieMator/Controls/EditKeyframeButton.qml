import QtQuick.Controls 1.0

Button
{
    property int nParameterIndex

    implicitWidth: 20
    implicitHeight: 20

    onClicked:
    {
        filter.emitEditKeyframeOfParameter(nParameterIndex)
    }
}

