import QtQuick 2.1
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: root

    color: '#353535'    //activePalette.window
    width: 400

    SystemPalette { id: activePalette }

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
        visible: titleLabel.text != ""
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
        text: "TemplateEditor"      // 模板名称
        elide: Text.ElideLeft
        color: activePalette.highlightedText
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
    }

    Rectangle {
        id: editorDock
        anchors {
            top: titleBackground.bottom
            left: titleBackground.left
            topMargin: 10
        }
        height: parent.height -40
        width: parent.width -20
        color: "#525252"     // activePalette.window

        GridView {
            id: gridView
            anchors.fill: parent
            anchors {
                topMargin: 20
                leftMargin: 20
                bottomMargin: 20
            }
            boundsBehavior: GridView.StopAtBounds
            clip: true
            focus: true
            model: templateEditorDock.count()
            delegate: gridComponent
            cellWidth: 120
            cellHeight: cellWidth + 10        
        }

        Component {
            id: gridComponent
            Rectangle {
                id: delegateRect
                property alias filename: fileName
                property alias thumbimg: thumbImg
                height: 100
                width: height
                color: "transparent"    //activePalette.window
                Rectangle {
                    id: background
                    height: 80
                    width: parent.width
                    anchors.centerIn: parent
                    color: delegateRect.GridView.isCurrentItem ? "#C0482C" : "transparent"    // "transparent"
                    Image { // 缩略图
                        id: thumbImg
                        anchors.centerIn: parent
                        source: "image://editorThumbnail/" + templateEditorDock.filePath(index)
                    }
                }
                Label { // 文件名
                    id: fileName
                    height: 25
                    width: parent.width
                    anchors.top:background.bottom
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: delegateRect.GridView.isCurrentItem ? "#C0482C" : "white"   //activePalette.windowText
                    font.pixelSize: 16
                    text: templateEditorDock.fileName(index)
                    elide: Text.ElideMiddle
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            delegateRect.GridView.view.currentIndex = index;
                        }
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        chooseButton.visible = true;
                        removeButton.visible = true;
                    }
                    onExited: {
                        chooseButton.visible = false;
                        removeButton.visible = false;
                    }
                    onClicked: {
                        delegateRect.GridView.view.currentIndex = index;
                    }
                }
                DropArea {
                    anchors.fill: parent
                    onDropped: {
                        // 替换文件，更新文件名、缩略图等
                        delegateRect.GridView.view.currentIndex = index;
                        if(drop.hasUrls) {
                            // 从文件夹中拖放
                            templateEditorDock.replaceFile(0, index, drop.urls[0]);
                        }
                        else if(drop.formats.indexOf('application/mlt+xml') >= 0) {
                            // 从历史记录中拖放
                            templateEditorDock.replaceFile(1, index, drop.getDataAsString('application/mlt+xml'));
                        }
                    }
                }
                Button { // 选择文件按钮
                    id: chooseButton
                    visible: false
                    anchors.top: background.top
                    anchors.right: background.right
                    height: 20
                    width: 30
                    text: "+"
                    onClicked: {
                        delegateRect.GridView.view.currentIndex = index;
                        templateEditorDock.chooseFile(index);
                    }
                    onHoveredChanged: {
                        chooseButton.visible = hovered;
                        removeButton.visible = hovered;
                    }
                }
                Button { // 移除文件按钮
                    id: removeButton
                    visible: false
                    anchors.bottom: background.bottom
                    anchors.right: background.right
                    height: 20
                    width: 30
                    text: "-"
                    onClicked: {
                       delegateRect.GridView.view.currentIndex = index;
                       templateEditorDock.remove(index);
                    }
                    onHoveredChanged: {
                       chooseButton.visible = hovered;
                       removeButton.visible = hovered;
                    }
                }
                Connections {
                    target: templateEditorDock
                    onRefreshThumbnail: {
                        if(delegateRect.GridView.view.currentIndex===index){
                            // 只改变选中item的fileName的text，currentItem只能访问property
                            delegateRect.GridView.view.currentItem.filename.text = templateEditorDock.fileName(index);
                            delegateRect.GridView.view.currentItem.thumbimg.source = "image://editorThumbnail/" + templateEditorDock.filePath(index);
                        }
                    }
                }
            }
        }
        Connections {
            target: templateEditorDock
            onRefreshView: {
                gridView.model = 0;
                gridView.model = templateEditorDock.count();
            }
        }
    }
}

