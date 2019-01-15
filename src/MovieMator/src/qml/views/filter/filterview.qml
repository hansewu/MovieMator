
import QtQuick 2.1
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: root

    
    signal currentFilterRequested(int attachedIndex)
    
    function clearCurrentFilter() {
        if (filterConfig.item) {
            filterConfig.item.width = 1
            filterConfig.item.height = 1
        }
        filterConfig.source = ""
    }
    
    function setCurrentFilter(index) {
        attachedFilters.setCurrentFilter(index)
        removeButton.selectedIndex = index
        filterConfig.source = metadata ? metadata.qmlFilePath : ""
    }

    color: '#353535'//activePalette.window
    width: 400

    onWidthChanged: _setLayout()
    onHeightChanged: _setLayout()
    
    function _setLayout() {
        if (height > width - 200) {
            root.state = "landscape"
        } else {
            root.state = "landscape"
        }
    }
    
    SystemPalette { id: activePalette }
    
    FilterMenu {
        id: filterMenu

        height: 400
        onFilterSelected: {
            attachedfiltersmodel.add(metadatamodel.get(index))
        }
    }

    Rectangle {
        id: titleBackground
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: titleLabel.bottom
            topMargin: 10
            leftMargin: 10
            rightMargin: 10
        }
        color: activePalette.highlight
        visible: attachedfiltersmodel.producerTitle != ""
    }

    Label {
        id: titleLabel
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: 10
            leftMargin: 10
            rightMargin: 10
        }
        text: attachedfiltersmodel.producerTitle
        elide: Text.ElideLeft
        color: activePalette.highlightedText
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
    }


    GridLayout {
        id: attachedContainer
        columns: 3
        anchors {
            top: titleBackground.bottom
            left: parent.left
            leftMargin: 10
            rightMargin: 10
            topMargin: 6
            bottomMargin: 4
        }


        AttachedFilters {
            id: attachedFilters
            Layout.columnSpan: 3
            Layout.fillWidth: true
            Layout.fillHeight: true
            onFilterClicked: {
                root.currentFilterRequested(index)
            }
            Label {
                anchors.centerIn: parent
                text: qsTr("Nothing selected")
                color: activePalette.text
                visible: !attachedfiltersmodel.isProducerSelected
            }
        }

        CustomFilterButton {
            id:addButton


            enabled: attachedfiltersmodel.ready
            opacity: enabled ? 1.0 : 0.5
            tooltip: qsTr('Add a filter')

            implicitWidth: 32
            implicitHeight: 32

            customIconSource: 'qrc:///icons/light/32x32/list-add.png'
            onClicked: {
                filterMenu.popup(addButton)
            }
        }

        CustomFilterButton {
            id:removeButton
            property int selectedIndex: -1
            enabled: selectedIndex > -1 ? true : false
            opacity: enabled ? 1.0 : 0.5
            tooltip: qsTr('Remove selected filter')

            implicitWidth: 32
            implicitHeight: 32

            customIconSource: 'qrc:///icons/light/32x32/list-remove.png'
            onClicked: {
                attachedfiltersmodel.remove(selectedIndex)
            }
        }

        Item {
            Layout.fillWidth: true
        }
    }

    ScrollView {
        id: filterConfigScrollView

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
                corner: Item {
                    implicitWidth: 14
                    implicitHeight: 14
                }

            }

        function expandWidth() {

            if (filterConfig.item) {
                filterConfig.item.width =
                    Math.max(filterConfig.minimumWidth,
                             filterConfigScrollView.width /* scroll bar */)
            }
        }
        onWidthChanged: expandWidth()
        Loader {
            id: filterConfig
            property int minimumWidth: 0
            onLoaded: {
                minimumWidth = item.width
                filterConfigScrollView.expandWidth()
            }
        }
    }

    KeyFrameAnimation {
        id: keyFrameAnimation
        anchors {
            top: attachedContainer.bottom
            left: parent.left
            leftMargin: 10
            topMargin: 0
        }
    }


    states: [
        State {
            name: "landscape"  // 左右结构
            AnchorChanges {
                target: filterConfigScrollView
                anchors {
                    top: titleBackground.bottom
                    bottom: attachedContainer.bottom //root.bottom
                    left: attachedContainer.right
                    right: root.right
                }
            }
            PropertyChanges {
                target: attachedContainer; width: 200
                height: root.height - addButton.height * 2
            }
        },
        State {
            name: "portrait"  //上下结构
            AnchorChanges {
                target: filterConfigScrollView
                anchors {
                    top: attachedContainer.bottom
                    bottom: root.bottom
                    left: root.left
                    right: root.right
                }
            }
            PropertyChanges {
                target: attachedContainer
                width: titleBackground.width
                height: 165
            }
        }
    ]
}
