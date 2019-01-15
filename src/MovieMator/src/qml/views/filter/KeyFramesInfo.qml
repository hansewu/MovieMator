
import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Window {
    id: keyFramesInfoWindow
    visible: false

    color: 'transparent'
    flags: Qt.Sheet
    width: 200
    height: 160

    property bool hasFocuss: activeFocusItem != null
    onHasFocussChanged: if (!hasFocuss) keyFramesInfoWindow.close()

    function popup(triggerItem) {
        keyFramesInfoWindow.x = parent.x + parent.width
        keyFramesInfoWindow.y = parent.y + parent.height
        keyFramesInfoWindow.height = 300
        keyFramesInfoWindow.show()
        keyFramesInfoWindow.requestActivate()

        updateKeyFramesModel(-1)
    }

//    signal keyFrameClicked(int index)

    SystemPalette {
        id: activePalette
    }


    function updateKeyFramesModel(selectedIndex)
    {
        keyFramesModel.clear()

//        var nFrameCount = filter.getKeyFrameNumber()
        var paramCount = metadata.keyframes.parameterCount
        if (paramCount <= 0) return

        var key             = metadata.keyframes.parameters[0].property
        var keyFrameCount   = filter.getKeyFrameCountOnProject(key);
        if(keyFrameCount > 0)
        {
            var index = 0
            for(index = 0; index < keyFrameCount; index++)
            {
                var nFrame      = filter.getKeyFrameOnProjectOnIndex(index, key);
                var keyValue    = filter.getKeyValueOnProjectOnIndex(index, key);
                var sel         = (index == selectedIndex) ? true : false

                keyFramesModel.append({frame: nFrame, value: keyValue, selected: sel})
            }
        }

    }

    Rectangle {
        id: keyframeInfoRect

        anchors.fill: parent
        anchors.margins: 6
        color: '#525252'
        radius: 5


        Column {
            anchors.fill: parent
            anchors.margins: 6
            spacing: 2

            ScrollView {
                id: keyFramesScrollView
                width: parent.width
                height: parent.height - keyFrameInfosToolBar.height - 6

                ListView {
                    id: keyFramesListView


                    function keyFrameSelected(index) {
                            keyFramesInfoWindow.close()
//                            filterSelected(index)
                    }

                    anchors.fill: parent
                    model: keyFramesModel
                    delegate: keyFrameDelegate
                    boundsBehavior: Flickable.StopAtBounds
                    snapMode: ListView.SnapToItem
                    currentIndex: -1
                    focus: true
                }

                style: ScrollViewStyle {
                        transientScrollBars: false
                      //  scrollToClickedPosition:true
                        handle: Item {
                            implicitWidth: 14
                            implicitHeight: 14
                            Rectangle {
                                color: "#787878"
                                anchors.fill: parent
                                anchors.margins: 3
                                radius: 4
                            }
                        }
                        scrollBarBackground: Item {
                            implicitWidth: 14
                            implicitHeight: 14
                        }
                        decrementControl: Rectangle {
                                    implicitWidth: 0
                                    implicitHeight: 0
                        }
                        incrementControl: Rectangle {
                                    implicitWidth: 0
                                    implicitHeight: 0
                        }

                    }
            }

            Rectangle {
                id: separatorBar
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: 1
                color: Qt.darker(activePalette.window, 1.5)
            }

            RowLayout {
                id: keyFrameInfosToolBar
                height: 50
                width: parent.width

                ToolButton {
                    id: modifyFrameButton
//                    anchors.left:parent.left + 40
                    implicitWidth: 40
                    implicitHeight: 28
                    iconSource: 'qrc:///icons/light/32x32/bookmarks-highlight.png'
                    tooltip: qsTr('Modify Frame')

                    onClicked: {
//                        setKeyFrameValue()
                    }

                }
                Item {
                    Layout.fillWidth: true
                }
                ToolButton {
                    id: removeFrameButton
//                    anchors.right:parent.width - 40
                    implicitWidth: 40
                    implicitHeight: 28
                    iconSource: 'qrc:///icons/light/32x32/video-television.png'
                    tooltip: qsTr('Remove selected Frame')
                    onClicked: {
//                        setKeyFrameValue()
                    }
//                    onCheckedChanged: {
//                        if (checked) {
//                            metadatamodel.filter = MovieMator.MetadataModel.VideoFilter
//                        }
//                    }
                }
            }
        }
    }

    ListModel {
        id:keyFramesModel
    }

    Component {
        id: keyFrameDelegate

        Row {
            height: 30

            Rectangle {
            id: itemFrameBackground
            height: 30
            width: keyFramesListView.width
            color: selected ? '#3594e5' : '#525252' //"transparent"
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            ToolButton {
                id: frameKeyItemIcon
                implicitWidth: 50
                implicitHeight: 30
                anchors.left: itemFrameBackground.left
                anchors.verticalCenter: parent.verticalCenter
                enabled: true
                iconSource: 'qrc:///icons/light/32x32/video-television.png'
            }

            Label {
                id: frameKeyItemText
                anchors.left: frameKeyItemIcon.right
                anchors.right: itemFrameBackground.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                text:  qsTr('Key Frame : ') + frame//name""
                color: selected ? activePalette.highlightedText : '#ffffff'
                verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    ListView.currentIndex = index
                    updateKeyFramesModel(index)

                    ListView.keyFrameSelected(index)
                }
            }
        }
        }
    }
}
