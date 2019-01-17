import QtQuick 2.1
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0


Rectangle {
    width: 500
    height: 400
    color: "white"


    ListModel {
        id: fruitModel

        ListElement {
            name: "Text1"
            thumbnail: "./2.jpeg"
        }
        ListElement {
            name: "Text2"
            thumbnail: "./2.jpeg"
        }
        ListElement {
            name: "Text3"
            thumbnail: "./2.jpeg"
        }
    }

    GridView {
        anchors.fill: parent
        anchors.margins: 20
        clip: true
        model: textmetadatamodel
        cellWidth: 100
        cellHeight: 100
        currentIndex: -1
        delegate: numberDelegate

        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        focus: true


    }

    Component {
        id: numberDelegate
        Rectangle {
            id: container
            width: 80
            height: 80
            color: "red"//transparent"

            Drag.active: dragArea.drag.active;
            Drag.supportedActions: Qt.CopyAction;
            Drag.dragType: Drag.Automatic;
         //   Drag.mimeData: {"color": color, "width": width, "height": height};


            Column {

                anchors.fill: parent

                Image {
                 //   id: textThumbnail
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 60
                    height: 60
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: thumbnail

                }

                Text {
                  //  id: textName
                    width:80
                    height:20
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 10
                    text: name
                }

            }


            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton
                drag.target: parent;

                //                onClicked: {
                //                    container.GridView.view.currentIndex = index
                //                    container.forceActiveFocus()
                //                 //   textManagerWidget.previewText(metadatamodel.get(index))
                //                    if(parent.Drag.supportedActions == Qt.MoveAction){
                //                        container.x = 0;
                //                        container.y = 0;
                //                    }
                //                }
                onReleased: {
                    console.log("******** onReleased is called");
                    if(parent.Drag.supportedActions == Qt.CopyAction){
                        container.x = 0;
                        container.y = 0;
                    }
                }

            }

            MouseArea {
                anchors.fill: parent
                enabled: true
                acceptedButtons: Qt.RightButton
                onClicked: {
                    console.log("******** onClicked is called");
                    menu.popup()
                }
            }



            Menu {
                id: menu
                MenuItem {
                    text: qsTr('Add to timeline')
                    onTriggered: {
                        textManagerWidget.addTextToTimeline(metadatamodel.get(index))
                    }
                }
                MenuItem {
                    text: qsTr('Preview')
                    onTriggered: {}
                }
            }
        }
    }






}
