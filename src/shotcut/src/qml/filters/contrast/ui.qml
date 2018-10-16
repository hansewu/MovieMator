
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import MovieMator.Controls 1.0

Item {
    property var defaultParameters: ['gamma_r', 'gamma_g', 'gamma_b', 'gain_r', 'gain_g', 'gain_b']
    property double gammaFactor: 2.0
    property double gainFactor: 2.0
    width: 300
    height: 250
    
    function setControls() {
        contrastSlider.value = filter.getDouble("gamma_r") / gammaFactor * 100.0
    }
    
    Component.onCompleted: {
        if (filter.isNew) {
            // Set default parameter values
            filter.set("gamma_r", 1.0);
            filter.set("gamma_g", 1.0);
            filter.set("gamma_b", 1.0);
            filter.set("gain_r", 1.0);
            filter.set("gain_g", 1.0);
            filter.set("gain_b", 1.0);
            filter.savePreset(defaultParameters)
        }
        setControls()
    }

    GridLayout {
        columns: 3
        anchors.fill: parent
        anchors.margins: 8

        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        Preset {
            Layout.columnSpan: 2
            parameters: defaultParameters
            onPresetSelected: {
                setControls()
            }
        }

        Label {
            text: qsTr('Contrast')
            Layout.alignment: Qt.AlignRight
            color: '#ffffff'
        }
        SliderSpinner {
            id: contrastSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            spinnerWidth: 80
            suffix: ' %'
            onValueChanged: {
                var v = value / 100.0
                filter.set("gamma_r", (1.0 - v) * gammaFactor)
                filter.set("gamma_g", (1.0 - v) * gammaFactor)
                filter.set("gamma_b", (1.0 - v) * gammaFactor)
                filter.set("gain_r",  v * gainFactor)
                filter.set("gain_g",  v * gainFactor)
                filter.set("gain_b",  v * gainFactor)
            }
        }
        UndoButton {
            onClicked: contrastSlider.value = 50
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
