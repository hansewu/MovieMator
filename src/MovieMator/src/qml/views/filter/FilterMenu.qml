
import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import 'FilterMenu.js' as Logic
import com.moviemator.qml 1.0 as MovieMator

Window {
    id: filterWindow
    visible: false

    property bool hasFocus: activeFocusItem != null

    signal filterSelected(int index)

    function popup(triggerItem) {
        var menuRect = Logic.calcMenuRect(triggerItem, toolBar.height + 2)
        filterWindow.x = menuRect.x
        filterWindow.y = menuRect.y
        filterWindow.height = menuRect.height
        filterWindow.show()
        filterWindow.requestActivate()
    }

    color: 'transparent'//#323232'//Qt.darker(activePalette.window, 1.5) // Border color
    flags: Qt.Sheet
    width: 250
    height: 200
    onHasFocusChanged: if (!hasFocus) filterWindow.close()

    SystemPalette {
        id: activePalette

    }

    Rectangle {
        id: menuColorRect


        anchors.fill: parent
        anchors.margins: 1
        color: '#525252'//activePalette.base
        radius: 5

        Column {
            anchors.fill: parent
//            anchors.left: parent.left
//            anchors.right: parent.right
           anchors.margins: 6
//            anchors.leftMargin: 10
//            anchors.rightMargin: 1
//            anchors.topMargin: 10
//            anchors.bottomMargin: 10
           spacing: 2

            ScrollView {
                id: scrollView
                width: parent.width
                height: parent.height - toolBar.height - 6

                ListView {
                    id: menuListView


                    function itemSelected(index) {
                            filterWindow.close()
                            filterSelected(index)
                    }

                    anchors.fill: parent
                    model: metadatamodel
                    delegate: FilterMenuDelegate {}
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
                id: toolBar
                height: 30
                width: parent.width

                ExclusiveGroup { id: typeGroup }

                ToolButton {
                    id: favButton
                    implicitWidth: 28
                    implicitHeight: 28
                    checkable: true
                    checked: true
                  //  iconName: 'bookmarks'
                    iconSource: 'qrc:///icons/light/32x32/bookmarks-highlight.png'
                    tooltip: qsTr('Show favorite filters')

                    exclusiveGroup: typeGroup
                    onCheckedChanged: {
                        if (checked) {
                            metadatamodel.filter = MovieMator.MetadataModel.FavoritesFilter
                            favButton.iconSource = 'qrc:///icons/light/32x32/bookmarks-highlight.png'

                            vidButton.iconSource = 'qrc:///icons/light/32x32/video-television.png'
                            audButton.iconSource = 'qrc:///icons/light/32x32/speaker.png'

                        }

                    }
                }
                ToolButton {
                    id: vidButton
                    implicitWidth: 28
                    implicitHeight: 28
                    checkable: true
              //      iconName: 'video-television'
                    iconSource: 'qrc:///icons/light/32x32/video-television.png'
                    tooltip: qsTr('Show video filters')
                    exclusiveGroup: typeGroup
                    onCheckedChanged: {
                        if (checked) {
                            metadatamodel.filter = MovieMator.MetadataModel.VideoFilter
                            vidButton.iconSource = 'qrc:///icons/light/32x32/video-television-highlight.png'
                            favButton.iconSource = 'qrc:///icons/light/32x32/bookmarks.png'
                            audButton.iconSource = 'qrc:///icons/light/32x32/speaker.png'
                        }
                    }
                }
                ToolButton {
                    id: audButton
                    implicitWidth: 28
                    implicitHeight: 28
                    checkable: true
                 //   iconName: 'speaker'
                    iconSource: 'qrc:///icons/light/32x32/speaker.png'
                    tooltip: qsTr('Show audio filters')
                    exclusiveGroup: typeGroup
                    onCheckedChanged: {
                        if (checked) {
                            metadatamodel.filter = MovieMator.MetadataModel.AudioFilter
                            audButton.iconSource = 'qrc:///icons/light/32x32/speaker-highlight.png'

                            vidButton.iconSource = 'qrc:///icons/light/32x32/video-television.png'
                            favButton.iconSource = 'qrc:///icons/light/32x32/bookmarks.png'

                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
                ToolButton {
                    id: closeButton
                    implicitWidth: 28
                    implicitHeight: 28
                 //   iconName: 'window-close'
                    iconSource: 'qrc:///icons/light/32x32/window-close.png'
                    tooltip: qsTr('Close menu')
                    onClicked: filterWindow.close()
                }
            }
        }
    }
}
