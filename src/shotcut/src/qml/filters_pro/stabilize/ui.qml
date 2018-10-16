
import QtQuick 2.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0
import com.moviemator.qml 1.0

Item {
    id: stabilizeRoot
    width: 300
    height: 250
    property string settingsSavePath: settings.savePath
    property bool bEnableControls: keyFrame.bKeyFrame  ||  (!filter.getKeyFrameNumber())


    Component.onCompleted: {
            shakinessSlider.value = filter.getDouble('shakiness')
            accuracySlider.value = filter.getDouble('accuracy')
            button.enabled = !hasAnalysisCompleted()
            setStatus(false)

        var keyFrameCount = filter.getKeyFrameCountOnProject("anim-shakiness");
        if(keyFrameCount > 0)
        {
            var index=0
            for(index=0; index<keyFrameCount;index++)
            {
                var nFrame = filter.getKeyFrameOnProjectOnIndex(index, "anim-shakiness");
                var keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-shakiness");
                filter.setKeyFrameParaValue(nFrame, "shakiness", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-accuracy");
                filter.setKeyFrameParaValue(nFrame, "accuracy", keyValue)

                keyValue = filter.getKeyValueOnProjectOnIndex(index, "anim-Zoom");
                filter.setKeyFrameParaValue(nFrame, "Zoom", keyValue)
            }
            filter.combineAllKeyFramePara();
        }
    }

    function hasAnalysisCompleted() {
            return (filter.get("results").length > 0 &&
                    filter.get("filename").indexOf(filter.get("results")) !== -1)
        }

    function setKeyFrameValue(bKeyFrame)
    {
        var nFrame = keyFrame.getCurrentFrame();
        if(bKeyFrame)
        {

            var contrastValue = shakinessSlider.value;
            filter.setKeyFrameParaValue(nFrame,"shakiness", contrastValue.toString() )

            contrastValue = accuracySlider.value;
            filter.setKeyFrameParaValue(nFrame, "accuracy", contrastValue.toString())

            contrastValue = zoomSlider.value;
            filter.setKeyFrameParaValue(nFrame, "zoom", contrastValue.toString())
            filter.setKeyFrameParaValue(nFrame, "refresh", "1")

            filter.combineAllKeyFramePara();
        }
        else
        {
            //Todo, delete the keyframe date of the currentframe
            filter.removeKeyFrameParaValue(nFrame);
            if(!filter.getKeyFrameNumber())
            {
                filter.anim_set("shakiness","")
                filter.anim_set("accuracy","")
                filter.anim_set("zoom","")
                filter.anim_set("refresh","")
                
            }
            filter.set('shakiness', shakinessSlider.value)
            filter.set('accuracy', accuracySlider.value)
            filter.set('zoom', zoomSlider.value)
            filter.set("refresh", 1);
        }
    }

    function setStatus( inProgress ) {
        if (inProgress) {
            status.text = qsTr('Analyzing...')
        }
        else if (filter.get("results").length > 0 && 
                 filter.get("filename").indexOf(filter.get("results")) !== -1) {
            status.text = qsTr('Analysis complete.')
        }
        else
        {
            status.text = qsTr('Click Analyze to use this filter.')
        }
    }

    // This signal is used to workaround context properties not available in
    // the FileDialog onAccepted signal handler on Qt 5.5.
    signal fileSaved(string filename)
    onFileSaved: {
        var lastPathSeparator = filename.lastIndexOf('/')
        if (lastPathSeparator !== -1) {
            settings.savePath = filename.substring(0, lastPathSeparator)
        }
    }

    Connections {
        target: filter
        onAnalyzeFinished: {
            filter.set("reload", 1);
            setStatus(false)
            button.enabled = true
        }
    }
    
    FileSaveDialog {
        id: fileDialog
        title: qsTr( 'Select a file to store analysis results.' )
//        modality: Qt.WindowModal
//        selectExisting: false
//        selectMultiple: false
//        selectFolder: false
//        folder: settingsSavePath
        nameFilters: [ "Stabilize Results (*.stab)" ]
//        selectedNameFilter: "Stabilize Results (*.stab)"
        filename: "stabilize_analyze.stab"
        onAccepted: {
            var filename = fileDialog.fileUrl.toString()
            // Remove resource prefix ("file://")
            filename = filename.substring(7)
            if (filename.substring(2, 4) == ':/') {
                // In Windows, the prefix is a little different
                filename = filename.substring(1)
            }
            stabilizeRoot.fileSaved(filename)

            var extension = ".stab"
            // Force file extension to ".stab"
            var extIndex = filename.indexOf(extension, filename.length - extension.length)
            if (extIndex == -1) {
                filename += ".stab"
            }
            filter.set('filename', filename)
            filter.getHash()
            setStatus(true)
            filter.analyze();
        }
        onRejected: {
            button.enabled = true
        }
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        KeyFrame{
             id: keyFrame
             Layout.columnSpan:3
        // 	currentPosition: filterDock.getCurrentPosition()
             onSetAsKeyFrame:
             {
                setKeyFrameValue(bKeyFrame)
             }

             onLoadKeyFrame:
             {
                 var stablizeValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "shakiness");
                 if(stablizeValue != -1.0)
                 {
                     shakinessSlider.value = stablizeValue
                 }

                 stablizeValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "accuracy");
                 if(stablizeValue != -1.0)
                 {
                      accuracySlider.value = stablizeValue
                 }

                 stablizeValue = filter.getKeyFrameParaDoubleValue(keyFrameNum, "zoom");
                 if(stablizeValue != -1.0)
                 {
                      zoomSlider.value = stablizeValue
                 }

             }
         }

        Label {
            text: qsTr('<b>Analyze Options</b>')
            Layout.columnSpan: 3
            color: bEnableControls?'#ffffff': '#828282'
        }

        Label {
            text: qsTr('Shakiness')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            enabled: bEnableControls
            id: shakinessSlider
            minimumValue: 1
            maximumValue: 10
            tickmarksEnabled: true
            stepSize: 1
            value: filter.getDouble('shakiness')
            onValueChanged:
            {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame,"shakiness", value.toString() )
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('shakiness', value)

              //  setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                shakinessSlider.value = 4
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {
            text: qsTr('Accuracy')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: accuracySlider
            enabled: bEnableControls
            minimumValue: 1
            maximumValue: 15
            tickmarksEnabled: true
            stepSize: 1
            value: filter.getDouble('accuracy')
            onValueChanged:{
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame,"accuracy", value.toString() )
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('accuracy', value)

               // setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                accuracySlider.value = 4
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Label {
            text: qsTr('<b>Filter Options</b>')
            Layout.columnSpan: 3
            color: bEnableControls?'#ffffff': '#828282'
        }

        Label {
            text: qsTr('Zoom')
            Layout.alignment: Qt.AlignRight
            color: bEnableControls?'#ffffff': '#828282'
        }
        SliderSpinner {
            id: zoomSlider
            enabled: bEnableControls
            minimumValue: -50
            maximumValue: 50
            decimals: 1
            suffix: ' %'
            value: filter.getDouble('zoom')
            onValueChanged: {
                if(keyFrame.bKeyFrame)
                {
                    var nFrame = keyFrame.getCurrentFrame();
                    filter.setKeyFrameParaValue(nFrame,"zoom", value.toString() )
                    filter.combineAllKeyFramePara()
                }
                else
                    filter.set('zoom', value)
               // setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }
        UndoButton {
            enabled: bEnableControls
            onClicked: {
                zoomSlider.value = 0
                setKeyFrameValue(keyFrame.bKeyFrame)
            }
        }

        Button {
            id: button
            enabled: bEnableControls
            text: qsTr('Analyze')
            Layout.alignment: Qt.AlignRight
            onClicked: {
                button.enabled = false
             //   fileDialog.folder = settings.savePath
                fileDialog.open()
            }
        }
        Label {
            id: status
            Layout.columnSpan: 2
            color: bEnableControls?'#ffffff': '#828282'
            Component.onCompleted: {
                setStatus(false)
            }
        }

        Item {
            Layout.fillHeight: true;
        }
    }
}
