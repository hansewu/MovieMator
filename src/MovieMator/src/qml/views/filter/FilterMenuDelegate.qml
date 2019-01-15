
import QtQuick 2.2
import QtQuick.Controls 1.1
import 'FilterMenu.js' as Logic

Item {
    id: wrapper

    
    visible: isVisible
    height: visible ? Logic.ITEM_HEIGHT : 0

    Behavior on height {
        NumberAnimation { duration: 200 }
    }

    SystemPalette { id: activePalette }

    Row {
        height: Logic.ITEM_HEIGHT

        
        ToolButton {
            id: favButton
            implicitWidth: 20
            implicitHeight: 18
            anchors.verticalCenter: parent.verticalCenter
            opacity: favorite ? 1.0 : 0.3
          //  iconName: 'bookmarks'
            iconSource: 'qrc:///icons/light/32x32/bookmarks.png'
            onClicked: {
                favorite = !favorite
            }
        }
        
        Rectangle {
            id: itemBackground
            color: '#525252'//activePalette.base
            width: wrapper.ListView.view.width - favButton.width
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            
            ToolButton {
                id: itemIcon
                implicitWidth: 20
                implicitHeight: 18
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                enabled: false
                iconName: needsGpu ? 'cpu' : isAudio ? 'speaker' : 'video-television'
                iconSource: needsGpu ? 'qrc:///icons/oxygen/32x32/devices/cpu.png' : isAudio ? 'qrc:///icons/oxygen/32x32/actions/speaker.png' : 'qrc:///icons/oxygen/32x32/devices/video-television.png'
            }
            
            Label {
                id: itemText
                anchors.left: itemIcon.right
                anchors.right: itemBackground.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                text: name
                color: !freeVersion && !filterDock.proVersion ? activePalette.dark : '#ffffff' //activePalette.text//'#ffffff'//activePalette.text
                verticalAlignment: Text.AlignVCenter
            }
            
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: wrapper.height > 0
                onClicked: {
                    if (!freeVersion && !filterDock.proVersion) {
                        filterDock.promptForUpgrade()
                    } else {
                        wrapper.ListView.view.itemSelected(index)
                    }

                }
                onEntered: {
                    itemBackground.color = '#3594e5'//activePalette.highlight
                    itemText.color = activePalette.highlightedText
                }
                onExited: {
                    itemBackground.color = '#525252'//activePalette.base
                    itemText.color = !freeVersion && !filterDock.proVersion ? activePalette.dark : '#ffffff' //activePalette.text
                }
            }
        }
    }
}
